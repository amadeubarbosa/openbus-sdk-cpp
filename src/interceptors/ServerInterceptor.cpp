// -*- coding: iso-8859-1-unix -*-
#include "openbus/detail/interceptors/ServerInterceptor.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/detail/LoginCache.hpp"
#include "openbus/detail/openssl/PublicKey.hpp"
#include "openbus/detail/any.hpp"
#include "openbus/log.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <cstring>
#include <iostream>
#include <string>

namespace openbus { namespace interceptors {

const std::size_t LRUSize(128);

Session::Session(const std::string &login) 
  : remote_id(login)
{
  tickets_init(&tickets);
  secret = boost::uuids::random_generator()();
  id = static_cast<CORBA::ULong>(
    boost::uuids::hash_value(boost::uuids::random_generator()()));
}

ServerInterceptor::ServerInterceptor(ORBInitializer *orb_init)
  : _orb_init(orb_init),
    _sessionLRUCache(LRUSize),
    _bus_ctx(0)
{
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::ServerInterceptor");
}

CORBA::Any ServerInterceptor::attach_legacy_credential_rst(
  boost::shared_ptr<Session> session,
  Connection &conn,
  idl::OctetSeq &secret)
{
  legacy_idl_cr::CredentialReset rst;
  rst.session = session->id;
  rst.target = conn._login()->id;
  std::memcpy(rst.challenge, secret.get_buffer(), idl::EncryptedBlockSize);
  CORBA::Any any;
  any <<= rst;
  return any;
}

CORBA::Any ServerInterceptor::attach_credential_rst(
  boost::shared_ptr<Session> session,
  Connection &conn,
  idl::OctetSeq &secret)
{
  idl_cr::CredentialReset rst;
  rst.session = session->id;
  rst.target = conn._login()->id;
  rst.entity = conn._login()->entity;
  std::memcpy(rst.challenge, secret.get_buffer(), idl::EncryptedBlockSize);
  CORBA::Any any;
  any <<= rst;
  return any;
}
  
template <typename C>
void ServerInterceptor::send_credential_reset(
  Connection &conn,
  boost::shared_ptr<Login> caller,
  PI::ServerRequestInfo_ptr r) 
{
  CORBA::Any any;
  idl::OctetSeq secret;
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);
#endif
    boost::shared_ptr<Session> session(
      boost::shared_ptr<Session>(
        new Session(caller->loginInfo->id.in())));
    _sessionLRUCache.insert(session->id, session);
    secret = caller->pubKey->encrypt(session->secret.data, 
                                     boost::uuids::uuid::static_size());
    if (boost::is_same<C, idl_cr::CredentialData>::value)
      any = attach_credential_rst(session, conn, secret);
    else
      any = attach_legacy_credential_rst(session, conn, secret);
  }
  CORBA::OctetSeq_var o(_orb_init->codec->encode_value(any));
  IOP::ServiceContext serviceContext;
  serviceContext.context_id =
    boost::is_same<C, idl_cr::CredentialData>::value ?
    idl_cr::CredentialContextId : legacy_idl_cr::CredentialContextId;
  CORBA::OctetSeq s(o->length(), o->length(), o->get_buffer(), 0);
  serviceContext.context_data = s;
  r->add_reply_service_context(serviceContext, true);
  throw CORBA::NO_PERMISSION(idl_ac::InvalidCredentialCode, 
                             CORBA::COMPLETED_NO);              
}

template <typename C>
C ServerInterceptor::get_credential(PI::ServerRequestInfo_ptr r) const
{
  C credential;
  try 
  {
    IOP::ServiceContext_var sc(
      r->get_request_service_context(
        boost::is_same<C, idl_cr::CredentialData>::value ?
        idl_cr::CredentialContextId : legacy_idl_cr::CredentialContextId));
    CORBA::Any_var any(
      _orb_init->codec->decode_value(
        sc->context_data,
        boost::is_same<C, idl_cr::CredentialData>::value ?
        idl_cr::_tc_CredentialData : legacy_idl_cr::_tc_CredentialData));
    credential = extract<C>(any);
  }
  catch (const CORBA::BAD_PARAM &) 
  {    
    throw CORBA::NO_PERMISSION(idl_ac::NoCredentialCode, 
                               CORBA::COMPLETED_NO);
  }
  return credential;
}

