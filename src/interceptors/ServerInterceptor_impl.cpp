// -*- coding: iso-8859-1 -*-
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/log.hpp"
#include "stubs/credential_v1_5.h"

#include <cstring>
#include <iostream>
#include <string>

namespace openbus 
{
namespace interceptors 
{

const std::size_t LRUSize = 128;

Session::Session(std::size_t i, const std::string &login) 
  : id(i), remoteId(login)
{
  tickets_init(&tickets);
  for (std::size_t i = 0; i != secretSize; ++i) 
  {
    secret[i] = rand() % 255;
  }
}

ServerInterceptor::ServerInterceptor(
  PortableInterceptor::Current *piCurrent, 
  PortableInterceptor::SlotId slotId_requesterConnection,
  PortableInterceptor::SlotId slotId_receiveConnection,
  PortableInterceptor::SlotId slotId_joinedCallChain,
  PortableInterceptor::SlotId slotId_signedCallChain, 
  PortableInterceptor::SlotId slotId_legacyCallChain,
  IOP::Codec *cdr_codec) 
  : _piCurrent(piCurrent), 
    _slotId_requesterConnection(slotId_requesterConnection),
    _slotId_receiveConnection(slotId_receiveConnection), 
    _slotId_joinedCallChain(slotId_joinedCallChain),
    _slotId_signedCallChain(slotId_signedCallChain),
    _slotId_legacyCallChain(slotId_legacyCallChain),
    _cdrCodec(cdr_codec), _openbusContext(0),
    _sessionLRUCache(SessionLRUCache(LRUSize))
{
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::ServerInterceptor");
}

void ServerInterceptor::sendCredentialReset(
  Connection &conn, Login &caller, PortableInterceptor::ServerRequestInfo &r) 
{
  idl_cr::CredentialReset credentialReset;

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  Session session(_sessionLRUCache.size() + 1,
                  std::string(caller.loginInfo->id));
  _sessionLRUCache.insert(session.id, session);
  credentialReset.session = session.id;

  CORBA::OctetSeq encrypted =
    caller.pubKey->encrypt(session.secret, secretSize); 
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif

  credentialReset.login = conn._login()->id;
  std::memcpy(credentialReset.challenge, encrypted.get_buffer(),
         idl::EncryptedBlockSize);

  CORBA::Any any;
  any <<= credentialReset;
  CORBA::OctetSeq_var o = _cdrCodec->encode_value(any);

  IOP::ServiceContext serviceContext;
  serviceContext.context_id = idl_cr::CredentialContextId;
  IOP::ServiceContext::_context_data_seq s(o->length(), o->length(),
                                           o->get_buffer(), 0);
  serviceContext.context_data = s;
  r.add_reply_service_context(serviceContext, true);          

  throw CORBA::NO_PERMISSION(idl_ac::InvalidCredentialCode, 
                             CORBA::COMPLETED_NO);              
}

Connection &ServerInterceptor::getDispatcher(OpenBusContext &context,
                                             const std::string &busId, 
                                             const std::string &loginId,
                                             const std::string &operation)
{
  Connection *conn = 0;
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::getDispatcher");
  if (context.onCallDispatch())
  {
    try 
    {
      conn = context.onCallDispatch()(context, busId, loginId, operation);
    }
    catch (...) 
    {
      l.level_log(warning_level, "Falha na execucao da callback CallDispatch.");
    }

    if (conn)
    {
      if (!conn->login() || (conn->busid() != busId)) 
      {
        throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, 
                                   CORBA::COMPLETED_NO);
      } 
    }
  }
  else
  {
    if (!(conn = context.getDefaultConnection())) 
    {
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    }
  }
  assert(conn);
  return *conn;
}

void ServerInterceptor::receive_request_service_contexts(
  PortableInterceptor::ServerRequestInfo *r)
{
  log_scope l(log().general_logger(), debug_level,
              "ServerInterceptor::receive_request_service_contexts");
  l.level_vlog(debug_level, "operation: %s", r->operation());
  
  CORBA::Any_var any;
  bool hasContext = true;
  try 
  {
    IOP::ServiceContext_var sc =
      r->get_request_service_context(idl_cr::CredentialContextId);
    IOP::ServiceContext::_context_data_seq &cd = sc->context_data;
    CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer());
    any = _cdrCodec->decode_value(contextData, idl_cr::_tc_CredentialData);
  }
  catch (const CORBA::BAD_PARAM &) 
  {
    hasContext = false;
  }
  idl_cr::CredentialData credential;
  if (hasContext && (any >>= credential)) 
  {
    Connection &conn = getDispatcher(
      *_openbusContext, std::string(credential.bus), 
      std::string(credential.login), r->operation());

    size_t const bufSize = sizeof(Connection*);
    unsigned char buf[bufSize];
    Connection *_c = &conn;
    std::memcpy(buf, &_c, bufSize);
    idl::OctetSeq_var connectionAddrOctetSeq =
      new idl::OctetSeq(bufSize, bufSize, buf);
    CORBA::Any connectionAddrAny;
    connectionAddrAny <<= *(connectionAddrOctetSeq);
    r->set_slot(_slotId_requesterConnection, connectionAddrAny);

    r->set_slot(_slotId_receiveConnection, connectionAddrAny);

    _openbusContext->setCurrentConnection(&conn);

    if (!conn._login())
    {
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    }

    Login *caller;
    if (strcmp(credential.bus.in(), conn._busid.c_str())) 
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
      if (e.minor() == idl_ac::NoLoginCode) 
      {
        throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
      }
    } 
    catch (const CORBA::Exception &) 
    {
      /* não há uma entrada válida no cache e o cache nõo consegui
      ** validar o login com o barramento. */
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
    Session *session = 0;

#ifdef OPENBUS_SDK_MULTITHREAD
    boost::unique_lock<boost::mutex> lock(_mutex);
#endif
    bool hasSession = _sessionLRUCache.exists(credential.session);
    if (hasSession)
    {
      session = _sessionLRUCache.fetch_ptr(credential.session);
    }
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.unlock();
#endif

    tickets_History *t = 0;
    std::string remoteId;
    if (hasSession) 
    {
      size_t operationSize = strlen(r->operation());
      int bufSize = 22 + operationSize;
      std::auto_ptr<unsigned char> buf(new unsigned char[bufSize]());
      unsigned char *pBuf = buf.get();
      pBuf[0] = idl::MajorVersion;
      pBuf[1] = idl::MinorVersion;
#ifdef OPENBUS_SDK_MULTITHREAD
      lock.lock();
#endif
      std::memcpy(pBuf+2, session->secret, secretSize);
#ifdef OPENBUS_SDK_MULTITHREAD
      lock.unlock();
#endif
      std::memcpy(pBuf+18, &credential.ticket, 4);
      std::memcpy(pBuf+22, r->operation(), operationSize);
      SHA256(pBuf, bufSize, hash);
#ifdef OPENBUS_SDK_MULTITHREAD
      lock.lock();
#endif
      t = &session->tickets;
      remoteId = session->remoteId;
#ifdef OPENBUS_SDK_MULTITHREAD
      lock.unlock();
#endif
    }

    if (!(hasSession 
          && !std::memcmp(hash, credential.hash, idl::HashValueSize) 
          && !std::strcmp(remoteId.c_str(), credential.login.in()) 
          && tickets_check(t, credential.ticket))) 
    {
      l.level_vlog(debug_level, 
                   "credential not valid, try to reset credetial session");
      sendCredentialReset(conn, *caller, *r);
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
        _cdrCodec->decode_value(credential.chain.encoded,
                                idl_ac::_tc_CallChain);
      idl_ac::CallChain callChain;
      callChainAny >>= callChain;
      if (std::strcmp(callChain.target, conn._login()->id)) 
      { 
        sendCredentialReset(conn, *caller, *r);
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
        r->set_slot(_slotId_signedCallChain, signedCallChainAny);
      }
    }
  } 
  else 
  {
    l.level_vlog(debug_level, "verificando se existe uma credencial legacy");
    hasContext = true;
    CORBA::Any_var lany;
    try 
    {
      IOP::ServiceContext_var sc = r->get_request_service_context(1234);
      IOP::ServiceContext::_context_data_seq &cd = sc->context_data;
      CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);
      lany = _cdrCodec->decode_value(contextData, 
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
      if (std::strcmp(legacyCredential.delegate, "")) 
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
      else 
      {
        legacyChain.originators.length(0);
        idl_ac::LoginInfo loginInfo;
        loginInfo.id = legacyCredential.identifier;
        loginInfo.entity = legacyCredential.owner;
        legacyChain.caller = loginInfo;            
      }
      CORBA::Any legacyChainAny;
      legacyChainAny <<= legacyChain;
      r->set_slot(_slotId_legacyCallChain, legacyChainAny);
    } 
    else
    {
      throw CORBA::NO_PERMISSION(idl_ac::NoCredentialCode, CORBA::COMPLETED_NO);
    }
  }
}

}
}
