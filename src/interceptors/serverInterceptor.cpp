#include <interceptors/serverInterceptor_impl.h>
#include <connection_impl.h>

#include <iostream>
#include <openssl/sha.h>
#include <openssl/x509.h>

namespace openbus {
  namespace interceptors {
    ServerInterceptor::ServerInterceptor(
      PortableInterceptor::Current* piCurrent, 
      PortableInterceptor::SlotId slotId_joinedCallChain,
      PortableInterceptor::SlotId slotId_signedCallChain, 
      PortableInterceptor::SlotId slotId_busid, 
      IOP::Codec* cdr_codec) 
      : piCurrent(piCurrent), 
        _slotId_joinedCallChain(slotId_joinedCallChain), 
        _slotId_signedCallChain(slotId_signedCallChain), 
        _slotId_busid(slotId_busid), 
        cdr_codec(cdr_codec), 
        connection(0) { }
    
    ServerInterceptor::~ServerInterceptor() { }
    
    void ServerInterceptor::receive_request_service_contexts(
      PortableInterceptor::ServerRequestInfo* ri)
      throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
      std::cout << "receive_request_service_contexts: " << ri->operation() << std::endl;
      //[todo] legacy Openbus 1.5
      if (connection && connection->loginInfo()) {
        IOP::ServiceContext_var sc = ri->get_request_service_context(idl_cr::CredentialContextId);
        IOP::ServiceContext::_context_data_seq& cd = sc->context_data;
        CORBA::OctetSeq contextData(
          cd.length(),
          cd.length(),
          cd.get_buffer(),
          0);    
        CORBA::Any_var any = cdr_codec->decode_value(
          contextData, 
          idl_cr::_tc_CredentialData);
        idl_cr::CredentialData credential;
        any >>= credential;
        
        //[todo] legacy Openbus 1.5
        Login* caller = connection->_loginCache->validateLogin(credential.login);
        if (caller) {
          CredentialSession* session = 0;
          idl::HashValue hash;
          if (sessionIdCredentialSession.find(credential.session) != sessionIdCredentialSession.end()) {
            session = sessionIdCredentialSession[credential.session];
            size_t slenOperation = strlen(ri->operation());
            int slen = 22 + slenOperation;
            unsigned char* s = new unsigned char[slen];
            s[0] = 2;
            s[1] = 0;
            memcpy((unsigned char*) (s+2), (unsigned char*) session->secret, 16);
            memcpy((unsigned char*) (s+18), (unsigned char*) &credential.ticket, 4);
            memcpy((unsigned char*) (s+22), ri->operation(), slenOperation);
            SHA256(s, slen, hash);
          }
          
          if (session &&
              !memcmp(hash, credential.hash, 32) && 
              tickets_check(&session->ticketsHistory, credential.ticket)) 
          {
            std::cout << "credential is valid." << std::endl;
            bool invalidChain = false;
            if (credential.chain.encoded.length()) {
              // [todo]: legacy
              idl::HashValue hash;
              SHA256(credential.chain.encoded.get_buffer(), credential.chain.encoded.length(), hash);

              EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(connection->busKey(), 0);
              assert(ctx);
              assert(EVP_PKEY_verify_init(ctx));
              if (EVP_PKEY_verify(ctx, credential.chain.signature, 256, hash, 32) != 1) {
                invalidChain = true;
              } else {
                CORBA::Any_var callChainAny = cdr_codec->decode_value(
                  credential.chain.encoded,
                  idl_ac::_tc_CallChain);
                idl_ac::CallChain callChain;
                callChainAny >>= callChain;
                if (strcmp(callChain.target, connection->loginInfo()->id) ||
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
            } else {
              invalidChain = true;
            }

            if (invalidChain)
              throw CORBA::NO_PERMISSION(
                idl_ac::InvalidChainCode, 
                CORBA::COMPLETED_NO);
          } else {
            //credential not valid, try to reset credetial session
            std::cout << "credential not valid, try to reset credetial session" << std::endl;
            CORBA::ULong newSessionId = sessionIdCredentialSession.size() + 1;
            CredentialSession* credentialSession = new CredentialSession(newSessionId);
            sessionIdCredentialSession[newSessionId] = credentialSession;

            EVP_PKEY_CTX* ctx;
            unsigned char* encrypted;
            size_t encryptedLen;
            if (!((ctx = EVP_PKEY_CTX_new(caller->key, 0)) &&
                (EVP_PKEY_encrypt_init(ctx) > 0) &&
                (EVP_PKEY_encrypt(
                  ctx, 
                  0, 
                  &encryptedLen,
                  credentialSession->secret,
                  16) > 0))
            )
              //[doubt] trocar assert por exceção ?
              assert(0);

            assert(encrypted = (unsigned char*) OPENSSL_malloc(encryptedLen));
            if (EVP_PKEY_encrypt(
                  ctx, 
                  encrypted, 
                  &encryptedLen,
                  credentialSession->secret,
                  16) <= 0
            )
              //[doubt] trocar assert por exceção ?
              assert(0);
              
            idl_cr::CredentialReset credentialReset;
            credentialReset.login = connection->loginInfo()->id;
            credentialReset.session = credentialSession->id;
            memcpy(credentialReset.challenge, encrypted, 256);
            
            CORBA::Any any;
            any <<= credentialReset;
            CORBA::OctetSeq_var octets;
            octets = cdr_codec->encode_value(any);

            IOP::ServiceContext serviceContext;
            serviceContext.context_id = idl_cr::CredentialContextId;
            IOP::ServiceContext::_context_data_seq seq(
              octets->length(),
              octets->length(),
              octets->get_buffer(),
              0);

            serviceContext.context_data = seq;
            ri->add_reply_service_context(serviceContext, true);          

            throw CORBA::NO_PERMISSION(
              idl_ac::InvalidCredentialCode, 
              CORBA::COMPLETED_NO);            
          }
          
        } else {
          throw CORBA::NO_PERMISSION(
            idl_ac::InvalidLoginCode, 
            CORBA::COMPLETED_NO);
        }
      }
    }

    void ServerInterceptor::addConnection(Connection* connection) {
      this->connection = connection;
    }
    
    void ServerInterceptor::removeConnection(Connection* connection) {
      this->connection = 0;
    }  
  }
}
