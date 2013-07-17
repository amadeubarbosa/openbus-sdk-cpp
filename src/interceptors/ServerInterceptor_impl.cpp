// -*- coding: iso-8859-1-unix-*-
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/log.hpp"
#include "stubs/credential_v1_5.h"

#include <boost/scoped_ptr.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <cstring>
#include <iostream>
#include <string>

namespace openbus 
{
namespace interceptors 
{

const std::size_t LRUSize = 128;

Session::Session(const std::string &login) 
  : remoteId(login)
{
  tickets_init(&tickets);
  secret = boost::uuids::random_generator()();
  id = boost::uuids::hash_value(boost::uuids::random_generator()());
}

ServerInterceptor::ServerInterceptor(boost::shared_ptr<orb_info> p)
  : _orb_info(p), _sessionLRUCache(LRUSize)
{
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::ServerInterceptor");
}

void ServerInterceptor::sendCredentialReset(
  Connection &conn, boost::shared_ptr<Login> caller, PI::ServerRequestInfo &r) 
{
  idl_cr::CredentialReset credentialReset;
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  boost::shared_ptr<Session> session = boost::shared_ptr<Session>(
    new Session(std::string(caller->loginInfo->id)));
  _sessionLRUCache.insert(session->id, session);
  credentialReset.session = session->id;
  CORBA::OctetSeq secret = caller->pubKey->encrypt(
    session->secret.data, boost::uuids::uuid::static_size());
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif
  credentialReset.target = conn._login()->id;
  std::memcpy(credentialReset.challenge, secret.get_buffer(), 
              idl::EncryptedBlockSize);
  CORBA::Any any;
  any <<= credentialReset;
  CORBA::OctetSeq_var o = _orb_info->codec->encode_value(any);
  IOP::ServiceContext serviceContext;
  serviceContext.context_id = idl_cr::CredentialContextId;
  IOP::ServiceContext::_context_data_seq s(o->length(), o->length(),
                                           o->get_buffer(), 0);
  serviceContext.context_data = s;
  r.add_reply_service_context(serviceContext, true);
  throw CORBA::NO_PERMISSION(idl_ac::InvalidCredentialCode, 
                             CORBA::COMPLETED_NO);              
}

Connection &ServerInterceptor::getDispatcher(
  boost::shared_ptr<OpenBusContext> ctx, const std::string &busId, 
  const std::string &loginId, const std::string &operation)
{
  Connection *conn = 0;
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::getDispatcher");
  if (ctx->onCallDispatch())
  {
    try 
    {
      conn = ctx->onCallDispatch()(*ctx, busId, loginId, operation);
    }
    catch (...) 
    {
      l.level_log(warning_level, "Falha na execucao da callback CallDispatch.");
    }

    if (conn && ( (!conn->login() || (conn->busid() != busId)) ))
    {
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    } 
  }
  else
  {
    if (!(conn = ctx->getDefaultConnection())) 
    {
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    }
  }
  assert(conn);
  return *conn;
}

void ServerInterceptor::receive_request_service_contexts(
  PI::ServerRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::receive_request_service_contexts");
  l.level_vlog(debug_level, "operation: %s", r->operation());
  CORBA::Any_var any;
  bool hasContext = true;
  try 
  {
    IOP::ServiceContext_var sc = r->get_request_service_context(
      idl_cr::CredentialContextId);
    IOP::ServiceContext::_context_data_seq &cd = sc->context_data;
    CORBA::OctetSeq data(cd.length(), cd.length(), cd.get_buffer());
    any = _orb_info->codec->decode_value(data, idl_cr::_tc_CredentialData);
  }
  catch (const CORBA::BAD_PARAM &) 
  {
    hasContext = false;
  }
  idl_cr::CredentialData credential;
  if (hasContext && (any >>= credential)) 
  {
    Connection &conn = getDispatcher(_openbus_ctx, std::string(credential.bus), 
                                     std::string(credential.login), 
                                     r->operation());
    size_t const bufSize = sizeof(Connection *);
    unsigned char buf[bufSize];
    Connection *_c = &conn;
    std::memcpy(buf, &_c, bufSize);
    idl::OctetSeq_var connectionAddrOctetSeq =
      new idl::OctetSeq(bufSize, bufSize, buf);
    CORBA::Any connectionAddrAny;
    connectionAddrAny <<= *(connectionAddrOctetSeq);
    r->set_slot(_orb_info->slot.requester_conn, connectionAddrAny);
    r->set_slot(_orb_info->slot.receive_conn, connectionAddrAny);
    _openbus_ctx->setCurrentConnection(&conn);
    if (!conn._login())
    {
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    }
    boost::shared_ptr<Login> caller;
    if (strcmp(credential.bus, conn._busid.c_str())) 
    {
      l.log("Login diferente daquele que iniciou a sessão.");
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    }
    try 
    {
      l.vlog("Validando login: %s", credential.login.in());
      caller = conn._loginCache->validateLogin(std::string(credential.login));
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
    idl::HashValue hash;   
    boost::shared_ptr<Session> session;
    std::string remoteId;
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::unique_lock<boost::mutex> lock(_mutex);
#endif
    session = _sessionLRUCache.fetch(credential.session);
    if (session)
    {
      int bufSize = 22 + strlen(r->operation());
      boost::scoped_array<unsigned char> buf(new unsigned char[bufSize]());
      unsigned char *const pBuf = buf.get();
      pBuf[0] = idl::MajorVersion;
      pBuf[1] = idl::MinorVersion;
      std::memcpy(pBuf+2, session->secret.data, 
                  boost::uuids::uuid::static_size());
      std::memcpy(pBuf+18, &credential.ticket, 4);
      std::memcpy(pBuf+22, r->operation(), strlen(r->operation()));
      SHA256(pBuf, bufSize, hash);
      remoteId = session->remoteId;
    }
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.unlock();
#endif
    if (!(session
          && !std::memcmp(hash, credential.hash, idl::HashValueSize) 
          && !std::strcmp(remoteId.c_str(), credential.login) 
          && tickets_check(&session->tickets, credential.ticket))) 
    {
      l.level_vlog(debug_level, 
                   "credential not valid, try to reset credetial session");
      sendCredentialReset(conn, caller, *r);
    }
    l.level_vlog(debug_level, "credential is valid");
    if (!credential.chain.encoded.length())
    {
      throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
    }
    idl::HashValue hashChain;
    SHA256(credential.chain.encoded.get_buffer(),
           credential.chain.encoded.length(), hashChain);

    if (!conn._buskey->verify(credential.chain.signature, 
                              idl::EncryptedBlockSize, hashChain, 
                              idl::HashValueSize))
    {
      throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
    }
    else
    {
      CORBA::Any_var callChainAny =
        _orb_info->codec->decode_value(credential.chain.encoded,
                                idl_ac::_tc_CallChain);
      idl_ac::CallChain callChain;
      callChainAny >>= callChain;
      if (std::strcmp(callChain.target, conn._login()->entity)) 
      { 
        sendCredentialReset(conn, caller, *r);
      }
      else if (std::strcmp(callChain.caller.id, caller->loginInfo->id)) 
      {
        throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode,
                                   CORBA::COMPLETED_NO);
      }
      else 
      {
        CORBA::Any signedCallChainAny;
        signedCallChainAny <<= credential.chain;
        r->set_slot(_orb_info->slot.signed_call_chain, signedCallChainAny);
      }
    }
  } 
  else 
  {
    //[TODO] conn._legacyEnabled
    l.level_vlog(debug_level, "verificando se existe uma credencial legacy");
    hasContext = true;
    CORBA::Any_var lany;
    try 
    {
      IOP::ServiceContext_var sc = r->get_request_service_context(1234);
      IOP::ServiceContext::_context_data_seq &cd = sc->context_data;
      CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);
      lany = _orb_info->codec->decode_value(contextData, 
                                     openbus::legacy::v1_5::_tc_Credential);
    }
    catch (const CORBA::BAD_PARAM &) 
    {
      hasContext = false;
    }
    openbus::legacy::v1_5::Credential legacyCredential;
    if (hasContext && (lany >>= legacyCredential)) 
    {
      l.level_vlog(info_level, "extraindo credencial legacy");
      l.level_vlog(debug_level, "credential.identifier: %s",
                   legacyCredential.identifier.in());
      l.level_vlog(debug_level, "credential.owner: %s",
                   legacyCredential.owner.in());
      l.level_vlog(debug_level, "credential.delegate: %s",
                   legacyCredential.delegate.in());
      idl_ac::CallChain legacyChain;
      legacyChain.target = "";
      if (std::string(legacyCredential.delegate) == "")
      {
        legacyChain.originators.length(0);
        idl_ac::LoginInfo loginInfo;
        loginInfo.id = legacyCredential.identifier;
        loginInfo.entity = legacyCredential.owner;
        legacyChain.caller = loginInfo;            
      } 
      else 
      {
        legacyChain.originators.length(1);
        idl_ac::LoginInfo delegate;
        delegate.id = "<unknown>";
        delegate.entity = legacyCredential.delegate;
        legacyChain.originators[0] = delegate;
        idl_ac::LoginInfo login;
        login.id = legacyCredential.identifier;
        login.entity = legacyCredential.owner;
        legacyChain.caller = login;
      }
      CORBA::Any legacyChainAny;
      legacyChainAny <<= legacyChain;
      r->set_slot(_orb_info->slot.legacy_call_chain, legacyChainAny);
    }
    else
    {
      throw CORBA::NO_PERMISSION(idl_ac::NoCredentialCode, CORBA::COMPLETED_NO);
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
