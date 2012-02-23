#ifndef TECGRAF_CONNECTION_IMPL_H_
#define TECGRAF_CONNECTION_IMPL_H_

#include "connection.h"

namespace openbus {
#ifdef MULTITHREAD
  class RenewLogin : public MICOMT::Thread {
    public:
      RenewLogin(
        Connection* connection,
        openbusidl_access_control::ValidityTime validityTime);
      ~RenewLogin();
      void _run(void*);
      void stop()
        { sigINT = true; }
    private:
      MICOMT::Mutex* mutex;
      Connection* connection;

      /** Flag que determina se a thread de renovação deve continuar a sua execução. */
      bool sigINT;

      /** Referência para o serviço de acesso do barramento.  */
      openbusidl_access_control::AccessControl_var _access_control;

      /** Tempo mínimo pelo qual o login permanecerá ativo sem necessidade de renovação. */
      openbusidl_access_control::ValidityTime validityTime;

      bool _sleep(unsigned int time);
  };
#else
  class RenewLogin : public CORBA::DispatcherCallback {
    public:
      RenewLogin(openbusidl_access_control::AccessControl* _access_control);
      void callback(CORBA::Dispatcher* dispatcher, Event event);
    private:
      openbusidl_access_control::AccessControl* _access_control;
  };
#endif
}

#endif
