#include <iostream>
#include <openssl/sha.h>
#include <openssl/x509.h>

#include "openbus/interceptors/ServerInterceptor_impl.h"
#include "legacy/stubs/credential_v1_5.h"
#include "openbus/util/OpenSSL.h"
#include "openbus/log.h"
#include "openbus/util/AutoLock_impl.h"

namespace openbus {
namespace interceptors {

ServerInterceptor::ServerInterceptor(
  PortableInterceptor::Current *piCurrent, 
  PortableInterceptor::SlotId slotId_connectionAddr,
  PortableInterceptor::SlotId slotId_joinedCallChain,
  PortableInterceptor::SlotId slotId_signedCallChain, 
  PortableInterceptor::SlotId slotId_legacyCallChain,
  IOP::Codec *cdr_codec) 
  : _piCurrent(piCurrent), _slotId_connectionAddr(slotId_connectionAddr),
    _slotId_joinedCallChain(slotId_joinedCallChain),_slotId_signedCallChain(slotId_signedCallChain),
    _slotId_legacyCallChain(slotId_legacyCallChain), _cdrCodec(cdr_codec), _manager(0),
    _sessionLRUCache(SessionLRUCache(LOGINCACHE_LRU_SIZE))
{
  log_scope l(log.general_logger(), debug_level,"ServerInterceptor::ServerInterceptor");
}

void ServerInterceptor::sendCredentialReset(Connection *conn, Login *caller, 
  PortableInterceptor::ServerRequestInfo *r) 
{
  /* estabelecer uma nova sess�o e enviar um CredentialReset para o cliente. */
  Session session;
  AutoLock m(&_mutex);
  session.id = _sessionLRUCache.size() + 1;
  _sessionLRUCache.insert(session.id, session);
  m.unlock();
  
  /* cifrando o segredo com a chave p�blica do cliente. */
  CORBA::OctetSeq_var encrypted = openssl::encrypt(caller->key, session.secret, SECRET_SIZE); 

  idl_cr::CredentialReset credentialReset;
  AutoLock conn_mutex(&conn->_mutex);
  credentialReset.login = conn->_login()->id;
  credentialReset.session = session.id;
  conn_mutex.unlock();
  memcpy(credentialReset.challenge, encrypted->get_buffer(), idl::EncryptedBlockSize);

  CORBA::Any any;
  any <<= credentialReset;
  CORBA::OctetSeq_var o = _cdrCodec->encode_value(any);

  /* anexando CredentialReset a resposta para o cliente. */
  IOP::ServiceContext serviceContext;
  serviceContext.context_id = idl_cr::CredentialContextId;
  IOP::ServiceContext::_context_data_seq s(o->length(), o->length(), o->get_buffer(), 0);
  serviceContext.context_data = s;
  r->add_reply_service_context(serviceContext, true);          

  throw CORBA::NO_PERMISSION(idl_ac::InvalidCredentialCode, CORBA::COMPLETED_NO);              
}

ServerInterceptor::~ServerInterceptor() { }

void ServerInterceptor::receive_request_service_contexts(PortableInterceptor::ServerRequestInfo *r)
{
  const char *operation = r->operation();
  log_scope l(log.general_logger(), debug_level,
    "ServerInterceptor::receive_request_service_contexts");
  l.level_vlog(debug_level, "operation: %s", operation);
  
  /* extraindo a credencial desta requisi��o. */
  CORBA::Any_var any;
  bool hasContext = true;
  try {
    IOP::ServiceContext_var sc = r->get_request_service_context(idl_cr::CredentialContextId);
    IOP::ServiceContext::_context_data_seq &cd = sc->context_data;
    CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);    
    any = _cdrCodec->decode_value(contextData, idl_cr::_tc_CredentialData);
  } catch (CORBA::BAD_PARAM&) {
    hasContext = false;
  }
  idl_cr::CredentialData credential;
  if (hasContext && (any >>= credential)) {
    Connection *conn = _manager->getDispatcher(credential.bus);
    if (!conn) conn = _manager->getDefaultConnection();
    if (!conn) throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);

    /* disponibilizando a conex�o para o usu�rio (getRequester()) e configurando a conex�o 
    ** para ser utilizada por esta thread at� o t�rmino do tratamento desta requisi��o. */
    size_t bufSize = sizeof(Connection*);
    unsigned char buf[bufSize];
    memcpy(buf, &conn, bufSize);
    idl::OctetSeq_var connectionAddrOctetSeq = new idl::OctetSeq(bufSize, bufSize, buf);
    CORBA::Any connectionAddrAny;
    connectionAddrAny <<= *(connectionAddrOctetSeq);
    r->set_slot(_slotId_connectionAddr, connectionAddrAny);
    _manager->setRequester(conn);

