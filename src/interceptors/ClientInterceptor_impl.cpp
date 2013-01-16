// -*- coding: iso-8859-1 -*-
#include "openbus/interceptors/ClientInterceptor_impl.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "stubs/core.h"
#include "stubs/credential_v1_5.h"
#include "openbus/log.hpp"

#include <openssl/sha.h>

#include <cstddef>
#include <cstring>
#include <sstream>

const std::size_t LRUSize = 128;

namespace openbus 
{
namespace idl = tecgraf::openbus::core::v2_0;

namespace interceptors 
{ 

PortableInterceptor::SlotId ClientInterceptor::_slotId_ignoreInterceptor;

std::string getSessionKey(PortableInterceptor::ClientRequestInfo &r) 
{
  idl::HashValue profileDataHash;
  ::IOP::TaggedProfile::_profile_data_seq profile = 
    r.effective_profile()->profile_data;
  SHA256(profile.get_buffer(), profile.length(), profileDataHash);
  return std::string((const char*) profileDataHash, idl::HashValueSize);
}

Connection &ClientInterceptor::getCurrentConnection(
  PortableInterceptor::ClientRequestInfo &r)
{
  log_scope l(log().general_logger(),info_level,
              "ClientInterceptor::getCurrentConnection");
  Connection *conn = 0;
  CORBA::Any_var connectionAddrAny;
  connectionAddrAny = r.get_slot(_slotId_requesterConnection);
  idl::OctetSeq connectionAddrOctetSeq;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) 
  {
    assert(connectionAddrOctetSeq.length() == sizeof(conn));
    std::memcpy(&conn, connectionAddrOctetSeq.get_buffer(), sizeof(conn));
  }
  assert(_openbusContext);
  if(!conn) 
  {
    if (!(conn = _openbusContext->getDefaultConnection())) 
    {
      l.log("throw NoLoginCode");
      throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
    }
  }
  assert(conn);
  l.vlog("connection:%p", conn);
  return *conn;
}

CallerChain ClientInterceptor::getJoinedChain(Connection &c, 
  PortableInterceptor::ClientRequestInfo &r)
{
  CORBA::Any_var signedCallChainAny= r.get_slot(_slotId_joinedCallChain);
  idl_cr::SignedCallChain signedCallChain;
  if (*signedCallChainAny >>= signedCallChain) 
  {
    CORBA::Any_var callChainAny =
      _cdrCodec->decode_value(signedCallChain.encoded, idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain)
    {
      return CallerChain(c.busid(), *c.login(), callChain.originators, 
                         callChain.caller, signedCallChain);
    }
  }
  return CallerChain();
}

ClientInterceptor::ClientInterceptor(
  PortableInterceptor::SlotId slotId_requesterConnection,
  PortableInterceptor::SlotId slotId_joinedCallChain,
  PortableInterceptor::SlotId slotId_ignoreInterceptor,
  IOP::Codec *cdr_codec)
  : _cdrCodec(cdr_codec), 
    _slotId_requesterConnection(slotId_requesterConnection), 
    _slotId_joinedCallChain(slotId_joinedCallChain), 
    _sessionLRUCache(SessionLRUCache(LRUSize)),
    _callChainLRUCache(CallChainLRUCache(LRUSize)), 
    _openbusContext(0)
{ 
  log_scope l(log().general_logger(), info_level, 
              "ClientInterceptor::ClientInterceptor");
  _slotId_ignoreInterceptor = slotId_ignoreInterceptor;
}

void ClientInterceptor::send_request(PortableInterceptor::ClientRequestInfo *r)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::send_request");
  l.level_vlog(debug_level, "operation: %s", r->operation());

