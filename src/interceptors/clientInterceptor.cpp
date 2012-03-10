#include <interceptors/clientInterceptor_impl.h>

#include <openssl/sha.h>
#include <sstream>

namespace openbus {
  namespace interceptors {    
    ClientInterceptor::ClientInterceptor(IOP::Codec* cdr_codec) 
      : allowRequestWithoutCredential(false), cdr_codec(cdr_codec), connection(0) { }
    
    ClientInterceptor::~ClientInterceptor() { }
    
    void ClientInterceptor::send_request(PortableInterceptor::ClientRequestInfo* ri)
      throw (CORBA::Exception)
    {
      const char* operation = ri->operation();
      std::cout << "send_request:" << operation << std::endl;
      if (!allowRequestWithoutCredential) {
        if (connection && connection->loginInfo()) {
          IOP::ServiceContext serviceContext;
          serviceContext.context_id = openbusidl_credential::CredentialContextId;
          openbusidl_credential::CredentialData credential;
          credential.bus = CORBA::string_dup(connection->busId());
          credential.login = CORBA::string_dup(connection->loginInfo()->id);
          
          openbusidl::HashValue profileDataHash;
          SHA256(
            ri->effective_profile()->profile_data.get_buffer(),
            ri->effective_profile()->profile_data.length(), 
            profileDataHash);
          std::string sprofileDataHash((const char*) profileDataHash, 32);

          if (profile2login.find(sprofileDataHash) == profile2login.end()) {
            credential.ticket = 0;
            credential.session = 0;
            memset(credential.hash, '\0', 32);
            memset(credential.chain.signature, '\0', 256);
          } else {
            std::string login = profile2login[sprofileDataHash];
            CredentialSession* credSession = login2credsession[login];
            credential.session = credSession->id;
            credential.ticket = ++credSession->ticket;
            CORBA::Long objectKeyLen;
            const CORBA::Octet* objectKeyOct = 
              ri->target()->_ior()->get_profile(0)->objectkey(objectKeyLen);
            char* objectKey = new char[objectKeyLen+1];
            memcpy(objectKey, objectKeyOct, objectKeyLen);
            objectKey[objectKeyLen] = '\0';
            int slen = 26 + strlen(operation);
            unsigned char* s = new unsigned char[slen];
            s[0] = 2;
            s[1] = 0;
            memcpy((unsigned char*) (s+2), (unsigned char*) credSession->secret, 16);
            memcpy((unsigned char*) (s+18), (unsigned char*) &credential.ticket, 4);
            unsigned int rid = (unsigned int) ri->request_id();
            memcpy((unsigned char*) (s+22), &rid, 4);
            memcpy((unsigned char*) (s+26), operation, strlen(operation));
            SHA256(s, slen, credential.hash);
            
            const char* clogin = login.c_str();
            if (strcmp(connection->busId(), clogin)) {
              credential.chain = *connection->access_control()->signChainFor(clogin);
            } else {
              memset(credential.chain.signature, '\0', 256);              
            }
          }
          
          CORBA::Any any;
          any <<= credential;
          CORBA::OctetSeq_var octets;
          octets = cdr_codec->encode_value(any);
          
          IOP::ServiceContext::_context_data_seq seq(
            octets->length(),
            octets->length(),
            octets->get_buffer(),
            0);
      
          serviceContext.context_data = seq;
          ri->add_request_service_context(serviceContext, true);          
        } else
          throw CORBA::NO_PERMISSION(openbusidl_access_control::NoLoginCode, CORBA::COMPLETED_NO);          
      }
    }

    void ClientInterceptor::receive_exception(PortableInterceptor::ClientRequestInfo* ri)
      throw (CORBA::Exception, PortableInterceptor::ForwardRequest)
    {
      std::cout << "receive_exception:" << ri->received_exception_id() << std::endl;
      if (!strcmp(ri->received_exception_id(), "IDL:omg.org/CORBA/NO_PERMISSION:1.0")) {
        CORBA::SystemException* ex = CORBA::SystemException::_decode(*ri->received_exception());
        if ((ex->completed() == CORBA::COMPLETED_NO) && 
            (ex->minor() == openbusidl_access_control::InvalidCredentialCode)) 
        {
          std::cout << "creating credential session..." << std::endl;
          if (IOP::ServiceContext_var sctx = 
            ri->get_request_service_context(openbusidl_credential::CredentialContextId)) {
            CORBA::OctetSeq o(
              sctx->context_data.length(), 
              sctx->context_data.length(), 
              sctx->context_data.get_buffer());
            //[doubt] pegar exceção FormatMismatch ?
            CORBA::Any_var any = 
              cdr_codec->decode_value(o, openbusidl_credential::_tc_CredentialReset);
            openbusidl_credential::CredentialReset credentialReset;
            any >>= credentialReset;
            
            unsigned char* secret;
            size_t secretLen;
            EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(connection->prvKey(), 0);
            if (!(ctx && 
                (EVP_PKEY_decrypt_init(ctx) > 0) &&
                (EVP_PKEY_decrypt(
                  ctx,
                  0,
                  &secretLen,
                  (unsigned char*) credentialReset.challenge,
                  256) > 0))
            )
              //[doubt] trocar assert por exceção ?
              assert(0);
      
            assert(secret = (unsigned char*) OPENSSL_malloc(secretLen));
            if (EVP_PKEY_decrypt(
                  ctx,
                  secret,
                  &secretLen,
                  credentialReset.challenge,
                  256) <= 0
            )
              //[doubt] trocar assert por exceção ?
              assert(0);
            secret[secretLen] = '\0';

            openbusidl::HashValue profileDataHash;
            SHA256(
              ri->effective_profile()->profile_data.get_buffer(),
              ri->effective_profile()->profile_data.length(), 
              profileDataHash);
            std::string sprofileDataHash((const char*) profileDataHash, 32);
            std::string remoteid(credentialReset.login);
            profile2login[sprofileDataHash] = remoteid;
            
            if (login2credsession.find(remoteid) == login2credsession.end()) {
              CredentialSession* credSession = new CredentialSession();
              credSession->id = credentialReset.session;
              credSession->remoteid  = CORBA::string_dup(credentialReset.login);
              credSession->secret = secret;
              credSession->ticket = 0;
              login2credsession[remoteid] = credSession;
            }
            throw PortableInterceptor::ForwardRequest(ri->target(), false);
          }
        }
      }
    }
    
    void ClientInterceptor::addConnection(Connection* connection) {
      this->connection = connection;
    }
    
    void ClientInterceptor::removeConnection(Connection* connection) {
      this->connection = 0;
    }  
  }
}
