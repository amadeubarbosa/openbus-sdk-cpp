/*
** interceptors/ServerInterceptor.h
*/

#ifndef OPENBUS_SDK_INTERCEPTORS_SERVER_INTERCEPTOR_H
#define OPENBUS_SDK_INTERCEPTORS_SERVER_INTERCEPTOR_H

#include <openbus/version.h>

#ifdef OPENBUS_ORBIX
  #include <orbix/corba.hh>
  #include <omg/PortableInterceptor.hh>
  #include <it_ts/timer.h>
  #include "../../stubs/orbix/access_control_service.hh"
#else
  #include <CORBA.h>
  #include "access_control_service.h"
#endif

#include <set>

namespace openbus {

struct orb_state;

namespace interceptors {

namespace idl_namespace = tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE;

class server_interceptor : public PortableInterceptor::ServerRequestInterceptor
#ifdef OPENBUS_ORBIX
                         , public IT_CORBA::RefCountedLocalObject 
#endif
{
private:
  PortableInterceptor::Current* picurrent;
  PortableInterceptor::SlotId slotid;
  IOP::Codec_ptr cdr_codec;
  openbus::orb_state* orb_state;

  /*
   * Intervalo em milisegundos de validação das credenciais do cache.
   */
  static unsigned long validationTime;

  struct setCredentialCompare
  {
    bool operator() (const idl_namespace::access_control_service::Credential& c1
                     , const idl_namespace::access_control_service::Credential& c2) const
    {
      return (strcmp(c1.identifier, c2.identifier) < 0);
    }
  };

  static std::set<idl_namespace::access_control_service::Credential, setCredentialCompare> 
    credentialsCache;
  static std::set<idl_namespace::access_control_service::Credential>::iterator 
    itCredentialsCache;

#ifdef OPENBUS_ORBIX
  class CredentialsValidationThread : public IT_ThreadBody 
  {
  public:
    CredentialsValidationThread();
    ~CredentialsValidationThread();
    void* run();
  };
  friend class ServerInterceptor::CredentialsValidationThread;

  CredentialsValidationThread* credentialsValidationThread;
  IT_Timer* credentialsValidationTimer;
#else
  class CredentialsValidationCallback : 
    public CORBA::DispatcherCallback 
  {
  public:
    CredentialsValidationCallback(openbus::orb_state& orb_state);
    void callback(CORBA::Dispatcher* dispatcher, Event event);

    openbus::orb_state* orb_state;
  };
  friend class server_interceptor::CredentialsValidationCallback;

  /*
   * Callback de validação do cache de credenciais.
   */
  CredentialsValidationCallback credentialsValidationCallback;
#endif

public:
  server_interceptor(PortableInterceptor::Current* ppicurrent
                    , PortableInterceptor::SlotId pslotid
                    , IOP::Codec_ptr pcdr_codec
                     , openbus::orb_state& orb_state);
  ~server_interceptor();
  void receive_request_service_contexts(PortableInterceptor::ServerRequestInfo*);
  void receive_request(PortableInterceptor::ServerRequestInfo_ptr ri);
  void send_reply(PortableInterceptor::ServerRequestInfo*);
  void send_exception(PortableInterceptor::ServerRequestInfo*);
  void send_other(PortableInterceptor::ServerRequestInfo*);
  char* name();
  void destroy();
  idl_namespace::access_control_service::Credential_var getCredential();
#ifdef OPENBUS_ORBIX
  void registerValidationTimer();
#else
  void registerValidationDispatcher();
#endif
  void setValidationTime(unsigned long pValidationTime);
  unsigned long getValidationTime();
};

} }

#endif
