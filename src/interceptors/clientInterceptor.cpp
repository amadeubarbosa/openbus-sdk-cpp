#include <interceptors/clientInterceptor_impl.h>
#include <legacy/stubs/credential_v1_05.h>

#include <openssl/sha.h>
#include <sstream>

namespace openbus {
  namespace interceptors {    
    ClientInterceptor::ClientInterceptor(
      PortableInterceptor::SlotId slotId_joinedCallChain,
      IOP::Codec* cdr_codec) 
      : allowRequestWithoutCredential(false), _cdrCodec(cdr_codec), _conn(0), _manager(0),
      _slotId_joinedCallChain(slotId_joinedCallChain) 
    { }
    
    ClientInterceptor::~ClientInterceptor() { }
    
    void ClientInterceptor::send_request(PortableInterceptor::ClientRequestInfo* ri)
      throw (CORBA::Exception)
    {
      const char* operation = ri->operation();
      std::cout << "send_request:" << operation << std::endl;
      if (!allowRequestWithoutCredential) {
        Connection* conn;
        if (_manager) {
          if (_manager->_requestIdConnection.find(ri->request_id()) != 
          _manager->_requestIdConnection.end()) 
            conn = _manager->_requestIdConnection[ri->request_id()];
          else
            conn = _manager->getDefaultConnection();
          if (!conn) throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
        } else conn = _conn;

        if (conn && conn->login()) {
          CallerChain* callerChain = 0;
          IOP::ServiceContext serviceContext;
          serviceContext.context_id = idl_cr::CredentialContextId;
          idl_cr::CredentialData credential;
          credential.bus = CORBA::string_dup(conn->busid());
          credential.login = CORBA::string_dup(conn->login()->id);
          
          idl::HashValue profileDataHash;
          ::IOP::TaggedProfile::_profile_data_seq profile = ri->effective_profile()->profile_data;
          SHA256(profile.get_buffer(), profile.length(), profileDataHash);
          std::string sprofileDataHash((const char*) profileDataHash, 32);
        
          if (_profileSecretSession.find(sprofileDataHash) != _profileSecretSession.end()) {
            SecretSession* session = _profileSecretSession[sprofileDataHash];
            credential.session = session->id;
            session->ticket++;
            credential.ticket = session->ticket;
            int slen = 22 + strlen(operation);
            unsigned char* s = new unsigned char[slen];
            s[0] = 2;
            s[1] = 0;
            memcpy(s+2, session->secret, 16);
            memcpy(s+18, &credential.ticket, 4);
            memcpy(s+22, operation, strlen(operation));
            SHA256(s, slen, credential.hash);
            
            callerChain = conn->getJoinedChain();
            if (strcmp(conn->busid(), session->remoteid)) {
              CORBA::Object_var picRef = conn->orb()->resolve_initial_references("PICurrent");
              assert(!CORBA::is_nil(picRef));
              PortableInterceptor::Current_var p = PortableInterceptor::Current::_narrow(picRef);
              CORBA::Any_var signedCallChainAny = p->get_slot(_slotId_joinedCallChain);
              credential.chain = *conn->access_control()->signChainFor(session->remoteid);
            } else {
              if (callerChain) credential.chain = *callerChain->signedCallChain();
              else memset(credential.chain.signature, '\0', 256);
            }
          } else {
            credential.ticket = 0;
            credential.session = 0;
            memset(credential.hash, '\0', 32);
            memset(credential.chain.signature, '\0', 256);
          }
          CORBA::Any any;
          any <<= credential;
          CORBA::OctetSeq_var o = _cdrCodec->encode_value(any);
          IOP::ServiceContext::_context_data_seq s(o->length(), o->length(), o->get_buffer(), 0);
          serviceContext.context_data = s;
          ri->add_request_service_context(serviceContext, true);
          
          IOP::ServiceContext legacyContext;
          legacyContext.context_id = 1234;
          legacy::v1_05::Credential legacyCredential;
          legacyCredential.identifier = conn->login()->id;
          legacyCredential.owner = conn->login()->entity;
          if (callerChain && (callerChain->callers.length() > 1))
            legacyCredential.delegate = CORBA::string_dup(callerChain->callers[0].entity);
          else legacyCredential.delegate = "";
          CORBA::Any lany;
          lany <<= legacyCredential;
          o = _cdrCodec->encode_value(lany);
          IOP::ServiceContext::_context_data_seq ls(o->length(), o->length(), o->get_buffer(), 0);
          legacyContext.context_data = ls;
          ri->add_request_service_context(legacyContext, true);
        } else throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);          
      }
    }

    void ClientInterceptor::receive_exception(PortableInterceptor::ClientRequestInfo* ri)
      throw (CORBA::Exception, PortableInterceptor::ForwardRequest)
    {
      std::cout << "receive_exception:" << ri->received_exception_id() << std::endl;

      Connection* conn;
      if (_manager) {
        conn = _manager->getDefaultConnection();
        if (!conn) throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
      } else conn = _conn;

      if (!strcmp(ri->received_exception_id(), "IDL:omg.org/CORBA/NO_PERMISSION:1.0")) {
        CORBA::SystemException* ex = CORBA::SystemException::_decode(*ri->received_exception());
        if (ex->completed() == CORBA::COMPLETED_NO) {
          if (ex->minor() == idl_ac::InvalidCredentialCode) {
            std::cout << "creating credential session." << std::endl;
            IOP::ServiceContext_var sctx;
            if (sctx = ri->get_request_service_context(idl_cr::CredentialContextId)) {
              CORBA::OctetSeq o(
                sctx->context_data.length(), 
                sctx->context_data.length(), 
                sctx->context_data.get_buffer());
              //[doubt] pegar exceção FormatMismatch ?
              CORBA::Any_var any = _cdrCodec->decode_value(o, idl_cr::_tc_CredentialReset);
              idl_cr::CredentialReset credentialReset;
              any >>= credentialReset;
            
              unsigned char* secret;
              size_t secretLen;
              EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(conn->prvKey(), 0);
              if (!(ctx && 
                  (EVP_PKEY_decrypt_init(ctx) > 0) &&
                  (EVP_PKEY_decrypt(
                    ctx,
                    0,
                    &secretLen,
                    credentialReset.challenge,
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
              IOP::TaggedProfile::_profile_data_seq profile = ri->effective_profile()->profile_data;
              SHA256(profile.get_buffer(), profile.length(), profileDataHash);
              std::string sprofileDataHash((const char*) profileDataHash, 32);
              SecretSession* session = new SecretSession();
              session->id = credentialReset.session;
              session->remoteid  = CORBA::string_dup(credentialReset.login);
              session->secret = secret;
              session->ticket = 0;
              _profileSecretSession[sprofileDataHash] = session;
              throw PortableInterceptor::ForwardRequest(ri->target(), false);
            }
          } else if (ex->minor() == idl_ac::NoCredentialCode) {
            throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, CORBA::COMPLETED_NO);
          } else if (ex->minor() == idl_ac::InvalidLoginCode) {
            //[todo] tratar valor de retorno
            if (conn && conn->onInvalidLoginCallback())
              (conn->onInvalidLoginCallback())(conn, conn->login());
            conn->_logout(true);
          }
        }
      }
    }
  }
}
