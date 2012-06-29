#include <iostream>
#include <openssl/sha.h>
#include <openssl/x509.h>

#include <openbus/interceptors/serverInterceptor_impl.h>
#include <legacy/stubs/credential_v1_5.h>
#include <openbus/util/openssl.h>
#include <openbus/log.h>
#include <openbus/util/autolock_impl.h>

namespace openbus {
namespace interceptors {

ServerInterceptor::ServerInterceptor(
  PortableInterceptor::Current* piCurrent, 
  PortableInterceptor::SlotId slotId_connectionAddr,
  PortableInterceptor::SlotId slotId_joinedCallChain,
  PortableInterceptor::SlotId slotId_signedCallChain, 
  PortableInterceptor::SlotId slotId_legacyCallChain,
  IOP::Codec* cdr_codec) 
  : _piCurrent(piCurrent), _slotId_connectionAddr(slotId_connectionAddr),
    _slotId_joinedCallChain(slotId_joinedCallChain),_slotId_signedCallChain(slotId_signedCallChain),
    _slotId_legacyCallChain(slotId_legacyCallChain), _cdrCodec(cdr_codec), _manager(0),
    _sessionLRUCache(SessionLRUCache(LOGINCACHE_LRU_SIZE))
{
  log_scope l(log.general_logger(), debug_level,"ServerInterceptor::ServerInterceptor");
}

void ServerInterceptor::sendCredentialReset(Connection* conn, Login* caller, 
  PortableInterceptor::ServerRequestInfo* r) 
{
  AutoLock m(&_mutex);
  /* estabelecer uma nova sessão e enviar um CredentialReset para o cliente. */
  CORBA::ULong newSessionId = _sessionLRUCache.size() + 1;
  Session* session = new Session(newSessionId);
  _sessionLRUCache.insert(newSessionId, session);
  m.unlock();
  
  /* cifrando o segredo com a chave pública do cliente. */
  unsigned char* encrypted = openssl::encrypt(caller->key, session->secret, SECRET_SIZE); 

  idl_cr::CredentialReset credentialReset;
  AutoLock conn_mutex(&conn->_mutex);
  credentialReset.login = conn->login()->id;
  conn_mutex.unlock();
  credentialReset.session = session->id;
  memcpy(credentialReset.challenge, encrypted, 256);
  OPENSSL_free(encrypted);

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

void ServerInterceptor::receive_request_service_contexts(PortableInterceptor::ServerRequestInfo* r)
{
  const char* operation = r->operation();
  log_scope l(log.general_logger(), debug_level,
    "ServerInterceptor::receive_request_service_contexts");
  l.level_vlog(debug_level, "operation: %s", operation);
  
  /* extraindo a credencial desta requisição. */
  CORBA::Any_var any;
  bool hasContext = true;
  try {
    IOP::ServiceContext_var sc = r->get_request_service_context(idl_cr::CredentialContextId);
    IOP::ServiceContext::_context_data_seq& cd = sc->context_data;
    CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);    
    any = _cdrCodec->decode_value(contextData, idl_cr::_tc_CredentialData);
  } catch (CORBA::BAD_PARAM&) {
    hasContext = false;
  }
  idl_cr::CredentialData credential;
  if (hasContext && (any >>= credential)) {
    Connection* conn = _manager->getDispatcher(credential.bus);
    if (!conn) conn = _manager->getDefaultConnection();
    if (!conn) throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);

    /* disponibilizando a conexão para o usuário (getRequester()) e configurando a conexão 
    ** para ser utilizada por esta thread até o término do tratamento desta requisição. */
    size_t size = sizeof(Connection*);
    unsigned char buf[size];
    memcpy(buf, &conn, size);
    idl::OctetSeq_var connectionAddrOctetSeq = new idl::OctetSeq(size, size, buf);
    CORBA::Any connectionAddrAny;
    connectionAddrAny <<= *(connectionAddrOctetSeq);
    r->set_slot(_slotId_connectionAddr, connectionAddrAny);
    _manager->setRequester(conn);

    AutoLock conn_mutex(&conn->_mutex);
    if (conn->login()) {
    conn_mutex.unlock();
      Login* caller;
      /* consulta ao cache de logins para saber se este login é valido. 
      ** obtenção da estrutura Login referente a este login id. (caller) */
      try {
        caller = conn->_loginCache->validateLogin(credential.login);
      } catch(CORBA::Exception&) {
        /* não há uma entrada válida no cache e o cache nõo consegui validar o login 
        ** com o barramento. */
        throw CORBA::NO_PERMISSION(idl_ac::UnverifiedLoginCode, CORBA::COMPLETED_NO);
      }
      /* o login do caller é valido? */
      if (caller) {
        idl::HashValue hash;        
        Session* session;
        AutoLock m(&_mutex);
        if (_sessionLRUCache.fetch(credential.session, session)) {
          /* montando uma hash com os dados da credencial recebida e da sessão existente. */
          size_t slenOperation = strlen(r->operation());
          int slen = 22 + slenOperation;
          unsigned char* s = new unsigned char[slen];
          s[0] = idl::MajorVersion;
          s[1] = idl::MinorVersion;
          memcpy(s+2, session->secret, SECRET_SIZE);
          memcpy(s+18, &credential.ticket, 4);
          memcpy(s+22, r->operation(), slenOperation);
          SHA256(s, slen, hash);
        }
        
        if (session && !memcmp(hash, credential.hash, 32) && 
            tickets_check(&session->ticketsHistory, credential.ticket)) 
        {
          /* a credencial recebida é válida. */
          l.level_vlog(debug_level, "credential is valid");

          bool sendInvalidChainCode = false;
          if (credential.chain.encoded.length()) {
            idl::HashValue hash;
            SHA256(credential.chain.encoded.get_buffer(), credential.chain.encoded.length(), hash);

            conn_mutex.lock();
            EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(conn->buskey(), 0);
            conn_mutex.unlock();
            assert(ctx);
            int status = EVP_PKEY_verify_init(ctx);
            assert(status);
            if (EVP_PKEY_verify(ctx, credential.chain.signature, 256, hash, 32) != 1)
              sendInvalidChainCode = true;
            else {
              CORBA::Any_var callChainAny = _cdrCodec->decode_value(
                credential.chain.encoded, idl_ac::_tc_CallChain);
              idl_ac::CallChain callChain;
              callChainAny >>= callChain;
              conn_mutex.lock();
              if (strcmp(callChain.target, conn->login()->id)) { 
                conn_mutex.unlock();
                /* a cadeia tem como destino(target) outro login. */
                m.unlock();
                sendCredentialReset(conn, caller, r);
              } else if(strcmp(callChain.caller.id, caller->loginInfo->id)) {
                /* o último elemento da cadeia não é quem está me chamando. */
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
      IOP::ServiceContext::_context_data_seq& cd = sc->context_data;
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
