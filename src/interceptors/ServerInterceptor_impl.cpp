// -*- coding: iso-8859-1-unix -*-
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/LoginCache.hpp"
#include "openbus/crypto/PublicKey.hpp"
#include "openbus/log.hpp"
#include "openbus/any.hpp"
#include "stubs/credential_v1_5.h"

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
  id = boost::uuids::hash_value(boost::uuids::random_generator()());
}

ServerInterceptor::ServerInterceptor(boost::shared_ptr<orb_info> p)
  : _orb_info(p), _codec(_orb_info->codec), _sessionLRUCache(LRUSize)
{
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::ServerInterceptor");
}

void ServerInterceptor::send_credential_reset(
  Connection &conn, boost::shared_ptr<Login> caller, PI::ServerRequestInfo &r) 
{
  idl_cr::CredentialReset credentialReset;
  CORBA::OctetSeq secret;
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
  CORBA::OctetSeq_var o(_codec->encode_value(any));
  IOP::ServiceContext serviceContext;
  serviceContext.context_id = idl_cr::CredentialContextId;
  IOP::ServiceContext::_context_data_seq s(o->length(), o->length(),
                                           o->get_buffer(), 0);
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
      _codec->decode_value(sc->context_data,
                           idl_cr::_tc_CredentialData));

    idl_cr::CredentialData credential_data(
      extract<idl_cr::CredentialData>(any));
    credential_.data = credential_data;
  }
  catch (const CORBA::BAD_PARAM &) 
  {    
    try 
    {
      IOP::ServiceContext_var sc(r.get_request_service_context(1234));
      CORBA::Any_var any(
        _codec->decode_value(sc->context_data,
                             openbus::legacy::v1_5::_tc_Credential));

      openbus::legacy::v1_5::Credential legacy_credential(
        extract<openbus::legacy::v1_5::Credential>(any));
      credential_.legacy.identifier = legacy_credential.identifier;
      credential_.legacy.owner = legacy_credential.owner;
      credential_.legacy.delegate = legacy_credential.delegate;
      credential_.data.login = credential_.legacy.identifier;
    }
    catch (const CORBA::BAD_PARAM &) 
    {
      throw CORBA::NO_PERMISSION(idl_ac::NoCredentialCode, 
                                 CORBA::COMPLETED_NO);
    }
  }
  return credential_;
}

void ServerInterceptor::build_legacy_chain(
  PI::ServerRequestInfo &r,
  const std::string &target,
  const openbus::legacy::v1_5::Credential &credential) const
{
  idl_ac::CallChain legacyChain;
  legacyChain.target = target.c_str();
  if (std::string(credential.delegate.in()).empty())
  {
    legacyChain.originators.length(0);
    idl_ac::LoginInfo login_info;
    login_info.id = credential.identifier;
    login_info.entity = credential.owner;
    legacyChain.caller = login_info;            
  } 
  else 
  {
    legacyChain.originators.length(1);
    idl_ac::LoginInfo delegate;
    delegate.id = "<unknown>";
    delegate.entity = credential.delegate;
    legacyChain.originators[0u] = delegate;
    idl_ac::LoginInfo login;
    login.id = credential.identifier;
    login.entity = credential.owner;
    legacyChain.caller = login;
  }
  CORBA::Any legacy_chain_any;
  legacy_chain_any <<= legacyChain;
  CORBA::OctetSeq_var legacy_chain_cdr(_codec->encode_value(legacy_chain_any));
  idl_cr::SignedCallChain signed_legacy_chain;
  std::memset(signed_legacy_chain.signature, '\0', idl::EncryptedBlockSize);
  signed_legacy_chain.encoded = legacy_chain_cdr;
  CORBA::Any signed_legacy_chain_any;
  signed_legacy_chain_any <<= signed_legacy_chain;
  r.set_slot(_orb_info->slot.signed_call_chain, signed_legacy_chain_any);
}

void ServerInterceptor::save_dispatcher_connection(
  Connection &conn,
  PI::ServerRequestInfo &r,
  boost::shared_ptr<OpenBusContext> ctx)
{
  size_t const bufSize(sizeof(Connection *));
  unsigned char buf[bufSize];
  Connection *_c(&conn);
  std::memcpy(buf, &_c, bufSize);
  idl::OctetSeq connectionAddrOctetSeq(bufSize, bufSize, buf);
  CORBA::Any connectionAddrAny;
  connectionAddrAny <<= connectionAddrOctetSeq;
  r.set_slot(_orb_info->slot.current_connection, connectionAddrAny);
  ctx->setCurrentConnection(&conn);
}

Connection &ServerInterceptor::get_dispatcher_connection(
  boost::shared_ptr<OpenBusContext> ctx,
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
    get_dispatcher_connection(_openbus_ctx, std::string(credential_.data.bus),
                              std::string(credential_.data.login), *r));

  if (credential_.is_legacy()) 
  {
    build_legacy_chain(*r, conn.login()->entity.in(), credential_.legacy);
    return;
  }

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
    CORBA::Any_var any(
      _codec->decode_value(
        credential_.data.chain.encoded, idl_ac::_tc_CallChain));

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
      r->set_slot(_orb_info->slot.signed_call_chain, any);
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
