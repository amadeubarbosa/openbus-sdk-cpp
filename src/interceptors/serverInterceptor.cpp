#include <interceptors/serverInterceptor_impl.h>
#include <connection_impl.h>
#include <legacy/stubs/credential_v1_05.h>
#include <util/openssl.h>
#include <log.h>

#include <iostream>
#include <openssl/sha.h>
#include <openssl/x509.h>

namespace openbus {
namespace interceptors {
ServerInterceptor::ServerInterceptor(
  PortableInterceptor::Current* piCurrent, 
  PortableInterceptor::SlotId slotId_connectionAddr,
  PortableInterceptor::SlotId slotId_joinedCallChain,
  PortableInterceptor::SlotId slotId_signedCallChain, 
  PortableInterceptor::SlotId slotId_legacyCallChain,
  PortableInterceptor::SlotId slotId_busid, 
  IOP::Codec* cdr_codec) 
  : _piCurrent(piCurrent), _slotId_connectionAddr(slotId_connectionAddr),
    _slotId_joinedCallChain(slotId_joinedCallChain), 
    _slotId_signedCallChain(slotId_signedCallChain),
    _slotId_legacyCallChain(slotId_legacyCallChain), _slotId_busid(slotId_busid), 
    _cdrCodec(cdr_codec), 
    _manager(0) 
{
  log_scope l(log.server_interceptor_logger(), debug_level,
    "ServerInterceptor::ServerInterceptor");
}

ServerInterceptor::~ServerInterceptor() { }

void ServerInterceptor::receive_request_service_contexts(
  PortableInterceptor::ServerRequestInfo* r)
  throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
{
  const char* operation = r->operation();
  log_scope l(log.server_interceptor_logger(), debug_level,
    "ServerInterceptor::receive_request_service_contexts");
  l.level_vlog(debug_level, "operation: %s", operation);
  
  /* extraindo a credencial desta requisição. */
  IOP::ServiceContext_var sc = r->get_request_service_context(idl_cr::CredentialContextId);
  IOP::ServiceContext::_context_data_seq& cd = sc->context_data;
  CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);    
  CORBA::Any_var any = _cdrCodec->decode_value(contextData, idl_cr::_tc_CredentialData);
  idl_cr::CredentialData credential;
  if (any >>= credential) {
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
    
    if (conn->login()) {
      Login* caller;
      /* consulta ao cache de logins para saber se este login é valido. 
      ** obtenção da estrutura Login referente a este login id. (caller) */
      try {
        caller = conn->_loginCache->validateLogin(credential.login);
      } catch(CORBA::SystemException &e) {
        /* não há uma entrada válida no cache e o cache nõo consegui validar o login 
        ** com o barramento. */
        throw CORBA::NO_PERMISSION(idl_ac::UnverifiedLoginCode, CORBA::COMPLETED_NO);
      }
      /* o login do caller é valido? */
      if (caller) {
        SecretSession* session = 0;
        idl::HashValue hash;
        if (_idSecretSession.find(credential.session) != _idSecretSession.end()) {
          /* montando uma hash com os dados da credencial recebida e da sessão existente. */
          session = _idSecretSession[credential.session];
          size_t slenOperation = strlen(r->operation());
          int slen = 22 + slenOperation;
          unsigned char* s = new unsigned char[slen];
          s[0] = 2;
          s[1] = 0;
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
          //?
          bool invalidChain = false;
          if (credential.chain.encoded.length()) {
            idl::HashValue hash;
            SHA256(credential.chain.encoded.get_buffer(), credential.chain.encoded.length(), hash);

            EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(conn->busKey(), 0);
            assert(ctx);
            assert(EVP_PKEY_verify_init(ctx));
            if (EVP_PKEY_verify(ctx, credential.chain.signature, 256, hash, 32) != 1) {
              invalidChain = true;
            } else {
              CORBA::Any_var callChainAny = _cdrCodec->decode_value(
                credential.chain.encoded,
                idl_ac::_tc_CallChain);
              idl_ac::CallChain callChain;
              callChainAny >>= callChain;
              if (strcmp(callChain.target, conn->login()->id) ||
                 (strcmp(callChain.callers[callChain.callers.length()-1].id, 
                 caller->loginInfo->id)))
              {
                invalidChain = true;
              } else {
                CORBA::Any signedCallChainAny;
                signedCallChainAny <<= credential.chain;
                r->set_slot(_slotId_signedCallChain, signedCallChainAny);
                CORBA::Any busidAny;
                busidAny <<= credential.bus;
                r->set_slot(_slotId_busid, busidAny);
              }
            }
          } else invalidChain = true;
         if (invalidChain) 
           throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
        } else {
          l.level_vlog(debug_level, "credential not valid, try to reset credetial session");

          /* estabelecer uma nova sessão e enviar um CredentialReset para o cliente. */
          CORBA::ULong newSessionId = _idSecretSession.size() + 1;
          SecretSession* secretSession = new SecretSession(newSessionId);
          _idSecretSession[newSessionId] = secretSession;
          
          /* cifrando o segredo com a chave pública do cliente. */
          unsigned char* encrypted= openssl::encrypt(caller->key,secretSession->secret,SECRET_SIZE); 

          idl_cr::CredentialReset credentialReset;
          credentialReset.login = conn->login()->id;
          credentialReset.session = secretSession->id;
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
      } else throw CORBA::NO_PERMISSION(idl_ac::InvalidLoginCode, CORBA::COMPLETED_NO);
    } else throw CORBA::NO_PERMISSION(idl_ac::UnverifiedLoginCode, CORBA::COMPLETED_NO);
  } else {
    IOP::ServiceContext_var sc = r->get_request_service_context(1234);
    IOP::ServiceContext::_context_data_seq& cd = sc->context_data;
    CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);    
    CORBA::Any_var lany = _cdrCodec->decode_value(contextData, 
      openbus::legacy::v1_05::_tc_Credential);
    openbus::legacy::v1_05::Credential legacyCredential;
    if (lany >>= legacyCredential) {
      idl_ac::CallChain legacyChain;
      legacyChain.target = "";
      if (strcmp(legacyCredential.delegate, "")) {
        legacyChain.callers.length(2);
        idl_ac::LoginInfo delegate;
        delegate.id = "<unknown>";
        delegate.entity = legacyCredential.delegate;
        legacyChain.callers[0] = delegate;
        idl_ac::LoginInfo login;
        login.id = legacyCredential.identifier;
        login.entity = legacyCredential.owner;
        legacyChain.callers[0] = login;
      } else {
        legacyChain.callers.length(1);
        idl_ac::LoginInfo loginInfo;
        loginInfo.id = legacyCredential.identifier;
        loginInfo.entity = legacyCredential.owner;
        legacyChain.callers[0] = loginInfo;            
      }
      CORBA::Any legacyChainAny;
      legacyChainAny <<= legacyChain;
      r->set_slot(_slotId_legacyCallChain, legacyChainAny);
    } else throw CORBA::NO_PERMISSION(idl_ac::NoCredentialCode, CORBA::COMPLETED_NO);
  }
}
}
}
