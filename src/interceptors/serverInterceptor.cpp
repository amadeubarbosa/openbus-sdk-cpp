#include <interceptors/serverInterceptor_impl.h>
#include <iostream>

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
        if (cd[0] == 2 && cd[1] == 0) {
          CORBA::OctetSeq contextData(
            cd.length()-2,
            cd.length()-2,
            cd.get_buffer()+2,
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
            Session* session = loginSession[std::string(credential.login)];
            
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
            memcpy((unsigned char*) (s+2), (unsigned char*) session->secret, 16);
            memcpy((unsigned char*) (s+18), (unsigned char*) &credential.ticket, 4);
            unsigned int rid = (unsigned int) ri->request_id();
            memcpy((unsigned char*) (s+22), &rid, 4);
            memcpy((unsigned char*) (s+26), objectKey, strlen(objectKey));
            memcpy((unsigned char*) (s+26+strlen(objectKey)), operation, strlen(operation));
            HashValue hash;
            SHA256(s, slen, hash);
            
            if (memcmp(hash, credential.hash, 32)) {
              
            } else {
              //credential not valid, try to reset credetial session
              std::cout << "credential not valid!" << std::endl;
            }
            
            //validate ticket
          } else {
            throw CORBA::NO_PERMISSION(
              openbusidl_access_control::InvalidLoginCode, 
              CORBA::COMPLETED_NO);
          }
        }
      }
    }

    void ServerInterceptor::send_reply(PortableInterceptor::ServerRequestInfo* ri)
      throw (CORBA::SystemException) 
    {
    }
    
    void ServerInterceptor::addConnection(Connection* connection) {
      this->connection = connection;
    }
    
    void ServerInterceptor::removeConnection(Connection* connection) {
      this->connection = 0;
    }  
  }
}
