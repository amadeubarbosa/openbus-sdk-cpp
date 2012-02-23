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
      std::cout << operation << std::endl;
      if (!allowRequestWithoutCredential) {
        if (connection && connection->loginInfo()) {
          IOP::ServiceContext serviceContext;
          serviceContext.context_id = openbusidl_credential::CredentialContextId;
          openbusidl_credential::CredentialData credential;
          credential.bus = CORBA::string_dup(connection->busId());
          credential.login = CORBA::string_dup(connection->loginInfo()->id);
          memset(credential.chain.signature, '\0', 256);
          std::string busid(credential.bus);
          openbusidl::HashValue profileDataHash;
          SHA256(
            ri->effective_profile()->profile_data.get_buffer(),
            ri->effective_profile()->profile_data.length(), 
            profileDataHash);
          std::string sprofileDataHash((const char*) profileDataHash);
          if (profile2busid.find(sprofileDataHash) == profile2busid.end()) {      
            credential.ticket = 0;
            memset(credential.hash, '\0', 32);
          } else {
            CredentialSession* credSession = profile2busid[sprofileDataHash];
            if (credSession->ticket > 0) {
              ++credSession->ticket;
              credential.ticket = credSession->ticket;
            }
            CORBA::Long objectKeyLen;
            const CORBA::Octet* objectKeyOct = 
              ri->target()->_ior()->get_profile(0)->objectkey(objectKeyLen);
            char* objectKey = new char[objectKeyLen+1];
            memcpy(objectKey, objectKeyOct, objectKeyLen);
            objectKey[objectKeyLen] = '\0';
            int slen = 26 + strlen(objectKey) + strlen(operation);
            unsigned char* s = new unsigned char[slen];
            s[0] = 2;
            s[1] = 0;
            memcpy((unsigned char*) (s+2), (unsigned char*) credSession->secret, 16);
            memcpy((unsigned char*) (s+18), (unsigned char*) &credential.ticket, 4);
            unsigned int rid = (unsigned int) ri->request_id();
            memcpy((unsigned char*) (s+22), &rid, 4);
            memcpy((unsigned char*) (s+26), objectKey, strlen(objectKey));
            memcpy((unsigned char*) (s+26+strlen(objectKey)), operation, strlen(operation));
            SHA256(s, slen, credential.hash);
          }
      
          CORBA::Any any;
          any <<= credential;
          CORBA::OctetSeq_var octets;
          octets = cdr_codec->encode_value(any);
          
          unsigned char* data = new unsigned char[octets->length()+2];
          data[0] = 2;
          data[1] = 0;
          memcpy(&data[2], octets->get_buffer(), octets->length());
          
          IOP::ServiceContext::_context_data_seq seq(
            octets->length()+2,
            octets->length()+2,
            data,
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
      std::cout << "[exception]" << ri->received_exception_id() << std::endl;
      if (!strcmp(ri->received_exception_id(), "IDL:omg.org/CORBA/NO_PERMISSION:1.0")) {
        CORBA::SystemException* ex = CORBA::SystemException::_decode(*ri->received_exception());
        if ((ex->completed() == CORBA::COMPLETED_NO) && 
            (ex->minor() == openbusidl_access_control::InvalidCredentialCode)) 
        {
          std::cout << "got invalid credential exception" << std::endl;
          if (IOP::ServiceContext_var sctx = 
            ri->get_request_service_context(openbusidl_credential::CredentialContextId)) {
            CORBA::OctetSeq o(
              sctx->context_data.length()-2, 
              sctx->context_data.length()-2, 
              sctx->context_data.get_buffer()+2);
            //[doubt] pegar exce��o FormatMismatch ?
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
              //[doubt] trocar assert por exce��o ?
              assert(0);
      
            assert(secret = (unsigned char*) OPENSSL_malloc(secretLen));
            if (EVP_PKEY_decrypt(
                  ctx,
                  secret,
                  &secretLen,
                  credentialReset.challenge,
                  256) <= 0
            )
              //[doubt] trocar assert por exce��o ?
              assert(0);
            secret[secretLen] = '\0';
            openbusidl::HashValue profileDataHash;
            SHA256(
              ri->effective_profile()->profile_data.get_buffer(),
              ri->effective_profile()->profile_data.length(), 
              profileDataHash);
            std::string sprofileDataHash((const char*) profileDataHash);
            if (profile2busid.find(sprofileDataHash) == profile2busid.end()) {
              CredentialSession* credSession = new CredentialSession();
              credSession->remoteid  = CORBA::string_dup(credentialReset.login);
              credSession->secret = secret;
              credSession->ticket = 0;
              profile2busid[sprofileDataHash] = credSession;
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
