#include <interceptors/clientInterceptor_impl.h>

#include <openssl/sha.h>
#include <sstream>

namespace openbus {
  namespace interceptors {    
    ClientInterceptor::ClientInterceptor(
      PortableInterceptor::SlotId slotId_joinedCallChain,
      IOP::Codec* cdr_codec) 
      : allowRequestWithoutCredential(false),
        _cdrCodec(cdr_codec), 
        _conn(0),
        _slotId_joinedCallChain(slotId_joinedCallChain) 
    { }
    
    ClientInterceptor::~ClientInterceptor() { }
    
    void ClientInterceptor::send_request(PortableInterceptor::ClientRequestInfo* ri)
    throw (CORBA::Exception)
    {
      const char* operation = ri->operation();
      std::cout << "send_request:" << operation << std::endl;
      if (!allowRequestWithoutCredential) {
        if (_conn && _conn->login()) {
          IOP::ServiceContext serviceContext;
          serviceContext.context_id = idl_cr::CredentialContextId;
          idl_cr::CredentialData credential;
          credential.bus = CORBA::string_dup(_conn->busid());
          credential.login = CORBA::string_dup(_conn->login()->id);
          
          idl::HashValue profileDataHash;
          SHA256(
            ri->effective_profile()->profile_data.get_buffer(),
            ri->effective_profile()->profile_data.length(), 
            profileDataHash);
          std::string sprofileDataHash((const char*) profileDataHash, 32);

          if (_profile2login.find(sprofileDataHash) != _profile2login.end()) {
            std::string login = _profile2login[sprofileDataHash];
            CredentialSession* credSession = _login2credsession[login];
            credential.session = credSession->id;
            credSession->ticket++;
            credential.ticket = credSession->ticket;
            CORBA::Long objectKeyLen;
            const CORBA::Octet* objectKeyOct = 
              ri->target()->_ior()->get_profile(0)->objectkey(objectKeyLen);
            char* objectKey = new char[objectKeyLen+1];
            memcpy(objectKey, objectKeyOct, objectKeyLen);
            objectKey[objectKeyLen] = '\0';
            int slen = 22 + strlen(operation);
            unsigned char* s = new unsigned char[slen];
            s[0] = 2;
            s[1] = 0;
            memcpy((unsigned char*) (s+2), (unsigned char*) credSession->secret, 16);
            memcpy((unsigned char*) (s+18), (unsigned char*) &credential.ticket, 4);
            memcpy((unsigned char*) (s+22), operation, strlen(operation));
            SHA256(s, slen, credential.hash);
            
            const char* clogin = login.c_str();
            if (strcmp(_conn->busid(), clogin)) {
              // credential.chain = *_conn->access_control()->signChainFor(clogin);
              CORBA::Object_var init_ref = _conn->orb()->resolve_initial_references(
                "PICurrent");
              assert(!CORBA::is_nil(init_ref));
              PortableInterceptor::Current_var piCurrent = 
                PortableInterceptor::Current::_narrow(init_ref);
              CORBA::Any_var signedCallChainAny = piCurrent->get_slot(_slotId_joinedCallChain);
              idl_ac::SignedCallChain signedCallChain;
              if (*signedCallChainAny >>= signedCallChain)
                credential.chain = signedCallChain;
              else 
                credential.chain = *_conn->access_control()->signChainFor(clogin);
            } else {
              memset(credential.chain.signature, '\0', 256);              
            }
          } else {
            credential.ticket = 0;
            credential.session = 0;
            memset(credential.hash, '\0', 32);
            memset(credential.chain.signature, '\0', 256);
          }
          
          CORBA::Any any;
          any <<= credential;
          CORBA::OctetSeq_var octets;
          octets = _cdrCodec->encode_value(any);
          
          IOP::ServiceContext::_context_data_seq seq(
            octets->length(),
            octets->length(),
            octets->get_buffer(),
            0);
      
          serviceContext.context_data = seq;
          ri->add_request_service_context(serviceContext, true);          
        } else
          throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);          
      }
    }

    void ClientInterceptor::receive_exception(PortableInterceptor::ClientRequestInfo* ri)
    throw (CORBA::Exception, PortableInterceptor::ForwardRequest)
    {
      std::cout << "receive_exception:" << ri->received_exception_id() << std::endl;
      if (!strcmp(ri->received_exception_id(), "IDL:omg.org/CORBA/NO_PERMISSION:1.0")) {
        CORBA::SystemException* ex = CORBA::SystemException::_decode(*ri->received_exception());
        if (ex->completed() == CORBA::COMPLETED_NO) {
          if (ex->minor() == idl_ac::InvalidCredentialCode) {
            std::cout << "creating credential session." << std::endl;
            if (IOP::ServiceContext_var sctx = 
              ri->get_request_service_context(idl_cr::CredentialContextId)) {
              CORBA::OctetSeq o(
                sctx->context_data.length(), 
                sctx->context_data.length(), 
                sctx->context_data.get_buffer());
              //[doubt] pegar exceção FormatMismatch ?
              CORBA::Any_var any = 
                _cdrCodec->decode_value(o, idl_cr::_tc_CredentialReset);
              idl_cr::CredentialReset credentialReset;
              any >>= credentialReset;
            
              unsigned char* secret;
              size_t secretLen;
              EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(_conn->prvKey(), 0);
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

              idl::HashValue profileDataHash;
              SHA256(
                ri->effective_profile()->profile_data.get_buffer(),
                ri->effective_profile()->profile_data.length(), 
                profileDataHash);
              std::string sprofileDataHash((const char*) profileDataHash, 32);
              std::string remoteid(credentialReset.login);
              _profile2login[sprofileDataHash] = remoteid;
            
              if (_login2credsession.find(remoteid) == _login2credsession.end()) {
                CredentialSession* credSession = new CredentialSession();
                credSession->id = credentialReset.session;
                credSession->remoteid  = CORBA::string_dup(credentialReset.login);
                credSession->secret = secret;
                credSession->ticket = 0;
                _login2credsession[remoteid] = credSession;
              }
              throw PortableInterceptor::ForwardRequest(ri->target(), false);
            }
          } else if (ex->minor() == idl_ac::InvalidLoginCode)
            //[todo] tratar valor de retorno
            (_conn->onInvalidLoginCallback())(_conn, _conn->login()->id);
        }
      }
    }
  }
}
