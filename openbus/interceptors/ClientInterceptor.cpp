/*
** interceptors/ClientInterceptor.cpp
*/

#include "ClientInterceptor.h"

#include "../../openbus.h"

#ifndef OPENBUS_MICO
  #include <it_ts/thread.h>
#endif

using namespace tecgraf::openbus::core::v1_05;

namespace openbus {
  namespace interceptors {
    access_control_service::Credential* ClientInterceptor::credential = 0;

    ClientInterceptor::ClientInterceptor(IOP::Codec_ptr pcdr_codec)  
    {
      Openbus::logger->log(INFO, "ClientInterceptor::ClientInterceptor() BEGIN");
      Openbus::logger->indent();
      cdr_codec = pcdr_codec;
      faultToleranceManager = FaultToleranceManager::getInstance();
      if (Openbus::FTConfigFilename) {
        faultToleranceManager->loadConfig(Openbus::FTConfigFilename);
      }
      Openbus::logger->dedent(INFO, "ClientInterceptor::ClientInterceptor() END");
    }

    ClientInterceptor::~ClientInterceptor() { }

    void ClientInterceptor::send_request(ClientRequestInfo_ptr ri) 
      throw(
        CORBA::SystemException,
        PortableInterceptor::ForwardRequest)
    {
      Openbus::logger->log(INFO, "ClientInterceptor::send_request() BEGIN");
      Openbus::logger->indent();
      stringstream msg;
      char * operation = ri->operation();
      msg << "Method: " << operation;
      Openbus::logger->log(INFO, msg.str());
      free(operation);
      if (credential) {
        stringstream msg;
        Openbus::logger->log(INFO, "Credential:");
        Openbus::logger->indent();
        msg << "[identifier]: " << credential->identifier;
        Openbus::logger->log(INFO, msg.str());
        msg.str("");
        msg << "[owner]: " << credential->owner;
        Openbus::logger->log(INFO, msg.str());
        msg.str("");
        msg << "[delegate]: " << credential->delegate;
        Openbus::logger->log(INFO, msg.str());
        Openbus::logger->dedent();
        IOP::ServiceContext sc;
        sc.context_id = 1234;

        CORBA::Any any;
        any <<= *credential;
        CORBA::OctetSeq_var octets;
        octets = cdr_codec->encode_value(any);
        IOP::ServiceContext::_context_data_seq seq(
          octets->length(),
          octets->length(),
          octets->get_buffer(),
          0);
        sc.context_data = seq;

        CORBA::ULong z;
        stringstream contextData;
        contextData << "Context data: ";
        for ( z = 0; z < sc.context_data.length(); z++ ) {
          contextData <<  (unsigned) sc.context_data[ z ] << " ";
        }
        Openbus::logger->log(INFO, contextData.str());

        ri->add_request_service_context(sc, true);
      }
      Openbus::logger->dedent(INFO, "ClientInterceptor::send_request() END");
    }

    char* ClientInterceptor::name() 
      throw(CORBA::SystemException)
    {
      return CORBA::string_dup("AccessControl");
    }
    void ClientInterceptor::send_poll( ClientRequestInfo_ptr ri ) 
      throw(CORBA::SystemException) 
    {}
    void ClientInterceptor::receive_reply( ClientRequestInfo_ptr ri ) 
      throw(CORBA::SystemException) 
    {}

