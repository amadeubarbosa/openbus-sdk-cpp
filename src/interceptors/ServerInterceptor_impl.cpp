// -*- coding: iso-8859-1-unix -*-
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/LoginCache.hpp"
#include "openbus/crypto/PublicKey.hpp"
#include "openbus/log.hpp"
#include "openbus/any.hpp"

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

void ServerInterceptor::send_credential_reset(
  Connection &conn, boost::shared_ptr<Login> caller, PI::ServerRequestInfo &r) 
{
  idl_cr::CredentialReset credentialReset;
  idl::OctetSeq secret;
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);
#endif
    boost::shared_ptr<Session> session(
      boost::shared_ptr<Session>(
        new Session(caller->loginInfo->id.in())));
    _sessionLRUCache.insert(session->id, session);
    credentialReset.session = session->id;
    secret = caller->pubKey->encrypt(session->secret.data, 
                                     boost::uuids::uuid::static_size());
  }
  credentialReset.target = conn._login()->id;
  std::memcpy(credentialReset.challenge, secret.get_buffer(), 
              idl::EncryptedBlockSize);
  CORBA::Any any;
  any <<= credentialReset;
  CORBA::OctetSeq_var o(_orb_init->codec->encode_value(any));
  IOP::ServiceContext serviceContext;
  serviceContext.context_id = idl_cr::CredentialContextId;
  CORBA::OctetSeq s(o->length(), o->length(), o->get_buffer(), 0);
  serviceContext.context_data = s;
  r.add_reply_service_context(serviceContext, true);
  throw CORBA::NO_PERMISSION(idl_ac::InvalidCredentialCode, 
                             CORBA::COMPLETED_NO);              
}

credential ServerInterceptor::get_credential(PI::ServerRequestInfo &r) const
{
  credential credential_;
  try 
  {
    IOP::ServiceContext_var sc(
      r.get_request_service_context(idl_cr::CredentialContextId));
    CORBA::Any_var any(
      _orb_init->codec->decode_value(sc->context_data,
                           idl_cr::_tc_CredentialData));

    idl_cr::CredentialData credential_data(
      extract<idl_cr::CredentialData>(any));
    credential_.data = credential_data;
  }
  catch (const CORBA::BAD_PARAM &) 
  {    
    throw CORBA::NO_PERMISSION(idl_ac::NoCredentialCode, 
                               CORBA::COMPLETED_NO);
  }
  return credential_;
}

void ServerInterceptor::save_dispatcher_connection(
  Connection &conn,
  PI::ServerRequestInfo &r,
  OpenBusContext *ctx)
{
  size_t const bufSize(sizeof(Connection *));
  unsigned char buf[bufSize];
  Connection *_c(&conn);
  std::memcpy(buf, &_c, bufSize);
  idl::OctetSeq connectionAddrOctetSeq(bufSize, bufSize, buf);
  CORBA::Any connectionAddrAny;
  connectionAddrAny <<= connectionAddrOctetSeq;
  r.set_slot(_orb_init->current_connection, connectionAddrAny);
  ctx->setCurrentConnection(&conn);
}

Connection &ServerInterceptor::get_dispatcher_connection(
  OpenBusContext *ctx,
  const std::string &busid, 
  const std::string &login,
  PI::ServerRequestInfo &r)
{
  Connection *conn(0);
  log_scope l(log().general_logger(), debug_level, 
              "ServerInterceptor::get_dispatcher");
  if (ctx->onCallDispatch())
  {
    try 
    {
      conn = ctx->onCallDispatch()(*ctx, busid, login, r.operation());
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
    
void ServerInterceptor::receive_request_service_contexts(
  PI::ServerRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::receive_request_service_contexts");
  l.level_vlog(debug_level, "operation: %s", r->operation());
  
  credential credential_(get_credential(*r));
  
  Connection &conn(
    get_dispatcher_connection(_bus_ctx, std::string(credential_.data.bus),
                              std::string(credential_.data.login), *r));

  boost::shared_ptr<Login> caller;
  try 
  {
    l.vlog("Validando login: %s", credential_.data.login.in());
    caller = conn._loginCache->validateLogin(credential_.data.login.in());
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
    session = _sessionLRUCache.fetch(credential_.data.session);
    if (session)
    {
      boost::array<unsigned char, secret_size> secret;
      std::copy(session->secret.begin(), session->secret.end(), 
                secret.c_array());
      hash = ::openbus::hash(r->operation(), 
                             credential_.data.ticket, secret);
      remote_id = session->remote_id;
    }
  }
  if (!(session
        && !std::memcmp(hash.data(), credential_.data.hash, 
                        idl::HashValueSize) 
        && !std::strcmp(remote_id.c_str(), credential_.data.login) 
        && tickets_check(&session->tickets, 
                         credential_.data.ticket))) 
  {
    l.level_vlog(debug_level, 
                 "credential not valid, try to reset credetial session");
    send_credential_reset(conn, caller, *r);
  }
  l.level_vlog(debug_level, "credential is valid");
  if (!credential_.data.chain.encoded.length())
  {
    throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
  }
  idl::HashValue hashChain;
  SHA256(credential_.data.chain.encoded.get_buffer(),
         credential_.data.chain.encoded.length(), hashChain);

  if (!conn._buskey->verify(credential_.data.chain.signature, 
                            idl::EncryptedBlockSize, hashChain, 
                            idl::HashValueSize))
  {
    throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
  }
  else
  {
    CORBA::OctetSeq o(credential_.data.chain.encoded.maximum(),
                      credential_.data.chain.encoded.length(),
                      credential_.data.chain.encoded.get_buffer());
    CORBA::Any_var any(
      _orb_init->codec->decode_value(o, idl_ac::_tc_CallChain));

    idl_ac::CallChain chain(extract<idl_ac::CallChain>(any));
    if (std::strcmp(chain.target, conn._login()->entity)) 
    { 
      send_credential_reset(conn, caller, *r);
    }
    else if (std::strcmp(chain.caller.id, caller->loginInfo->id)) 
    {
      throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode,
                                 CORBA::COMPLETED_NO);
    }
    else 
    {
      CORBA::Any any;
      any <<= credential_.data.chain;
      r->set_slot(_orb_init->signed_call_chain, any);
    }
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