  if (!IgnoreInterceptor::status(*r)) 
  {
    Connection &conn = getCurrentConnection(*r);
    if (conn._login()) 
    {
      l.vlog("login: %s", conn._login()->id.in());
      CallerChain callerChain;
      IOP::ServiceContext serviceContext;
      serviceContext.context_id = idl_cr::CredentialContextId;

      idl_cr::CredentialData credential;
      credential.bus = CORBA::string_dup(conn._busid.c_str());
      credential.login = CORBA::string_dup(conn._login()->id);      
      std::string sessionKey = getSessionKey(*r);    
      SecretSession session;
#ifdef OPENBUS_SDK_MULTITHREAD
      boost::unique_lock<boost::mutex> lock(_mutex);
#endif
      bool b = _sessionLRUCache.exists(sessionKey);
      if (b)
      {
        session = _sessionLRUCache.fetch(sessionKey);
      }
#ifdef OPENBUS_SDK_MULTITHREAD
      lock.unlock();
#endif
      if (b) 
      {
        credential.session = session.id;
        credential.ticket = ++session.ticket;
        int bufSize = 22 + strlen(r->operation());
        std::auto_ptr<unsigned char> buf (new unsigned char[bufSize]());
        buf.get()[0] = idl::MajorVersion;
        buf.get()[1] = idl::MinorVersion;
        std::memcpy(buf.get()+2, session.secret->get_buffer(), 16);
        std::memcpy(buf.get()+18, &credential.ticket, 4);
        std::memcpy(buf.get()+22, r->operation(), strlen(r->operation()));
        SHA256(buf.get(), bufSize, credential.hash);
        
        callerChain = getJoinedChain(conn, *r);
        if (std::strcmp(idl::BusLogin, session.remoteId.in())) 
        {
          idl::HashValue hash;
          CORBA::String_var connId = CORBA::string_dup(conn._login()->id);
          size_t idSize = strlen(connId);
          size_t remoteIdSize = strlen(session.remoteId.in());
          bufSize = idSize + remoteIdSize + idl::EncryptedBlockSize;
          buf.reset(new unsigned char[bufSize]());
          std::memcpy(buf.get(), connId, idSize);
          std::memcpy(buf.get() + idSize, session.remoteId.in(), remoteIdSize);
          if (callerChain != CallerChain())
          {
            std::memcpy(buf.get()+idSize+remoteIdSize, 
                   callerChain.signedCallChain()->signature, 
                   idl::EncryptedBlockSize);
          } 
          else
          {
            std::memset(buf.get() + idSize + remoteIdSize, '\0', 
                   idl::EncryptedBlockSize);
          }
          SHA256(buf.get(), bufSize, hash);
          std::string shash((const char*) hash, idl::HashValueSize);
          idl_cr::SignedCallChain signedCallChain;
#ifdef OPENBUS_SDK_MULTITHREAD
          lock.lock();
#endif
          bool b2 = _callChainLRUCache.exists(shash);
          if (b2)
          {
            signedCallChain = _callChainLRUCache.fetch(shash);
          }
#ifdef OPENBUS_SDK_MULTITHREAD
          lock.unlock();
#endif
          if (b2) 
          {
            l.level_vlog(debug_level, 
                         "Recuperando signedCallChain. remoteid: %s", 
                         session.remoteId.in());
            credential.chain = signedCallChain;
          } 
          else
          {
            credential.chain = 
              *conn.access_control()->signChainFor(session.remoteId.in());
#ifdef OPENBUS_SDK_MULTITHREAD
            lock.lock();
#endif
            _callChainLRUCache.insert(shash, credential.chain);
#ifdef OPENBUS_SDK_MULTITHREAD
            lock.unlock();
#endif
          }
        } 
        else
        {
          if (callerChain != CallerChain())
          {
            credential.chain = *(callerChain.signedCallChain());
          }
          else
          {
            std::memset(credential.chain.signature, '\0', 
                        idl::EncryptedBlockSize);
          }
        }
      } 
      else
      {
        credential.ticket = 0;
        credential.session = 0;
        std::memset(credential.hash, '\0', idl::HashValueSize);
        std::memset(credential.chain.signature, '\0', idl::EncryptedBlockSize);
      }
      
      CORBA::Any any;
      any <<= credential;
      CORBA::OctetSeq_var o = _cdrCodec->encode_value(any);
      IOP::ServiceContext::_context_data_seq s(o->length(), o->length(), 
                                               o->get_buffer());
      serviceContext.context_data = s;
      r->add_request_service_context(serviceContext, true);
      
      if (conn._legacyEnabled)
      {
        IOP::ServiceContext legacyContext;
        legacyContext.context_id = 1234;
        legacy::v1_5::Credential legacyCredential;
        legacyCredential.identifier = CORBA::string_dup(conn._login()->id);
        legacyCredential.owner = CORBA::string_dup(conn._login()->entity);
        if (callerChain != CallerChain())
        {
          if (conn._legacyDelegate == Connection::ORIGINATOR 
              && callerChain._originators.length())
          {
            legacyCredential.delegate = 
              CORBA::string_dup(callerChain._originators[0].entity);
          }
          else
          {
            legacyCredential.delegate = 
              CORBA::string_dup(callerChain._caller.entity); 
          }
        }
        else
        {
          legacyCredential.delegate = "";
        }
        CORBA::Any lany;
        lany <<= legacyCredential;
        o = _cdrCodec->encode_value(lany);
        IOP::ServiceContext::_context_data_seq ls(o->length(), o->length(), 
                                                  o->get_buffer());
        legacyContext.context_data = ls;
        r->add_request_service_context(legacyContext, true);
      }
    } 
    else 
    {
      l.log("throw NoLoginCode");
      throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, 
                                 CORBA::COMPLETED_NO);          
    }
  }
}