    void ClientInterceptor::receive_exception( ClientRequestInfo_ptr ri ) 
      throw(
        CORBA::SystemException,
        PortableInterceptor::ForwardRequest)
    {
      Openbus::logger->log(INFO, "ClientInterceptor::receive_exception() BEGIN");
      Openbus::logger->indent();
      stringstream out;
      const char* received_exception_id = ri->received_exception_id();
      PortableInterceptor::ReplyStatus reply_status = ri->reply_status();
      out << "Exception: " << received_exception_id;
      Openbus::logger->log(INFO, out.str());
      out.str(" ");
      out << "Reply Status: " << reply_status;
      Openbus::logger->log(INFO, out.str());
      out.str(" ");
      if (reply_status == 1 
          && 
          strcmp(received_exception_id, "IDL:omg.org/CORBA/NO_PERMISSION:1.0")) 
      {
        Openbus::logger->log(ERROR, "TRATANDO EXCEÇÂO RECEBIDA DO SERVIDOR!");
        const char* operation = ri->operation();
        out << "Método: " << operation;
        Openbus::logger->log(INFO, out.str());
        out.str(" ");
        
        Openbus* bus = Openbus::getInstance();
      #ifdef OPENBUS_MICO
        CORBA::Long objectKeyLen;
        const CORBA::Octet* objectKeyOct = ri->target()->_ior()->get_profile(0) \
          ->objectkey(objectKeyLen);
        char* objectKey = new char[objectKeyLen+1];
        memcpy(objectKey, objectKeyOct, objectKeyLen);
        objectKey[objectKeyLen] = '\0';
      #else
        char* objectKey = "(null)";
        lua_getglobal(Openbus::luaState, "IOR");
        lua_getfield(Openbus::luaState, -1, "IIOPProfileGetObjectKey");
        lua_pushstring(Openbus::luaState, 
          bus->getORB()->object_to_string(ri->target()));
        if (lua_pcall(Openbus::luaState, 1, 1, 0)) {
          const char* errmsg = lua_tostring(Openbus::luaState, -1);
          lua_pop(Openbus::luaState, 1);
          Openbus::logger->log(ERROR, errmsg);
        } else {
          objectKey = (char*) lua_tostring(Openbus::luaState, -1);
        }
      #endif
        
        out << "ObjectKey: " << objectKey;
        Openbus::logger->log(INFO, out.str());
        if (!strcmp(objectKey, "LP_v1_05")
            || !strcmp(objectKey, "ACS_v1_05")
            || !strcmp(objectKey, "RS_v1_05")
            || !strcmp(objectKey, "openbus_v1_05")
            || !strcmp(objectKey, "FTACS_v1_05")
           ) 
        {
          Host* newACSHost = faultToleranceManager->updateACSHostInUse();
          if (newACSHost) {
            bus->hostBus = newACSHost->name;
            bus->portBus = newACSHost->port;

            bus->createProxyToIAccessControlService();

          #ifdef OPENBUS_MICO
            if (!strcmp(objectKey, "LP_v1_05")) {
              throw ForwardRequest(bus->iLeaseProvider, false);
            } else if (!strcmp(objectKey, "ACS_v1_05")) {
              throw ForwardRequest(bus->iAccessControlService, false);
            } else if (!strcmp(objectKey, "RS_v1_05")) {
              throw ForwardRequest(bus->iRegistryService, false);
            } else if (!strcmp(objectKey, "openbus_v1_05")) {
              throw ForwardRequest(bus->iComponentAccessControlService, false);
            } else if (!strcmp(objectKey, "FTACS_v1_05")) {
              throw ForwardRequest(bus->iFaultTolerantService, false);
            }
          #else
            if (!strcmp(objectKey, "LP_v1_05")) {
              throw ForwardRequest(bus->iLeaseProvider);
            } else if (!strcmp(objectKey, "ACS_v1_05")) {
              throw ForwardRequest(bus->iAccessControlService);
            } else if (!strcmp(objectKey, "RS_v1_05")) {
              throw ForwardRequest(bus->iRegistryService);
            } else if (!strcmp(objectKey, "openbus_v1_05")) {
              throw ForwardRequest(bus->iComponentAccessControlService);
            } else if (!strcmp(objectKey, "FTACS_v1_05")) {
              throw ForwardRequest(bus->iFaultTolerantService);
            }
          #endif
          }
        }
      }
      Openbus::logger->dedent(INFO, 
        "ClientInterceptor::receive_exception() END");
    }

    void ClientInterceptor::receive_other( ClientRequestInfo_ptr ri ) 
      throw(
        CORBA::SystemException,
        PortableInterceptor::ForwardRequest)
    {}
    
    void ClientInterceptor::destroy() {}
  }
}