    AutoLock conn_mutex(&conn->_mutex);
    if (conn->_login()) {
    conn_mutex.unlock();
      Login *caller;
      /* consulta ao cache de logins para saber se este login � valido. 
      ** obten��o da estrutura Login referente a este login id. (caller) */
      try {
        caller = conn->_loginCache->validateLogin(credential.login);
      } catch(CORBA::Exception&) {
        /* n�o h� uma entrada v�lida no cache e o cache n�o consegui validar o login 
        ** com o barramento. */
        throw CORBA::NO_PERMISSION(idl_ac::UnverifiedLoginCode, CORBA::COMPLETED_NO);
      }
      /* o login do caller � valido? */
      if (caller) {
        idl::HashValue hash;   
        Session session;     
        AutoLock m(&_mutex);
        bool hasSession = _sessionLRUCache.exists(credential.session);
        if (hasSession) {
          session = _sessionLRUCache.fetch(credential.session);
          /* montando uma hash com os dados da credencial recebida e da sess�o existente. */
          size_t operationSize = strlen(r->operation());
          int bufSize = 22 + operationSize;
          std::auto_ptr<unsigned char> buf(new unsigned char[bufSize]());
          unsigned char *pBuf = buf.get();
          pBuf[0] = idl::MajorVersion;
          pBuf[1] = idl::MinorVersion;
          memcpy(pBuf+2, session.secret, SECRET_SIZE);
          memcpy(pBuf+18, &credential.ticket, 4);
          memcpy(pBuf+22, r->operation(), operationSize);
          SHA256(pBuf, bufSize, hash);
        }
        
        if (hasSession &!memcmp(hash, credential.hash, idl::HashValueSize) &
            tickets_check(&session.ticketsHistory, credential.ticket)) 
        {
          /* a credencial recebida � v�lida. */
          l.level_vlog(debug_level, "credential is valid");

          bool sendInvalidChainCode = false;
          if (credential.chain.encoded.length()) {
            idl::HashValue hash;
            SHA256(credential.chain.encoded.get_buffer(), credential.chain.encoded.length(), hash);

            conn_mutex.lock();
            EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(conn->__buskey(), 0);
            conn_mutex.unlock();
            assert(ctx);
            int status = EVP_PKEY_verify_init(ctx);
            assert(status);
            if (EVP_PKEY_verify(ctx, credential.chain.signature, idl::EncryptedBlockSize, hash, 
            idl::HashValueSize) != 1)
              sendInvalidChainCode = true;
            else {
              CORBA::Any_var callChainAny = _cdrCodec->decode_value(
                credential.chain.encoded, idl_ac::_tc_CallChain);
              idl_ac::CallChain callChain;
              callChainAny >>= callChain;
              conn_mutex.lock();
              int res = strcmp(callChain.target, conn->_login()->id);
              conn_mutex.unlock();
              if (res) { 
                /* a cadeia tem como destino(target) outro login. */
                m.unlock();
                sendCredentialReset(conn, caller, r);
              } else if(strcmp(callChain.caller.id, caller->loginInfo->id)) {
                /* o �ltimo elemento da cadeia n�o � quem est� me chamando. */
                sendInvalidChainCode = true;
              } else {
                CORBA::Any signedCallChainAny;
                signedCallChainAny <<= credential.chain;
                r->set_slot(_slotId_signedCallChain, signedCallChainAny);
              }
            }
          } else sendInvalidChainCode = true;
          if (sendInvalidChainCode) 
            throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
        } else {
          l.level_vlog(debug_level, "credential not valid, try to reset credetial session");
          m.unlock();
          sendCredentialReset(conn, caller, r);
        }
      } else throw CORBA::NO_PERMISSION(idl_ac::InvalidLoginCode, CORBA::COMPLETED_NO);
    } else throw CORBA::NO_PERMISSION(idl_ac::UnverifiedLoginCode, CORBA::COMPLETED_NO);
  } else {
    l.level_vlog(debug_level, "verificando se existe uma credencial legacy");
    hasContext = true;
    CORBA::Any_var lany;
    try {
      IOP::ServiceContext_var sc = r->get_request_service_context(1234);
      IOP::ServiceContext::_context_data_seq &cd = sc->context_data;
      CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);    
      lany = _cdrCodec->decode_value(contextData, openbus::legacy::v1_5::_tc_Credential);
    } catch (CORBA::BAD_PARAM&) {
      hasContext = false;
    }
    openbus::legacy::v1_5::Credential legacyCredential;
    if (hasContext && (lany >>= legacyCredential)) {
      l.level_vlog(info_level, "extraindo credencial legacy");
      l.level_vlog(debug_level, "credential.identifier: %s", legacyCredential.identifier.in());
      l.level_vlog(debug_level, "credential.owner: %s", legacyCredential.owner.in());
      l.level_vlog(debug_level, "credential.delegate: %s", legacyCredential.delegate.in());
      idl_ac::CallChain legacyChain;
      legacyChain.target = "";
      if (strcmp(legacyCredential.delegate, "")) {
        legacyChain.originators.length(1);
        idl_ac::LoginInfo delegate;
        delegate.id = "<unknown>";
        delegate.entity = legacyCredential.delegate;
        legacyChain.originators[0] = delegate;
        idl_ac::LoginInfo login;
        login.id = legacyCredential.identifier;
        login.entity = legacyCredential.owner;
        legacyChain.caller = login;
      } else {
        legacyChain.originators.length(0);
        idl_ac::LoginInfo loginInfo;
        loginInfo.id = legacyCredential.identifier;
        loginInfo.entity = legacyCredential.owner;
        legacyChain.caller = loginInfo;            
      }
      CORBA::Any legacyChainAny;
      legacyChainAny <<= legacyChain;
      r->set_slot(_slotId_legacyCallChain, legacyChainAny);
    } else throw CORBA::NO_PERMISSION(idl_ac::NoCredentialCode, CORBA::COMPLETED_NO);
  }
}

}
}
