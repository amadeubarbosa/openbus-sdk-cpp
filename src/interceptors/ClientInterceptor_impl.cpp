// -*- coding: iso-8859-1-unix-*-
#include "openbus/interceptors/ClientInterceptor_impl.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "stubs/core.h"
#include "stubs/credential_v1_5.h"
#include "openbus/log.hpp"

#include <boost/scoped_ptr.hpp>
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

std::string getSessionKey(PI::ClientRequestInfo &r) 
{
  idl::HashValue profileDataHash;
  ::IOP::TaggedProfile::_profile_data_seq profile = 
    r.effective_profile()->profile_data;
  SHA256(profile.get_buffer(), profile.length(), profileDataHash);
  return std::string((const char*) profileDataHash, idl::HashValueSize);
}

Connection &ClientInterceptor::getCurrentConnection(PI::ClientRequestInfo &r)
{
  log_scope l(log().general_logger(),info_level,
              "ClientInterceptor::getCurrentConnection");
  Connection *conn = 0;
  CORBA::Any_var connectionAddrAny;
  connectionAddrAny = r.get_slot(_orb_info->slot.requester_conn);
  idl::OctetSeq connectionAddrOctetSeq;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) 
  {
    assert(connectionAddrOctetSeq.length() == sizeof(conn));
    std::memcpy(&conn, connectionAddrOctetSeq.get_buffer(), sizeof(conn));
  }
  assert(_openbus_ctx);
  if(!conn) 
  {
    if (!(conn = _openbus_ctx->getDefaultConnection())) 
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
                                              PI::ClientRequestInfo &r)
{
  CORBA::Any_var signedCallChainAny= r.get_slot(_orb_info->slot.joined_call_chain);
  idl_cr::SignedCallChain signedCallChain;
  if (*signedCallChainAny >>= signedCallChain) 
  {
    CORBA::Any_var callChainAny =
      _orb_info->codec->decode_value(signedCallChain.encoded, idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain)
    {
      return CallerChain(
        c.busid(), c.login()->entity.in(), callChain.originators, 
        callChain.caller, signedCallChain);
    }
  }
  return CallerChain();
}

ClientInterceptor::ClientInterceptor(boost::shared_ptr<orb_info> p)
  : _orb_info(p), _callChainLRUCache(LRUSize)
{ 
  log_scope l(log().general_logger(), info_level, 
              "ClientInterceptor::ClientInterceptor");
}

void ClientInterceptor::send_request(PI::ClientRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::send_request");
  l.level_vlog(debug_level, "operation: %s", r->operation());

  CORBA::Any_var any = r->get_slot(_orb_info->slot.ignore_interceptor);
  CORBA::Boolean ignore = false;
  *any >>= CORBA::Any::to_boolean(ignore);
  if (!ignore) 
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
      Connection::SecretSession session;
#ifdef OPENBUS_SDK_MULTITHREAD
      boost::unique_lock<boost::mutex> lock(_mutex);
#endif
      bool b = conn._profile2session.exists(sessionKey);
      if (b)
      {
        session = conn._profile2session.fetch(sessionKey);
      }
#ifdef OPENBUS_SDK_MULTITHREAD
      lock.unlock();
#endif
      if (b) 
      {
        credential.session = session.id;
        credential.ticket = ++session.ticket;
        int bufSize = 22 + strlen(r->operation());
        boost::scoped_array<unsigned char> buf (new unsigned char[bufSize]());
        buf.get()[0] = idl::MajorVersion;
        buf.get()[1] = idl::MinorVersion;
        std::memcpy(buf.get()+2, session.secret->get_buffer(), 16);
        std::memcpy(buf.get()+18, &credential.ticket, 4);
        std::memcpy(buf.get()+22, r->operation(), strlen(r->operation()));
        SHA256(buf.get(), bufSize, credential.hash);
        
        callerChain = getJoinedChain(conn, *r);
        if (std::strcmp(idl::BusLogin, session.remoteId)) 
        {
          idl::HashValue hash;
          CORBA::String_var connId = CORBA::string_dup(conn._login()->id);
          size_t idSize = strlen(connId);
          size_t remoteIdSize = strlen(session.remoteId);
          bufSize = idSize + remoteIdSize + idl::EncryptedBlockSize;
          buf.reset(new unsigned char[bufSize]());
          std::memcpy(buf.get(), connId, idSize);
          std::memcpy(buf.get() + idSize, session.remoteId, remoteIdSize);
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
            try
            {
              credential.chain = 
                *conn.access_control()->signChainFor(session.remoteId);
            }
            catch (const idl_ac::InvalidLogins &)
            {
              throw CORBA::NO_PERMISSION(idl_ac::InvalidTargetCode, 
                                         CORBA::COMPLETED_NO);
            }
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
      CORBA::OctetSeq_var o = _orb_info->codec->encode_value(any);
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
        o = _orb_info->codec->encode_value(lany);
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

void ClientInterceptor::receive_exception(PI::ClientRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::receive_exception");
  l.level_vlog(debug_level, "operation: %s", r->operation()); 
  l.level_vlog(debug_level, "exception: %s", r->received_exception_id()); 

  if (std::string(r->received_exception_id())
      == "IDL:omg.org/CORBA/NO_PERMISSION:1.0")
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
            any = _orb_info->codec->decode_value(o, idl_cr::_tc_CredentialReset);
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
          
          Connection::SecretSession session;
          session.id = credentialReset.session;
          session.remoteId = CORBA::string_dup(credentialReset.target);
          session.secret = secret;
          session.ticket = 0;
          {
#ifdef OPENBUS_SDK_MULTITHREAD
            boost::lock_guard<boost::mutex> lock(_mutex);
#endif
            conn._profile2session.insert(sessionKey, session);
          }
          l.log("Retransmissao da requisicao...");
					CORBA::Object_var _o = new CORBA::Object(*r->target());
          throw PI::ForwardRequest(_o, false);
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
          throw PI::ForwardRequest(r->target(), false); 
        }
        else if (conn._state == Connection::UNLOGGED) 
        {
          l.log("Connection::UNLOGGED: throw NoLoginCode");
          throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode,
                                     CORBA::COMPLETED_NO);          
        } 
        else if (conn._state == Connection::INVALID) 
        {
          if (!std::strcmp(conn._login()->id, oldLogin.id)) 
          {
            conn._logout();
            l.log("Connection::INVALID: throw NoLoginCode");
            throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode,
                                       CORBA::COMPLETED_NO);
          }
          else
          {
            throw PI::ForwardRequest(r->target(), false);
          }
        }
      }
    }
  }
}

void ClientInterceptor::send_poll(PI::ClientRequestInfo_ptr) 
{ 
}

void ClientInterceptor::receive_reply(PI::ClientRequestInfo_ptr) 
{ 
}

void ClientInterceptor::receive_other(PI::ClientRequestInfo_ptr) 
{ 
}

char *ClientInterceptor::name() 
{ 
  return CORBA::string_dup("ClientInterceptor"); 
}

void ClientInterceptor::destroy() 
{ 
}

}}
