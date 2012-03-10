#include <interceptors/serverInterceptor_impl.h>

#include <iostream>
#include <openssl/sha.h>
#include <openssl/x509.h>

namespace openbus {
  namespace interceptors {
    ServerInterceptor::ServerInterceptor(
      PortableInterceptor::Current* piCurrent, 
      PortableInterceptor::SlotId slotId, 
      IOP::Codec* cdr_codec) 
      : piCurrent(piCurrent) , slotId(slotId), cdr_codec(cdr_codec), connection(0) { }
    
    ServerInterceptor::~ServerInterceptor() { }
    
    void ServerInterceptor::receive_request(PortableInterceptor::ServerRequestInfo* ri)
      throw (CORBA::SystemException, PortableInterceptor::ForwardRequest)
    {
      //[todo] legacy Openbus 1.5
      if (connection && connection->loginInfo()) {
        IOP::ServiceContext_var sc = ri->get_request_service_context(
          openbusidl_credential::CredentialContextId);
        IOP::ServiceContext::_context_data_seq& cd = sc->context_data;
        CORBA::OctetSeq contextData(
          cd.length(),
          cd.length(),
          cd.get_buffer(),
          0);    
        CORBA::Any_var any = cdr_codec->decode_value(
          contextData, 
          openbusidl_credential::_tc_CredentialData);
        openbusidl_credential::CredentialData credential;
        any >>= credential;
        if (!credential.chain.encoded.length()) {
          
        } else {
          // cadeia nula
        }
        //[todo] cache
        openbusidl::IdentifierSeq ids;
        ids.length(1);
        ids[0] = credential.login;
        if (connection->login_registry()->getValidity(ids)) {
          //validate credential
          //[todo] legacy Openbus 1.5
          //validate credential
          Session& session = loginSession[std::string(credential.login)];
          
          size_t slenOperation = strlen(ri->operation());
          int slen = 26 + slenOperation;
          unsigned char* s = new unsigned char[slen];
          s[0] = 2;
          s[1] = 0;
          memcpy((unsigned char*) (s+2), (unsigned char*) session.secret, 16);
          memcpy((unsigned char*) (s+18), (unsigned char*) &credential.ticket, 4);
          unsigned int rid = (unsigned int) ri->request_id();
          memcpy((unsigned char*) (s+22), &rid, 4);
          memcpy((unsigned char*) (s+26), ri->operation(), slenOperation);
          openbusidl::HashValue hash;
          SHA256(s, slen, hash);
          
          //validate ticket
          if (memcmp(hash, credential.hash, 32)) {
            //credential not valid, try to reset credetial session
            std::cout << "credential not valid!" << std::endl;
            openbusidl::OctetSeq_var encodedCallerPubKey;
            openbusidl_access_control::LoginInfo* caller = 
              connection->login_registry()->getLoginInfo(credential.login, encodedCallerPubKey);
            const unsigned char* buf = encodedCallerPubKey->get_buffer();
            EVP_PKEY* callerPubKey = d2i_PUBKEY(
              0, 
              &buf, 
              encodedCallerPubKey->length());
              
            EVP_PKEY_CTX* ctx;
            unsigned char* encrypted;
            size_t encryptedLen;
            if (!((ctx = EVP_PKEY_CTX_new(callerPubKey, 0)) &&
                (EVP_PKEY_encrypt_init(ctx) > 0) &&
                (EVP_PKEY_encrypt(
                  ctx, 
                  0, 
                  &encryptedLen,
                  session.secret,
                  16) > 0))
            )
              //[doubt] trocar assert por exceção ?
              assert(0);

            assert(encrypted = (unsigned char*) OPENSSL_malloc(encryptedLen));
            if (EVP_PKEY_encrypt(
                  ctx, 
                  encrypted, 
                  &encryptedLen,
                  session.secret,
                  16) <= 0
            )
              //[doubt] trocar assert por exceção ?
              assert(0);
              
            openbusidl_credential::CredentialReset credentialReset;
            credentialReset.login = credential.login;
            credentialReset.session = session.id;
            // openbusidl::OctetSeq_var o = new openbusidl::OctetSeq(
            //   encryptedLen,
            //   encryptedLen,
            //   static_cast<CORBA::Octet*> (encrypted),
            //   0);
            // credentialReset.challenge = static_cast<CORBA::Octet*> (encrypted);
            memcpy(credentialReset.challenge, encrypted, 256);
            
            CORBA::Any any;
            any <<= credentialReset;
            CORBA::OctetSeq_var octets;
            octets = cdr_codec->encode_value(any);

            IOP::ServiceContext serviceContext;
            serviceContext.context_id = openbusidl_credential::CredentialContextId;
            IOP::ServiceContext::_context_data_seq seq(
              octets->length(),
              octets->length(),
              octets->get_buffer(),
              0);

            serviceContext.context_data = seq;
            ri->add_reply_service_context(serviceContext, true);          

            throw CORBA::NO_PERMISSION(
              openbusidl_access_control::InvalidCredentialCode, 
              CORBA::COMPLETED_NO);            
          } else {
            std::cout << "credential is valid." << std::endl;
          }
          
        } else {
          throw CORBA::NO_PERMISSION(
            openbusidl_access_control::InvalidLoginCode, 
            CORBA::COMPLETED_NO);
        }
      }
    }

    void ServerInterceptor::send_reply(PortableInterceptor::ServerRequestInfo* ri)
      throw (CORBA::SystemException) 
    {
      openbusidl_credential::CredentialReset credentialReset;
      credentialReset.login = "";
      credentialReset.session = 0;
      // openbusidl::OctetSeq_var o = new openbusidl::OctetSeq(
      //   encryptedLen,
      //   encryptedLen,
      //   static_cast<CORBA::Octet*> (encrypted),
      //   0);
      // credentialReset.challenge = static_cast<CORBA::Octet*> (encrypted);
//      memcpy(credentialReset.challenge, encrypted, 256);
      
      CORBA::Any any;
      any <<= credentialReset;
      CORBA::OctetSeq_var octets;
      octets = cdr_codec->encode_value(any);

      IOP::ServiceContext serviceContext;
      serviceContext.context_id = openbusidl_credential::CredentialContextId;
      IOP::ServiceContext::_context_data_seq seq(
        octets->length(),
        octets->length(),
        octets->get_buffer(),
        0);

      serviceContext.context_data = seq;
      ri->add_reply_service_context(serviceContext, true);          
    }
    
    void ServerInterceptor::addConnection(Connection* connection) {
      this->connection = connection;
    }
    
    void ServerInterceptor::removeConnection(Connection* connection) {
      this->connection = 0;
    }  
  }
}
