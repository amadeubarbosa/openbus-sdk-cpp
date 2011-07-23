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

using namespace tecgraf::openbus::core::v1_05;
using namespace PortableInterceptor;
using namespace std;

namespace openbus {
  namespace interceptors {

    class ServerInterceptor : public ServerRequestInterceptor
    #ifdef OPENBUS_ORBIX
                              ,public IT_CORBA::RefCountedLocalObject 
    #endif
    {
      private:
        Current* picurrent;
        SlotId slotid;
        IOP::Codec_ptr cdr_codec;

      /*
      * Intervalo em milisegundos de validação das credenciais do cache.
      */
        static unsigned long validationTime;

        struct setCredentialCompare {
          bool operator() (
            const access_control_service::Credential& c1,
            const access_control_service::Credential& c2) const
          {
            return (strcmp(c1.identifier, c2.identifier) < 0);
          }
        };

        static set<access_control_service::Credential, setCredentialCompare> 
          credentialsCache;
        static set<access_control_service::Credential, setCredentialCompare>::iterator 
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
        ServerInterceptor(Current* ppicurrent, 
          SlotId pslotid, 
          IOP::Codec_ptr pcdr_codec);
        ~ServerInterceptor();
        void receive_request_service_contexts(ServerRequestInfo*);
        void receive_request(ServerRequestInfo_ptr ri);
        void send_reply(ServerRequestInfo*);
        void send_exception(ServerRequestInfo*);
        void send_other(ServerRequestInfo*);
        char* name();
        void destroy();
        access_control_service::Credential_var getCredential();
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