void ClientInterceptor::receive_exception(
  PortableInterceptor::ClientRequestInfo *r)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::receive_exception");
  l.level_vlog(debug_level, "operation: %s", r->operation()); 
  l.level_vlog(debug_level, "exception: %s", r->received_exception_id()); 

  if (!std::strcmp(r->received_exception_id(), 
              "IDL:omg.org/CORBA/NO_PERMISSION:1.0")) 
  {
    CORBA::SystemException &ex = 
      *CORBA::SystemException::_decode(*r->received_exception());
    if (ex.completed() == CORBA::COMPLETED_NO) 
    {
      l.level_vlog(debug_level, "minor: %d", ex.minor());
      Connection &conn = getCurrentConnection(*r);
      if (ex.minor() == idl_ac::InvalidCredentialCode) 
      {
        l.level_vlog(debug_level, "creating credential session");
        IOP::ServiceContext_var sctx;
        if ((sctx = r->get_request_service_context(
               idl_cr::CredentialContextId))) 
        {
          CORBA::ULong len = sctx->context_data.length();
          CORBA::OctetSeq o(len, len, sctx->context_data.get_buffer());
          CORBA::Any_var any;
          try 
          {
            any = _cdrCodec->decode_value(o, idl_cr::_tc_CredentialReset);
          }
          catch (const CORBA::Exception &) 
          {
            throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, 
                                       CORBA::COMPLETED_NO);
          }
          idl_cr::CredentialReset credentialReset;
          if (!(any >>= credentialReset)) 
          {
            throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode,
                                       CORBA::COMPLETED_NO);
          }
        
          CORBA::OctetSeq_var secret 
            (new CORBA::OctetSeq (conn._key.decrypt(credentialReset.challenge,
                                                    idl::EncryptedBlockSize)));

          std::string sessionKey = getSessionKey(*r);
          
          SecretSession session;
          session.id = credentialReset.session;
          session.remoteId = CORBA::string_dup(credentialReset.login);
          session.secret = secret;
          session.ticket = 0;
          {
#ifdef OPENBUS_SDK_MULTITHREAD
            boost::lock_guard<boost::mutex> lock(_mutex);
#endif
            _sessionLRUCache.insert(sessionKey, session);
          }
          l.log("Retransmissao da requisicao...");
					CORBA::Object *_o = new CORBA::Object(*r->target());
          throw PortableInterceptor::ForwardRequest(_o, false);
        } 
        throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, 
                                   CORBA::COMPLETED_NO);
      } 
      else if (ex.minor() == idl_ac::NoCredentialCode) 
      {
        throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode,
                                   CORBA::COMPLETED_NO);
      }
      else if (ex.minor() == idl_ac::InvalidLoginCode) 
      {
#ifdef OPENBUS_SDK_MULTITHREAD
        boost::unique_lock<boost::mutex> conn_lock(conn._mutex);
#endif
        idl_ac::LoginInfo oldLogin = *conn._loginInfo;
        conn._state = Connection::INVALID;
#ifdef OPENBUS_SDK_MULTITHREAD
        conn_lock.unlock();
#endif
        try 
        {
          if (conn.onInvalidLogin()) 
          {
            conn.onInvalidLogin()(conn, oldLogin);
          }
        } 
        catch (...) 
        {
          l.level_log(warning_level, 
                      "Falha na execucao da callback OnInvalidLogin.");
        }

        if (conn._state == Connection::LOGGED)
        {
          throw PortableInterceptor::ForwardRequest(r->target(), false); 
        }
        else if (conn._state == Connection::UNLOGGED) 
        {
          l.log("Connection::UNLOGGED: throw NoLoginCode");
          throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode,
                                     CORBA::COMPLETED_NO);          
        } 
        else if (conn._state == Connection::INVALID) 
        {
          if (!std::strcmp(conn._login()->id.in(), oldLogin.id.in())) 
          {
            conn._logout();
            l.log("Connection::INVALID: throw NoLoginCode");
            throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode,
                                       CORBA::COMPLETED_NO);
          }
          else
          {
            throw PortableInterceptor::ForwardRequest(r->target(), false);
          }
        }
      }
    }
  }
}

}
}
