#include <openssl/sha.h>
#include <sstream>

#include "openbus/interceptors/ClientInterceptor_impl.h"
#include "legacy/stubs/credential_v1_5.h"
#include "openbus/log.h"
#include "openbus/util/OpenSSL.h"
#include "openbus/util/AutoLock_impl.h"

namespace openbus {
namespace interceptors {    

PortableInterceptor::SlotId ClientInterceptor::_slotId_ignoreInterceptor;

/* monta uma indentificador(chave) para uma requisição através de uma hash do profile. */
std::string getSessionKey(PortableInterceptor::ClientRequestInfo *r) {
  idl::HashValue profileDataHash;
  ::IOP::TaggedProfile::_profile_data_seq profile = r->effective_profile()->profile_data;
  SHA256(profile.get_buffer(), profile.length(), profileDataHash);
  std::string sprofileDataHash((const char*) profileDataHash, idl::HashValueSize);
  return sprofileDataHash;
}

Connection &ClientInterceptor::getCurrentConnection(PortableInterceptor::ClientRequestInfo *r) {
  log_scope l(log.general_logger(),info_level,"ClientInterceptor::getCurrentConnection");
  Connection *conn = 0;
  CORBA::Any_var connectionAddrAny;
  connectionAddrAny = r->get_slot(_slotId_connectionAddr);
  idl::OctetSeq connectionAddrOctetSeq;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) {
    assert(connectionAddrOctetSeq.length() == sizeof(conn));
    std::memcpy(&conn, connectionAddrOctetSeq.get_buffer(), sizeof(conn));
  }
  assert(_manager);
  if(!conn)
    if (!(conn = _manager->getDefaultConnection()))
      throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
  assert(conn);
  l.vlog("connection:%p", conn);
  return *conn;
}

CallerChain *ClientInterceptor::getJoinedChain(PortableInterceptor::ClientRequestInfo *r) {
  CORBA::Any_var signedCallChainAny= r->get_slot(_slotId_joinedCallChain);
  idl_cr::SignedCallChain signedCallChain;
  if (*signedCallChainAny >>= signedCallChain) {
    CORBA::Any_var callChainAny = _cdrCodec->decode_value(signedCallChain.encoded,
      idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain) return new CallerChain(callChain.target, callChain.originators, 
      callChain.caller, signedCallChain);
    else return 0;
  } else return 0;
}

ClientInterceptor::ClientInterceptor(
  PortableInterceptor::SlotId slotId_connectionAddr,
  PortableInterceptor::SlotId slotId_joinedCallChain,
  PortableInterceptor::SlotId slotId_ignoreInterceptor,
  IOP::Codec *cdr_codec)
  : _cdrCodec(cdr_codec), 
    _slotId_connectionAddr(slotId_connectionAddr), 
    _slotId_joinedCallChain(slotId_joinedCallChain),
    _sessionLRUCache(SessionLRUCache(LOGINCACHE_LRU_SIZE)),
    _callChainLRUCache(CallChainLRUCache(LOGINCACHE_LRU_SIZE)),
    _manager(0)
{ 
  log_scope l(log.general_logger(), info_level,
    "ClientInterceptor::ClientInterceptor");
  _slotId_ignoreInterceptor = slotId_ignoreInterceptor;
}

ClientInterceptor::~ClientInterceptor() { }

void ClientInterceptor::send_request(PortableInterceptor::ClientRequestInfo *r){
  const char *operation = r->operation();
  log_scope l(log.general_logger(), debug_level, "ClientInterceptor::send_request");
  l.level_vlog(debug_level, "operation: %s", operation);

  /* esta chamada remota precisa ser interceptada? */
  if (!IgnoreInterceptor::status(r)) {
    Connection &conn = getCurrentConnection(r);
    AutoLock conn_mutex(&conn._mutex);
    if (conn._login()) {
      conn_mutex.unlock();
      CallerChain *callerChain = 0;
      IOP::ServiceContext serviceContext;
      serviceContext.context_id = idl_cr::CredentialContextId;

      /* montando uma credencial com os dados(busid e login) da conexão. */
      idl_cr::CredentialData credential;
      credential.bus = CORBA::string_dup(conn._busid);
      conn_mutex.lock();
      credential.login = CORBA::string_dup(conn._login()->id);
      conn_mutex.unlock();
      
      /* adquirindo uma chave para a sessão que corresponde a esta requisição. */
      std::string sessionKey = getSessionKey(r);
    
      SecretSession session;
      AutoLock m(&_mutex);
      if (_sessionLRUCache.exists(sessionKey)) {
        session = _sessionLRUCache.fetch(sessionKey);
        m.unlock();
        /* recuperando uma sessão para esta requisição. */
        credential.session = session.id;
        session.ticket = ++session.ticket;
        credential.ticket = session.ticket;
        int bufSize = 22 + strlen(operation);
        std::auto_ptr<unsigned char> buf (new unsigned char[bufSize]());
        unsigned char *pBuf = buf.get();
        pBuf[0] = idl::MajorVersion;
        pBuf[1] = idl::MinorVersion;
        memcpy(pBuf+2, session.secret->get_buffer(), SECRET_SIZE);
        memcpy(pBuf+18, &credential.ticket, 4);
        memcpy(pBuf+22, operation, strlen(operation));
        SHA256(pBuf, bufSize, credential.hash);
        
        callerChain = getJoinedChain(r);
        if (strcmp(idl::BusLogin, session.remoteId.in())) {
          /* esta requisição não é para o barramento, então preciso assinar essa cadeia. */
          /* montando uma hash para consultar o cache de cadeias assinadas. */
          idl::HashValue hash;
          conn_mutex.lock();
          CORBA::String_var connId = CORBA::string_dup(conn._login()->id);
          conn_mutex.unlock();
          size_t idSize = strlen(connId);
          size_t remoteIdSize = strlen(session.remoteId.in());
          bufSize = idSize + remoteIdSize + idl::EncryptedBlockSize;
          buf.reset(new unsigned char[bufSize]());
          unsigned char *pBuf = buf.get();
          memcpy(pBuf, connId, idSize);
          memcpy(pBuf+idSize, session.remoteId.in(), remoteIdSize);
          if (callerChain) memcpy(pBuf+idSize+remoteIdSize, 
            callerChain->signedCallChain()->signature, idl::EncryptedBlockSize);
          else memset(pBuf+idSize+remoteIdSize, '\0', idl::EncryptedBlockSize);
          SHA256(pBuf, bufSize, hash);
          std::string shash((const char*) hash, idl::HashValueSize);
          idl_cr::SignedCallChain signedCallChain;
          AutoLock m(&_mutex);
          if (_callChainLRUCache.exists(shash)) {
            signedCallChain = _callChainLRUCache.fetch(shash);
            m.unlock();
            l.level_vlog(debug_level,"Recuperando signedCallChain. remoteid: %s", 
              session.remoteId.in());
            credential.chain = signedCallChain;
          } else {
            m.unlock();
            credential.chain = *conn.access_control()->signChainFor(session.remoteId.in());
            m.lock();
            _callChainLRUCache.insert(shash, credential.chain);
          }
        } else
          if (callerChain) credential.chain = *callerChain->signedCallChain();
          else memset(credential.chain.signature, '\0', idl::EncryptedBlockSize);
      } else {
        m.unlock();
        /* montando uma credencial com o propósito de requisitar o estabelecimento de uma 
        ** nova sessão. */
        credential.ticket = 0;
        credential.session = 0;
        memset(credential.hash, '\0', idl::HashValueSize);
        memset(credential.chain.signature, '\0', idl::EncryptedBlockSize);
      }
      m.unlock();
      
      /* anexando a credencial a esta requisição. */
      CORBA::Any any;
      any <<= credential;
      CORBA::OctetSeq_var o = _cdrCodec->encode_value(any);
      IOP::ServiceContext::_context_data_seq s(o->length(), o->length(), o->get_buffer(), 0);
      serviceContext.context_data = s;
      r->add_request_service_context(serviceContext, true);
      
      /* anexando uma credencial legacy a esta requisição. */
      IOP::ServiceContext legacyContext;
      legacyContext.context_id = 1234;
      legacy::v1_5::Credential legacyCredential;
      conn_mutex.lock();
      legacyCredential.identifier = CORBA::string_dup(conn._login()->id);
      legacyCredential.owner = CORBA::string_dup(conn._login()->entity);
      conn_mutex.unlock();
      if (callerChain && (callerChain->_originators.length() > 1))
        legacyCredential.delegate = CORBA::string_dup(callerChain->_originators[0].entity);
      else legacyCredential.delegate = "";
      CORBA::Any lany;
      lany <<= legacyCredential;
      o = _cdrCodec->encode_value(lany);
      IOP::ServiceContext::_context_data_seq ls(o->length(), o->length(), o->get_buffer(), 0);
      legacyContext.context_data = ls;
      r->add_request_service_context(legacyContext, true);
    } else throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);          
  }
}