void ServerInterceptor::save_dispatcher_connection(
  Connection &conn,
  PI::ServerRequestInfo_ptr r,
  OpenBusContext *ctx)
{
  size_t const bufSize(sizeof(Connection *));
  unsigned char buf[bufSize];
  Connection *_c(&conn);
  std::memcpy(buf, &_c, bufSize);
  idl::OctetSeq connectionAddrOctetSeq(bufSize, bufSize, buf);
  CORBA::Any connectionAddrAny;
  connectionAddrAny <<= connectionAddrOctetSeq;
  r->set_slot(_orb_init->current_connection, connectionAddrAny);
  ctx->setCurrentConnection(&conn);
}

Connection &ServerInterceptor::get_dispatcher_connection(
  OpenBusContext *ctx,
  const std::string &busid, 
  const std::string &login,
  PI::ServerRequestInfo_ptr r)
{
  Connection *conn(0);
  log_scope l(log().general_logger(), debug_level, 
              "ServerInterceptor::get_dispatcher");
  if (ctx->onCallDispatch())
  {
    try 
    {
      conn = ctx->onCallDispatch()(*ctx, busid, login, r->operation());
      if (conn && conn->busid() != busid)
      {
        throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
      }
    }
    catch (const std::exception &e) 
    {
      l.level_vlog(
        warning_level, "Falha na execucao da callback CallDispatch: %s",
        e.what() );
    }
    catch (...)
    {
      l.level_log(warning_level,
                  "Falha desconhecida na execucao da callback CallDispatch.");
    }
  }
  if (!conn)
  {
    if (!(conn = ctx->getDefaultConnection()))
    {
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    }
  }
  if ( (!conn->login() || (conn->busid() != busid)) )
  {
    throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
  } 
  assert(conn);
  save_dispatcher_connection(*conn, r, ctx);
  return *conn;
}

bool ServerInterceptor::check_legacy_chain(
  legacy_idl_ac::CallChain chain,
  boost::shared_ptr<Login> caller,
  Connection &conn,
  PI::ServerRequestInfo_ptr r)
{
  if (std::strcmp(chain.target, conn._login()->entity)) 
  { 
    send_credential_reset<legacy_idl_cr::CredentialData>(conn, caller, r);
  }
  else if (std::strcmp(chain.caller.id, caller->loginInfo->id)) 
  {
    return false;
  }
  return true;
}

bool ServerInterceptor::check_chain(
  idl_ac::CallChain chain,
  boost::shared_ptr<Login> caller,  
  Connection &conn)
{
  if (std::strcmp(chain.caller.id, caller->loginInfo->id)
      || std::strcmp(chain.bus.in(), conn.busid().c_str()))
  {
    return false;
  }
  return true;
}
    
template <typename C>
bool ServerInterceptor::validate_chain(
  C& cred,
  boost::shared_ptr<Login> caller,  
  Connection &conn,
  PI::ServerRequestInfo_ptr r)
{
  if (!cred.chain.encoded.length())
  {
    return false;
  }
  idl::HashValue hashChain;
  SHA256(cred.chain.encoded.get_buffer(),
         cred.chain.encoded.length(), hashChain);

  if (!conn._buskey->verify(cred.chain.signature, 
                            idl::EncryptedBlockSize, hashChain, 
                            idl::HashValueSize))
  {
    return false;
  }
  else
  {
    CORBA::OctetSeq o(cred.chain.encoded.maximum(),
                      cred.chain.encoded.length(),
                      cred.chain.encoded.get_buffer());
    CORBA::Any_var any(
      _orb_init->codec->decode_value(
        o,
        boost::is_same<C, idl_cr::CredentialData>::value ?
        idl_ac::_tc_CallChain : legacy_idl_ac::_tc_CallChain));

    if (boost::is_same<C, idl_cr::CredentialData>::value)
    {
      idl_ac::CallChain chain(extract<idl_ac::CallChain>(any));
      return check_chain(chain, caller, conn);
    }
    else
    {
      legacy_idl_ac::CallChain chain(extract<legacy_idl_ac::CallChain>(any));
      return check_legacy_chain(chain, caller, conn, r);
    }
  }
  return true;
}

