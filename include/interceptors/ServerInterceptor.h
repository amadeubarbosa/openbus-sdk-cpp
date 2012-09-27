/*
** interceptors/ServerInterceptor.h
*/

#ifndef SERVERINTERCEPTOR_H_
#define SERVERINTERCEPTOR_H_

#ifdef OPENBUS_ORBIX
  #include <orbix/corba.hh>
  #include <omg/PortableInterceptor.hh>
  #include <it_ts/timer.h>
  #include "stubs/orbix/access_control_service.hh"
#else
  #include <CORBA.h>
  #include "stubs/mico/access_control_service.h"
#endif

#include <set>

namespace openbus {
  namespace interceptors {

    class ServerInterceptor : public PortableInterceptor::ServerRequestInterceptor
    #ifdef OPENBUS_ORBIX
                              ,public IT_CORBA::RefCountedLocalObject 
    #endif
    {
      private:
      PortableInterceptor::Current* picurrent;
      PortableInterceptor::SlotId slotid;
        IOP::Codec_ptr cdr_codec;

      /*
      * Intervalo em milisegundos de validação das credenciais do cache.
      */
        static unsigned long validationTime;

        struct setCredentialCompare {
          bool operator() (const tecgraf::openbus::core::v1_05::access_control_service::Credential& c1,
                           const tecgraf::openbus::core::v1_05::access_control_service::Credential& c2) const
          {
            return (strcmp(c1.identifier, c2.identifier) < 0);
          }
        };

        static std::set<tecgraf::openbus::core::v1_05::access_control_service::Credential, setCredentialCompare> 
          credentialsCache;
        static std::set<tecgraf::openbus::core::v1_05::access_control_service::Credential, setCredentialCompare>::iterator 
          itCredentialsCache;

      #ifdef OPENBUS_ORBIX
        class CredentialsValidationThread : public IT_ThreadBody {
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
            CredentialsValidationCallback();
            void callback(CORBA::Dispatcher* dispatcher, Event event);
        };
        friend class ServerInterceptor::CredentialsValidationCallback;

      /*
      * Callback de validação do cache de credenciais.
      */
        CredentialsValidationCallback credentialsValidationCallback;
      #endif

      public:
        ServerInterceptor(PortableInterceptor::Current* ppicurrent, 
                          PortableInterceptor::SlotId pslotid, 
                          IOP::Codec_ptr pcdr_codec);
        ~ServerInterceptor();
        void receive_request_service_contexts(PortableInterceptor::ServerRequestInfo*);
        void receive_request(PortableInterceptor::ServerRequestInfo_ptr ri);
        void send_reply(PortableInterceptor::ServerRequestInfo*);
        void send_exception(PortableInterceptor::ServerRequestInfo*);
        void send_other(PortableInterceptor::ServerRequestInfo*);
        char* name();
        void destroy();
        tecgraf::openbus::core::v1_05::access_control_service::Credential_var getCredential();
      #ifdef OPENBUS_ORBIX
        void registerValidationTimer();
      #else
        void registerValidationDispatcher();
      #endif
        void setValidationTime(unsigned long pValidationTime);
        unsigned long getValidationTime();
    };
  }
}

#endif