void ClientInterceptor::receive_exception(PortableInterceptor::ClientRequestInfo *r)
{
  const char *operation = r->operation();
  log_scope l(log.general_logger(), debug_level, "ClientInterceptor::receive_exception");
  l.level_vlog(debug_level, "operation: %s", operation); 
  l.level_vlog(debug_level, "exception: %s", r->received_exception_id()); 

  if (!strcmp(r->received_exception_id(), "IDL:omg.org/CORBA/NO_PERMISSION:1.0")) {
    CORBA::SystemException *ex = CORBA::SystemException::_decode(*r->received_exception());
    if (ex->completed() == CORBA::COMPLETED_NO) {
      l.level_vlog(debug_level, "minor: %d", ex->minor());
      Connection &conn = getCurrentConnection(r);
      if (ex->minor() == idl_ac::InvalidCredentialCode) {
        l.level_vlog(debug_level, "creating credential session");
        IOP::ServiceContext_var sctx;
        if (sctx = r->get_request_service_context(idl_cr::CredentialContextId)) {
          /* montando CredentialReset que foi enviado por quem está respondendo a um pedido de 
          ** inicialização de uma sessão. */
          CORBA::ULong len = sctx->context_data.length();
          CORBA::OctetSeq o(len, len, sctx->context_data.get_buffer());
          CORBA::Any_var any;
          try {
            any = _cdrCodec->decode_value(o, idl_cr::_tc_CredentialReset);
          } catch (CORBA::Exception&) {
            throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, CORBA::COMPLETED_NO);
          }
          idl_cr::CredentialReset credentialReset;
          if (!(any >>= credentialReset)) 
            throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, CORBA::COMPLETED_NO);
        
          /* decifrar o segredo usando a chave do usuário. */
          CORBA::OctetSeq_var secret = openssl::decrypt(conn.__key(), credentialReset.challenge, 
            idl::EncryptedBlockSize);

          /* adquirindo uma chave para a sessão que corresponde a esta requisição. */
          std::string sessionKey = getSessionKey(r);
          
          /* criando uma sessão. */
          SecretSession session;
          session.id = credentialReset.session;
          session.remoteId = CORBA::string_dup(credentialReset.login);
          session.secret = secret;
          session.ticket = 0;
          {
            AutoLock m(&_mutex);
            _sessionLRUCache.insert(sessionKey, session);
          }
          l.log("Retransmissao da requisicao...");
          /* retransmitindo a requisição após ter estabelecido uma sessão. */
          throw PortableInterceptor::ForwardRequest(r->target(), false);
        } throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, CORBA::COMPLETED_NO);
      } else if (ex->minor() == idl_ac::NoCredentialCode) {
        throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, CORBA::COMPLETED_NO);
      } else if (ex->minor() == idl_ac::InvalidLoginCode) {
        AutoLock conn_mutex(&conn._mutex);
        idl_ac::LoginInfo oldLogin = *conn._loginInfo;
        const char *oldBusid = CORBA::string_dup(conn._busid);
        conn._state = Connection::INVALID;
        conn_mutex.unlock();
        Connection::InvalidLoginCallback_ptr callback = conn.onInvalidLogin();
        bool callbackError = false;
        try {
          if (callback) (callback)(conn, oldLogin, oldBusid);
        } catch (...) {
          callbackError = true;
        }
        if (conn._state == Connection::LOGGED)
          throw PortableInterceptor::ForwardRequest(r->target(), false);            
        else if (conn._state == Connection::UNLOGGED) 
          throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
        else if ((conn._state == Connection::INVALID) || callbackError) {
          if (!strcmp(conn._login()->id.in(), oldLogin.id.in())) {
            conn._logout(true);
            throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
          } else throw PortableInterceptor::ForwardRequest(r->target(), false);
        }
      }
    }
  }
}

}
}
