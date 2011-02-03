/*
** interceptors/ServerInterceptor.cpp
*/

#include <openbus/interceptors/server_interceptor.h>
#include <openbus/openbus.h>
#include <openbus/orb_state.h>

#include <sstream>

namespace openbus { namespace interceptors {

unsigned long server_interceptor::validationTime = 30000; /* ms */

namespace idl_namespace = tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE;

std::set<idl_namespace::access_control_service::Credential>::iterator server_interceptor::itCredentialsCache;
std::set<idl_namespace::access_control_service::Credential, server_interceptor::setCredentialCompare> 
  server_interceptor::credentialsCache;

server_interceptor::CredentialsValidationCallback::CredentialsValidationCallback(openbus::orb_state& orb_state)
  : orb_state(&orb_state)
{
}

void server_interceptor::CredentialsValidationCallback::callback(CORBA::Dispatcher* dispatcher
                                                                , Event event)
{
  std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << std::endl;
  idl_namespace::access_control_service::IAccessControlService* iAccessControlService = 
    orb_state->getAccessControlService();
  for (itCredentialsCache = credentialsCache.begin();
       itCredentialsCache != credentialsCache.end(); 
       itCredentialsCache++)
  {
    std::cout << "Validando a credencial: " << 
      (const char*) ((*itCredentialsCache).identifier) << " ...";
    //Openbus::logger->log(INFO, out.str());
    try
    {
      if (iAccessControlService->isValid(*itCredentialsCache))
      {
        //Openbus::logger->log(INFO, "Credencial ainda é válida.");
      }
      else
      {
        //Openbus::logger->log(WARNING, "Credencial NÃO é mais válida!");
        credentialsCache.erase(itCredentialsCache++);
      }
    }
    catch (CORBA::SystemException& e)
    {
      //Openbus::logger->log(ERROR, "Erro ao verificar validade da credencial");
    }
    ++itCredentialsCache;
  }
  dispatcher->tm_event(this, validationTime);
  std::stringstream str;
  str << "Próxima validação em: " << validationTime << "ms" << std::endl;
  //Openbus::logger->log(INFO, str.str());
  //Openbus::logger->dedent(INFO, "ServerInterceptor::CredentialsValidationCallback() END");
}

server_interceptor::server_interceptor(PortableInterceptor::Current* ppicurrent
                                       , PortableInterceptor::SlotId pslotid
                                       , IOP::Codec_ptr pcdr_codec
                                       , openbus::orb_state& orb_state)
  : slotid(pslotid), picurrent(ppicurrent), cdr_codec(pcdr_codec)
  , orb_state(&orb_state)
  , credentialsValidationCallback(orb_state)
{
  std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << std::endl;
  //Openbus::logger->log(INFO, "ServerInterceptor::ServerInterceptor() BEGIN");
  //Openbus::logger->indent();
#ifdef OPENBUS_ORBIX
  credentialsValidationTimer = 0;
#endif
  //Openbus::logger->dedent(INFO, "ServerInterceptor::ServerInterceptor() END");
}

server_interceptor::~server_interceptor()
{
  std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << std::endl;
#ifdef OPENBUS_ORBIX
  if (credentialsValidationTimer)
  {
    credentialsValidationTimer->stop();
    delete credentialsValidationThread;
    delete credentialsValidationTimer;
  }
#else
  orb_state->orb->dispatcher()->remove(&credentialsValidationCallback
                                            , CORBA::Dispatcher::Timer);
#endif
}

void server_interceptor::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri)
{
  std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << std::endl;
  ::IOP::ServiceContext_var sc = ri->get_request_service_context(1234);
  CORBA::String_var repID = ri->target_most_derived_interface();
  CORBA::String_var operation_var = ri->operation();

  std::stringstream request;
  request << "RepID: " << repID << "\n";
  request << "Método: " << operation_var;
  std::cout << request.str() << std::endl;
  
  const char* rep_id = repID;
  const char* operation = operation_var;
  if (orb_state->isInterceptable(rep_id, operation))
  {
    CORBA::ULong z;
    std::stringstream contextData;
    contextData << "Context Data: ";
    for (z = 0; z < sc->context_data.length(); z++)
    {
      contextData << (unsigned) sc->context_data[z];
    }
    std::cout << contextData.str() << std::endl;

    IOP::ServiceContext::_context_data_seq& context_data = sc->context_data;
    
    CORBA::OctetSeq octets(context_data.length()
                           , context_data.length()
                           , context_data.get_buffer()
                           , 0);

    CORBA::Any_var any = cdr_codec->decode_value(octets
                                                 , idl_namespace::access_control_service::_tc_Credential);
#ifdef OPENBUS_ORBIX
    idl_namespace::access_control_service::Credential* c;
#else
    idl_namespace::access_control_service::Credential c;
#endif
    any >>= c;
    CredentialValidationPolicy policy = orb_state->getCredentialValidationPolicy(); 
    picurrent->set_slot(slotid, any);
    if (policy == ALWAYS)
    {
      try
      {
#ifdef OPENBUS_ORBIX
        if (orb_state->getAccessControlService()->isValid(*c))
        {
#else
        if (orb_state->getAccessControlService()->isValid(c))
        {
#endif
        }
        else
        {
          throw CORBA::NO_PERMISSION();
        }
      }
      catch (CORBA::SystemException& e)
      {
        throw;
      }
    }
    else if (policy == CACHED)
    {
      std::stringstream out;
      out << "Número de credenciais no cache: " << credentialsCache.size();
#ifdef OPENBUS_ORBIX
      if (credentialsCache.find(*c) != credentialsCache.end()
          && !credentialsCache.empty()) 
      {  
#else
      if (credentialsCache.find(c) != credentialsCache.end()
          && !credentialsCache.empty()) 
      {  
#endif
      }
      else
      {
        try
        {
#ifdef OPENBUS_ORBIX
          if (orb_state->getAccessControlService()->isValid(*c))
          {
#else
          if (orb_state->getAccessControlService()->isValid(c))
          {
#endif
          }
          else
          {
            throw CORBA::NO_PERMISSION();
          }
#ifdef OPENBUS_ORBIX
          credentialsCache.insert(*c);
#else
          credentialsCache.insert(c);
#endif  
        }
        catch (CORBA::SystemException& e)
        {
          throw;
        }
      }
    }
    else
    {
    }
  }
}          
      
void server_interceptor::receive_request_service_contexts(PortableInterceptor::ServerRequestInfo*)
{
  std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << std::endl;
}
void server_interceptor::send_reply(PortableInterceptor::ServerRequestInfo*)
{
  std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << std::endl;
}
void server_interceptor::send_exception(PortableInterceptor::ServerRequestInfo*)
{
  std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << std::endl;
}
void server_interceptor::send_other(PortableInterceptor::ServerRequestInfo*)
{
  std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << std::endl;
}

char* server_interceptor::name()
{
  std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << std::endl;
  return CORBA::string_dup("AccessControl");
}

void server_interceptor::destroy() {}

idl_namespace::access_control_service::Credential_var server_interceptor::getCredential()
{
  std::cout << __FILE__ ":" << __LINE__ << " " << __FUNCTION__ << std::endl;
  //Openbus::logger->log(INFO, "ServerInterceptor::getCredential() BEGIN");
  //Openbus::logger->indent();
  CORBA::Any_var any = picurrent->get_slot(slotid);

#ifdef OPENBUS_ORBIX
  access_control_service::Credential* c = 0;
  any >>= c;
  if (c)
  {
    //Openbus::logger->log(INFO, "credential->owner: " + (string) c->owner);
    //Openbus::logger->log(INFO, "credential->identifier: " + (string) c->identifier);
    //Openbus::logger->log(INFO, "credential->delegate: " + (string) c->delegate);
    access_control_service::Credential_var ret = new access_control_service::Credential();
    ret->owner = CORBA::string_dup(c->owner);
    ret->identifier = CORBA::string_dup(c->identifier);
    ret->delegate = CORBA::string_dup(c->delegate);
#else
  idl_namespace::access_control_service::Credential c;
  if (any >>=c)
  {
    //Openbus::logger->log(INFO, "credential->owner: " + (string) c.owner);
    //Openbus::logger->log(INFO, "credential->identifier: " + (string) c.identifier);
    //Openbus::logger->log(INFO, "credential->delegate: " + (string) c.delegate);
    idl_namespace::access_control_service::Credential_var ret = new idl_namespace::access_control_service::Credential();
    ret->owner = CORBA::string_dup(c.owner);
    ret->identifier = CORBA::string_dup(c.identifier);
    ret->delegate = CORBA::string_dup(c.delegate);
#endif
    //Openbus::logger->dedent(INFO, "ServerInterceptor::getCredential() END");
    return ret._retn();
  }
  else
  {
    //Openbus::logger->dedent(INFO, "ServerInterceptor::getCredential() END");
    return 0;
  }
}

#ifdef OPENBUS_ORBIX
void server_interceptor::registerValidationTimer()
{
  IT_Duration start(0);
  IT_Duration interval(validationTime/1000, 0);
  credentialsValidationThread = new CredentialsValidationThread();
  credentialsValidationTimer = new IT_Timer(start
                                            , interval
                                            , *credentialsValidationThread
                                            , true);
}
#else
void server_interceptor::registerValidationDispatcher()
{
  orb_state->orb->dispatcher()->tm_event(&credentialsValidationCallback
                                              , validationTime);
}
#endif

void server_interceptor::setValidationTime(unsigned long pValidationTime)
{
  validationTime = pValidationTime;
}

unsigned long server_interceptor::getValidationTime()
{
  return validationTime;
}

} }