template <typename C>
void ServerInterceptor::handle_credential(
  C &credential,
  PI::ServerRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::handle_credential");
  
  Connection &conn(
    get_dispatcher_connection(_bus_ctx, std::string(credential.bus),
                              std::string(credential.login), r));

  boost::shared_ptr<Login> caller;
  try 
  {
    l.vlog("Validando login: %s", credential.login.in());
    caller = conn._loginCache->validateLogin(credential.login.in());
  }
  catch (const CORBA::NO_PERMISSION &e) 
  {
    if (idl_ac::NoLoginCode == e.minor()) 
    {
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    }
  } 
  catch (const CORBA::Exception &) 
  {
    throw CORBA::NO_PERMISSION(idl_ac::UnverifiedLoginCode,
                               CORBA::COMPLETED_NO);
  }
  if (!caller) 
  {
    l.log("Lancando excecao InvalidLoginCode.");
    throw CORBA::NO_PERMISSION(idl_ac::InvalidLoginCode, CORBA::COMPLETED_NO);
  }
  l.log("Login valido.");
  hash_value hash;   
  boost::shared_ptr<Session> session;
  std::string remote_id;
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);
#endif
    session = _sessionLRUCache.fetch(credential.session);
    if (session)
    {
      boost::array<unsigned char, secret_size> secret;
      std::copy(session->secret.begin(), session->secret.end(), 
                secret.c_array());
      hash = ::openbus::hash<C>(
        r->operation(),
        credential.ticket,
        secret);
      remote_id = session->remote_id;
    }
  }
  if (!(session
        && !std::memcmp(hash.data(), credential.hash, 
                        idl::HashValueSize) 
        && !std::strcmp(remote_id.c_str(), credential.login) 
        && tickets_check(&session->tickets, 
                         credential.ticket))) 
  {
    l.level_vlog(debug_level, 
                 "credential not valid, try to reset credetial session");
    
    send_credential_reset<C>(conn, caller, r);
  }
  l.level_vlog(debug_level, "credential is valid");
  if (validate_chain<C>(credential, caller, conn, r))
  {
    CORBA::Any any;
    any <<= credential.chain;
    r->set_slot(_orb_init->signed_call_chain, any);
  }
  else
  {
    throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
  }
}
    
void ServerInterceptor::receive_request_service_contexts(
  PI::ServerRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::receive_request_service_contexts");
  l.level_vlog(debug_level, "operation: %s", r->operation());

  try
  {
    idl_cr::CredentialData credential(
      get_credential<idl_cr::CredentialData>(r));
    handle_credential<idl_cr::CredentialData>(credential, r);
  }
  catch(const CORBA::NO_PERMISSION &e)
  {
    if (idl_ac::NoCredentialCode == e.minor())
    {
      legacy_idl_cr::CredentialData legacy_credential(
        get_credential<legacy_idl_cr::CredentialData>(r));
      handle_credential<legacy_idl_cr::CredentialData>(legacy_credential, r);
    }
    else
      throw;
  }
}

void ServerInterceptor::receive_request(PI::ServerRequestInfo_ptr) 
{ 
}

void ServerInterceptor::send_reply(PI::ServerRequestInfo_ptr) 
{ 
}

void ServerInterceptor::send_exception(PI::ServerRequestInfo_ptr) 
{ 
}

void ServerInterceptor::send_other(PI::ServerRequestInfo_ptr) 
{ 
}

char *ServerInterceptor::name() 
{ 
  return CORBA::string_dup("ServerInterceptor"); 
}

void ServerInterceptor::destroy() 
{ 
}

}
}
