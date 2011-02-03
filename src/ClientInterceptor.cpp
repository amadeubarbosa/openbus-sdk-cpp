/*
** interceptors/ClientInterceptor.cpp
*/

#include <openbus/interceptors/client_interceptor.h>
#include <openbus/orb_state.h>

#include "openbus/openbus.h"

#ifdef OPENBUS_ORBIX
  #include <it_ts/thread.h>
#endif

#include <sstream>

namespace openbus { namespace interceptors {

namespace idl_namespace = tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE;
namespace fault_tolerance = tecgraf::openbus::fault_tolerance;

idl_namespace::access_control_service::Credential* client_interceptor::credential = 0;

client_interceptor::client_interceptor(IOP::Codec_ptr pcdr_codec
                                       , openbus::orb_state& orb_state)
  : orb_state(&orb_state)
{
  //Openbus::logger->log(INFO, "client_interceptor::client_interceptor() BEGIN");
  //Openbus::logger->indent();
  cdr_codec = pcdr_codec;
  faultToleranceManager.reset(new fault_tolerance_manager(orb_state));
  if (orb_state.FTConfigFilename)
  {
    faultToleranceManager->loadConfig(orb_state.FTConfigFilename);
  }
  //Openbus::logger->dedent(INFO, "client_interceptor::client_interceptor() END");
}

client_interceptor::~client_interceptor() {}

void client_interceptor::send_request(PortableInterceptor::ClientRequestInfo_ptr ri) 
  throw(CORBA::SystemException
        , PortableInterceptor::ForwardRequest)
{
  std::cout << __FILE__ ":" << __LINE__  << "client_interceptor::send_request" << std::endl;
  //Openbus::logger->log(INFO, "client_interceptor::send_request() BEGIN");
  //Openbus::logger->indent();
  std::stringstream msg;
  char * operation = ri->operation();
  msg << "Method: " << operation;
  std::cout << msg.str() << std::endl;
  //Openbus::logger->log(INFO, msg.str());
  free(operation);
  if (credential)
  {
    std::cout << __FILE__ ":" << __LINE__  << "There is a credential to intercept" << std::endl;
    std::stringstream msg;
    msg << "[identifier]: " << credential->identifier << "\n";
    msg << "[owner]: " << credential->owner << "\n";
    msg << "[delegate]: " << credential->delegate;

    std::cout << __FILE__ ":" << __LINE__  << msg.str() << std::endl;

    IOP::ServiceContext sc;
    sc.context_id = 1234;

    CORBA::Any any;
    any <<= *credential;
    CORBA::OctetSeq_var octets;
    octets = cdr_codec->encode_value(any);
    IOP::ServiceContext::_context_data_seq seq(octets->length()
                                               , octets->length()
                                               , octets->get_buffer()
                                               , 0);
    sc.context_data = seq;

    CORBA::ULong z;
    std::stringstream contextData;
    contextData << "Context data: ";
    for ( z = 0; z < sc.context_data.length(); z++ )
    {
      contextData <<  (unsigned) sc.context_data[ z ] << " ";
    }
    std::cout << __FILE__ ":" << __LINE__  << "context data" << contextData.str() << std::endl;

    ri->add_request_service_context(sc, true);
  }
    //Openbus::logger->dedent(INFO, "client_interceptor::send_request() END");
}

char* client_interceptor::name() throw(CORBA::SystemException)
{
  return CORBA::string_dup("AccessControl");
}
    
void client_interceptor::send_poll( PortableInterceptor::ClientRequestInfo_ptr ri ) 
  throw(CORBA::SystemException)
{
  std::cout << __FILE__ ":" << __LINE__  << " send_poll" << std::endl;
}
    
void client_interceptor::receive_reply( PortableInterceptor::ClientRequestInfo_ptr ri ) 
  throw(CORBA::SystemException)
{
  std::cout << __FILE__ ":" << __LINE__  << " receive_reply" << std::endl;
}

void client_interceptor::receive_exception( PortableInterceptor::ClientRequestInfo_ptr ri ) 
  throw(CORBA::SystemException
        , PortableInterceptor::ForwardRequest)
{
  std::cout << __FILE__ ":" << __LINE__  << "receive exception" << std::endl;
  //Openbus::logger->log(INFO, "client_interceptor::receive_exception() BEGIN");
  //Openbus::logger->indent();
  std::stringstream out;
  const char* received_exception_id = ri->received_exception_id();
  PortableInterceptor::ReplyStatus reply_status = ri->reply_status();
  out << "Exception: " << received_exception_id;
  //Openbus::logger->log(INFO, out.str());
  out.str(" ");
  out << "Reply Status: " << reply_status;
  //Openbus::logger->log(INFO, out.str());
  out.str(" ");
  if (reply_status == 1 
      && strcmp(received_exception_id, "IDL:omg.org/CORBA/NO_PERMISSION:1.0")) 
  {
    //Openbus::logger->log(ERROR, "TRATANDO EXCEÇÂO RECEBIDA DO SERVIDOR!");
    const char* operation = ri->operation();
    out << "Método: " << operation;
    //Openbus::logger->log(INFO, out.str());
    out.str(" ");
        
#ifdef OPENBUS_ORBIX
    char* objectKey = "(null)";
    lua_getglobal(orb_state->luaState, "IOR");
    lua_getfield(orb_state->luaState, -1, "IIOPProfileGetObjectKey");
    lua_pushstring(orb_state->luaState
                   , orb_state->getORB()->object_to_string(ri->target()));
    if (lua_pcall(orb_state->luaState, 1, 1, 0))
    {
      const char* errmsg = lua_tostring(orb_state->luaState, -1);
      lua_pop(orb_state->luaState, 1);
      //Openbus::logger->log(ERROR, errmsg);
    }
    else
    {
      objectKey = (char*) lua_tostring(orb_state->luaState, -1);
    }
#else
    CORBA::Long objectKeyLen;
    const CORBA::Octet* objectKeyOct = ri->target()->_ior()->get_profile(0)
      ->objectkey(objectKeyLen);
    char* objectKey = new char[objectKeyLen+1];
    memcpy(objectKey, objectKeyOct, objectKeyLen);
    objectKey[objectKeyLen] = '\0';
#endif
    out << "ObjectKey: " << objectKey;
    //Openbus::logger->log(INFO, out.str());
    if (!strcmp(objectKey, "LP_" OPENBUS_IDL_VERSION_STRING)
        || !strcmp(objectKey, "ACS_" OPENBUS_IDL_VERSION_STRING)
        || !strcmp(objectKey, "RS_" OPENBUS_IDL_VERSION_STRING)
        || !strcmp(objectKey, "openbus_" OPENBUS_IDL_VERSION_STRING)
        || !strcmp(objectKey, "FTACS_" OPENBUS_IDL_VERSION_STRING)
        ) 
    {
      Host* newACSHost = faultToleranceManager->updateACSHostInUse();
      if (newACSHost)
      {
        orb_state->hostBus = newACSHost->name;
        orb_state->portBus = newACSHost->port;

        orb_state->createProxyToIAccessControlService();

#ifdef OPENBUS_ORBIX
        if (!strcmp(objectKey, "LP_" OPENBUS_IDL_VERSION_STRING))
        {
          throw PortableInterceptor::ForwardRequest(orb_state->iLeaseProvider);
        }
        else if (!strcmp(objectKey, "ACS_" OPENBUS_IDL_VERSION_STRING))
        {
          throw PortableInterceptor::ForwardRequest(orb_state->iAccessControlService);
        }
        else if (!strcmp(objectKey, "RS_" OPENBUS_IDL_VERSION_STRING))
        {
          throw PortableInterceptor::ForwardRequest(orb_state->iRegistryService);
        }
        else if (!strcmp(objectKey, "openbus_" OPENBUS_IDL_VERSION_STRING))
        {
          throw PortableInterceptor::ForwardRequest(orb_state->iComponentAccessControlService);
        }
        else if (!strcmp(objectKey, "FTACS_" OPENBUS_IDL_VERSION_STRING)
                 && !CORBA::is_nil(orb_state->iFaultTolerantService))
        {
          throw PortableInterceptor::ForwardRequest(orb_state->iFaultTolerantService);
        }
#else
        if (!strcmp(objectKey, "LP_" OPENBUS_IDL_VERSION_STRING))
        {
          throw PortableInterceptor::ForwardRequest(orb_state->iLeaseProvider, false);
        }
        else if (!strcmp(objectKey, "ACS_" OPENBUS_IDL_VERSION_STRING))
        {
          throw PortableInterceptor::ForwardRequest(orb_state->iAccessControlService, false);
        }
        else if (!strcmp(objectKey, "RS_" OPENBUS_IDL_VERSION_STRING))
        {
          throw PortableInterceptor::ForwardRequest(orb_state->iRegistryService, false);
        }
        else if (!strcmp(objectKey, "openbus_" OPENBUS_IDL_VERSION_STRING))
        {
          throw PortableInterceptor::ForwardRequest(orb_state->iComponentAccessControlService, false);
        }
        else if (!strcmp(objectKey, "FTACS_" OPENBUS_IDL_VERSION_STRING)
                 && !CORBA::is_nil(orb_state->iFaultTolerantService))
        {
          throw PortableInterceptor::ForwardRequest(orb_state->iFaultTolerantService, false);
        }
#endif
      }
    }
  }
  //Openbus::logger->dedent(INFO, "client_interceptor::receive_exception() END");
}

void client_interceptor::receive_other( PortableInterceptor::ClientRequestInfo_ptr ri ) 
  throw(CORBA::SystemException
        , PortableInterceptor::ForwardRequest)
{
  std::cout << __FILE__ ":" << __LINE__  << "receive other" << std::endl;
}
    
void client_interceptor::destroy() {}

} }
