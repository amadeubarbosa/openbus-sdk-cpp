// -*- coding: iso-8859-1-unix -*-
#include "openbus/detail/interceptors/server.hpp"
#include "openbus/Connection.hpp"
#include "openbus/openbus_context.hpp"
#include "openbus/detail/login_cache.hpp"
#include "openbus/detail/openssl/public_key.hpp"
#include "openbus/detail/any.hpp"
#include "openbus/log.hpp"

#include <boost/scoped_ptr.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
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
  const boost::shared_ptr<Connection> &conn,
  idl::core::OctetSeq &secret)
{
  idl::legacy::creden::CredentialReset rst;
  rst.session = session->id;
  rst.target = conn->_login()->id;
  std::memcpy(rst.challenge, secret.get_buffer(), idl::core::EncryptedBlockSize);
  CORBA::Any any;
  any <<= rst;
  return any;
}

CORBA::Any ServerInterceptor::attach_credential_rst(
  boost::shared_ptr<Session> session,
  const boost::shared_ptr<Connection> &conn,
  idl::core::OctetSeq &secret)
{
  idl::creden::CredentialReset rst;
  rst.session = session->id;
  rst.target = conn->_login()->id;
  rst.entity = conn->_login()->entity;
  std::memcpy(rst.challenge, secret.get_buffer(), idl::core::EncryptedBlockSize);
  CORBA::Any any;
  any <<= rst;
  return any;
}
  
template <typename C>
void ServerInterceptor::send_credential_reset(
  const boost::shared_ptr<Connection> &conn,
  boost::shared_ptr<Login> caller,
  PI::ServerRequestInfo_ptr r) 
{
  CORBA::Any any;
  idl::core::OctetSeq secret;
  {
    boost::lock_guard<boost::mutex> lock(_mutex);
    boost::shared_ptr<Session> session(
      boost::shared_ptr<Session>(
        new Session(caller->loginInfo->id.in())));
    _sessionLRUCache.insert(session->id, session);
    secret = caller->pubKey->encrypt(session->secret.data, 
                                     boost::uuids::uuid::static_size());
    if (boost::is_same<C, idl::creden::CredentialData>::value)
      any = attach_credential_rst(session, conn, secret);
    else
      any = attach_legacy_credential_rst(session, conn, secret);
  }
  CORBA::OctetSeq_var o(_orb_init->codec->encode_value(any));
  IOP::ServiceContext serviceContext;
  serviceContext.context_id =
    boost::is_same<C, idl::creden::CredentialData>::value ?
    idl::creden::CredentialContextId : idl::legacy::creden::CredentialContextId;
  CORBA::OctetSeq s(o->length(), o->length(), o->get_buffer(), 0);
  serviceContext.context_data = s;
  r->add_reply_service_context(serviceContext, true);
  throw CORBA::NO_PERMISSION(idl::access::InvalidCredentialCode, 
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
        boost::is_same<C, idl::creden::CredentialData>::value ?
        idl::creden::CredentialContextId : idl::legacy::creden::CredentialContextId));
    CORBA::Any_var any(
      _orb_init->codec->decode_value(
        sc->context_data,
        boost::is_same<C, idl::creden::CredentialData>::value ?
        idl::creden::_tc_CredentialData : idl::legacy::creden::_tc_CredentialData));
    credential = extract<C>(any);
  }
  catch (const CORBA::BAD_PARAM &) 
  {    
    throw CORBA::NO_PERMISSION(idl::access::NoCredentialCode, 
                               CORBA::COMPLETED_NO);
  }
  return credential;
}

void ServerInterceptor::save_dispatcher_connection(
  const boost::shared_ptr<Connection> &conn,
  PI::ServerRequestInfo_ptr r,
  OpenBusContext *ctx)
{
  {
  boost::lock_guard<boost::mutex> lg(_bus_ctx->_mutex);
  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  ctx->id2conn[uuid] = conn;
  CORBA::Any any;
  any <<= boost::uuids::to_string(uuid);
  r->set_slot(_orb_init->current_connection, any);
  }
  ctx->setCurrentConnection(conn);
}

