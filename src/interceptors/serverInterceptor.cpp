#include <interceptors/serverInterceptor_impl.h>
#include <connection_impl.h>
#include <legacy/stubs/credential_v1_05.h>

#include <iostream>
#include <openssl/sha.h>
#include <openssl/x509.h>

namespace openbus {
  namespace interceptors {
    ServerInterceptor::ServerInterceptor(
      PortableInterceptor::Current* piCurrent, 
      PortableInterceptor::SlotId slotId_joinedCallChain,
      PortableInterceptor::SlotId slotId_signedCallChain, 
      PortableInterceptor::SlotId slotId_legacyCallChain,
      PortableInterceptor::SlotId slotId_busid, 
      IOP::Codec* cdr_codec) 
      : _piCurrent(piCurrent),  _slotId_joinedCallChain(slotId_joinedCallChain), 
        _slotId_signedCallChain(slotId_signedCallChain),
        _slotId_legacyCallChain(slotId_legacyCallChain), _slotId_busid(slotId_busid), 
        _cdrCodec(cdr_codec), 
        _manager(0) 
    { }
    
    ServerInterceptor::~ServerInterceptor() { }
    
    void ServerInterceptor::send_reply(PortableInterceptor::ServerRequestInfo* r)
      throw (CORBA::SystemException) 
    {
      std::cout << "[thread: " << MICOMT::Thread::self() << "] send_reply: " 
      << r->operation() << std::endl;
      
      #ifdef OPENBUS_SDK_MULTITHREAD
      MICOMT::Thread::set_specific(_manager->_threadConnectionDispatcherKey, 0);
      #else
      if (_manager)
        if (Connection* c = _manager->_userDefaultConnection) _manager->setDefaultConnection(c);
      #endif
    }
    
    void ServerInterceptor::send_exception(PortableInterceptor::ServerRequestInfo* r)
      throw (CORBA::SystemException, PortableInterceptor::ForwardRequest) 
    {
      std::cout << "[thread: " << MICOMT::Thread::self() << "] send_exception: " 
      << r->operation() << std::endl;
      
      #ifdef OPENBUS_SDK_MULTITHREAD
      MICOMT::Thread::set_specific(_manager->_threadConnectionDispatcherKey, 0);
      #else
      if (_manager)
        if (Connection* c = _manager->_userDefaultConnection) _manager->setDefaultConnection(c);
      #endif
    }
    
    void ServerInterceptor::send_other(PortableInterceptor::ServerRequestInfo* r)
      throw (CORBA::SystemException, PortableInterceptor::ForwardRequest) 
    { 
      std::cout << "[thread: " << MICOMT::Thread::self() << "] send_other: " 
      << r->operation() << std::endl;
      
      #ifdef OPENBUS_SDK_MULTITHREAD
      MICOMT::Thread::set_specific(_manager->_threadConnectionDispatcherKey, 0);
      #else
      if (_manager)
        if (Connection* c = _manager->_userDefaultConnection) _manager->setDefaultConnection(c);
      #endif
    }
    
    void ServerInterceptor::receive_request_service_contexts(
      PortableInterceptor::ServerRequestInfo* ri)
      throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
      std::cout << "[thread: " << MICOMT::Thread::self() << "] receive_request_service_contexts: " 
      << ri->operation() << std::endl;
      
      /* [doubt] o que eu devo fazer se eu não conseguir extrair a credencial neste ponto em
      ** que não tenho conexão?
      */
      IOP::ServiceContext_var sc = ri->get_request_service_context(idl_cr::CredentialContextId);
      IOP::ServiceContext::_context_data_seq& cd = sc->context_data;
      CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);    
      CORBA::Any_var any = _cdrCodec->decode_value(contextData, idl_cr::_tc_CredentialData);
      idl_cr::CredentialData credential;
      if (any >>= credential) {
        Connection* conn;
        if (_manager) {
          conn = _manager->getBusDispatcher(credential.bus);
          if (conn) {
            #ifdef OPENBUS_SDK_MULTITHREAD
            MICOMT::Thread::set_specific(_manager->_threadConnectionDispatcherKey, conn);
            #else
            _manager->_userDefaultConnection = _manager->getDefaultConnection();
            _manager->setDefaultConnection(conn);
            #endif
          } else conn = _manager->getDefaultConnection();
        }
        if (!conn) throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
        
        if (conn->login()) {
          Login* caller;
          try {
            caller = conn->_loginCache->validateLogin(credential.login);
          } catch(CORBA::SystemException &e) {
            throw CORBA::NO_PERMISSION(idl_ac::UnverifiedLoginCode, CORBA::COMPLETED_NO);
          }
          if (caller) {
            SecretSession* session = 0;
            idl::HashValue hash;
            if (_idSecretSession.find(credential.session) != _idSecretSession.end()) 
            {
              session = _idSecretSession[credential.session];
              size_t slenOperation = strlen(ri->operation());
              int slen = 22 + slenOperation;
              unsigned char* s = new unsigned char[slen];
              s[0] = 2;
              s[1] = 0;
              memcpy(s+2, session->secret, SECRET_SIZE);
              memcpy(s+18, &credential.ticket, 4);
              memcpy(s+22, ri->operation(), slenOperation);
              SHA256(s, slen, hash);
            }

            if (session && !memcmp(hash, credential.hash, 32) && 
                tickets_check(&session->ticketsHistory, credential.ticket)) 
            {
              std::cout << "credential is valid." << std::endl;
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
                    ri->set_slot(_slotId_signedCallChain, signedCallChainAny);
                    CORBA::Any busidAny;
                    busidAny <<= credential.bus;
                    ri->set_slot(_slotId_busid, busidAny);
                  }
                }
              } else invalidChain = true;
             if (invalidChain) 
               throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
            } else {
              //credential not valid, try to reset credetial session
              std::cout << "[thread: " << MICOMT::Thread::self() 
                << "] receive_request_service_contexts: credential not valid, try to reset credetial session" 
                << std::endl;
              CORBA::ULong newSessionId = _idSecretSession.size() + 1;
              SecretSession* secretSession = new SecretSession(newSessionId);
              _idSecretSession[newSessionId] = secretSession;

              EVP_PKEY_CTX* ctx;
              unsigned char* encrypted;
              size_t encryptedLen;
              if (!((ctx = EVP_PKEY_CTX_new(caller->key, 0)) &&
                  (EVP_PKEY_encrypt_init(ctx) > 0) &&
                  (EVP_PKEY_encrypt(
                    ctx, 
                    0, 
                    &encryptedLen,
                    secretSession->secret,
                    SECRET_SIZE) > 0))
              )
                //[doubt] trocar assert por exceção ?
                assert(0);

              assert(encrypted = (unsigned char*) OPENSSL_malloc(encryptedLen));
              if (EVP_PKEY_encrypt(
                    ctx, 
                    encrypted, 
                    &encryptedLen,
                    secretSession->secret,
                    SECRET_SIZE) <= 0
              )
                //[doubt] trocar assert por exceção ?
                assert(0);
              
              idl_cr::CredentialReset credentialReset;
              credentialReset.login = conn->login()->id;
              credentialReset.session = secretSession->id;
              memcpy(credentialReset.challenge, encrypted, 256);
            
              CORBA::Any any;
              any <<= credentialReset;
              CORBA::OctetSeq_var o = _cdrCodec->encode_value(any);

              IOP::ServiceContext serviceContext;
              serviceContext.context_id = idl_cr::CredentialContextId;
              IOP::ServiceContext::_context_data_seq s(o->length(), o->length(), o->get_buffer(), 0);
              serviceContext.context_data = s;
              ri->add_reply_service_context(serviceContext, true);          

              throw CORBA::NO_PERMISSION(idl_ac::InvalidCredentialCode, CORBA::COMPLETED_NO);            
            }
          } else throw CORBA::NO_PERMISSION(idl_ac::InvalidLoginCode, CORBA::COMPLETED_NO);
        } else throw CORBA::NO_PERMISSION(idl_ac::UnverifiedLoginCode, CORBA::COMPLETED_NO);
      } else {
        IOP::ServiceContext_var sc = ri->get_request_service_context(1234);
        IOP::ServiceContext::_context_data_seq& cd = sc->context_data;
        CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);    
        CORBA::Any_var lany = _cdrCodec->decode_value(
          contextData, openbus::legacy::v1_05::_tc_Credential);
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
          ri->set_slot(_slotId_legacyCallChain, legacyChainAny);
        } else throw CORBA::NO_PERMISSION(idl_ac::NoCredentialCode, CORBA::COMPLETED_NO);
      }
    }
  }
}