boost::shared_ptr<Connection> ServerInterceptor::get_dispatcher_connection(
  OpenBusContext *ctx,
  const std::string &busid, 
  const std::string &login,
  PI::ServerRequestInfo_ptr r)
{
  boost::shared_ptr<Connection> conn;
  log_scope l(log().general_logger(), debug_level, 
              "ServerInterceptor::get_dispatcher");
  if (ctx->onCallDispatch())
  {
    try 
    {
      conn = ctx->onCallDispatch()(*ctx, busid, login, r->operation());
      if (conn && conn->busid() != busid)
      {
        throw CORBA::NO_PERMISSION(idl::access::UnknownBusCode, CORBA::COMPLETED_NO);
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
      throw CORBA::NO_PERMISSION(idl::access::UnknownBusCode, CORBA::COMPLETED_NO);
    }
  }
  if ( (!conn->login() || (conn->busid() != busid)) )
  {
    throw CORBA::NO_PERMISSION(idl::access::UnknownBusCode, CORBA::COMPLETED_NO);
  } 
  save_dispatcher_connection(conn, r, ctx);
  return conn;
}

bool ServerInterceptor::check_legacy_chain(
  idl::legacy::access::CallChain chain,
  boost::shared_ptr<Login> caller,
  const boost::shared_ptr<Connection> &conn,
  PI::ServerRequestInfo_ptr r)
{
  if (std::strcmp(chain.target, conn->_login()->entity)) 
  { 
    send_credential_reset<idl::legacy::creden::CredentialData>(conn, caller, r);
  }
  else if (std::strcmp(chain.caller.id, caller->loginInfo->id)) 
  {
    return false;
  }
  return true;
}

bool ServerInterceptor::check_chain(
  idl::access::CallChain chain,
  boost::shared_ptr<Login> caller,  
  const boost::shared_ptr<Connection> &conn)
{
  if (std::strcmp(chain.caller.id, caller->loginInfo->id)
      || std::strcmp(chain.bus.in(), conn->busid().c_str()))
  {
    return false;
  }
  return true;
}
    
template <typename C>
bool ServerInterceptor::validate_chain(
  C& cred,
  boost::shared_ptr<Login> caller,  
  const boost::shared_ptr<Connection> &conn,
  PI::ServerRequestInfo_ptr r)
{
  if (!cred.chain.encoded.length())
  {
    return false;
  }
  idl::core::HashValue hashChain;
  SHA256(cred.chain.encoded.get_buffer(),
         cred.chain.encoded.length(), hashChain);

  if (!conn->_buskey->verify(cred.chain.signature, 
                            idl::core::EncryptedBlockSize, hashChain, 
                            idl::core::HashValueSize))
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
        boost::is_same<C, idl::creden::CredentialData>::value ?
        idl::access::_tc_CallChain : idl::legacy::access::_tc_CallChain));

    if (boost::is_same<C, idl::creden::CredentialData>::value)
    {
      idl::access::CallChain chain(extract<idl::access::CallChain>(any));
      return check_chain(chain, caller, conn);
    }
    else
    {
      idl::legacy::access::CallChain chain(extract<idl::legacy::access::CallChain>(any));
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
  
  boost::shared_ptr<Connection> conn(
    get_dispatcher_connection(_bus_ctx, std::string(credential.bus),
                              std::string(credential.login), r));

  boost::shared_ptr<Login> caller;
  try 
  {
    l.vlog("Validando login: %s", credential.login.in());
    caller = conn->_loginCache->validateLogin(credential.login.in());
  }
  catch (const CORBA::NO_PERMISSION &e) 
  {
    if (idl::access::NoLoginCode == e.minor()) 
    {
      throw CORBA::NO_PERMISSION(idl::access::UnknownBusCode, CORBA::COMPLETED_NO);
    }
  } 
  catch (const CORBA::Exception &) 
  {
    throw CORBA::NO_PERMISSION(idl::access::UnverifiedLoginCode,
                               CORBA::COMPLETED_NO);
  }
  if (!caller) 
  {
    l.log("Lancando excecao InvalidLoginCode.");
    throw CORBA::NO_PERMISSION(idl::access::InvalidLoginCode, CORBA::COMPLETED_NO);
  }
  l.log("Login valido.");
  hash_value hash;   
  boost::shared_ptr<Session> session;
  std::string remote_id;
  {
    boost::lock_guard<boost::mutex> lock(_mutex);
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
                        idl::core::HashValueSize) 
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
    throw CORBA::NO_PERMISSION(idl::access::InvalidChainCode, CORBA::COMPLETED_NO);
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
    idl::creden::CredentialData credential(
      get_credential<idl::creden::CredentialData>(r));
    handle_credential<idl::creden::CredentialData>(credential, r);
  }
  catch(const CORBA::NO_PERMISSION &e)
  {
    if (idl::access::NoCredentialCode == e.minor())
    {
      idl::legacy::creden::CredentialData legacy_credential(
        get_credential<idl::legacy::creden::CredentialData>(r));
      handle_credential<idl::legacy::creden::CredentialData>(legacy_credential, r);
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
