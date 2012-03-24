#ifndef TECGRAF_CONNECTION_IMPL_H_
#define TECGRAF_CONNECTION_IMPL_H_

#include "connection.h"

namespace openbus {
#ifdef MULTITHREAD
  class RenewLogin : public MICOMT::Thread {
    public:
      RenewLogin(
        Connection* connection,
        idl_ac::ValidityTime validityTime);
      ~RenewLogin();
      void _run(void*);
      void stop() { sigINT = true; }
    private:
      MICOMT::Mutex* mutex;
      Connection* connection;
      bool sigINT;
      idl_ac::AccessControl_var _access_control;
      idl_ac::ValidityTime validityTime;
      bool _sleep(unsigned int time);
  };
#else
  class RenewLogin : public CORBA::DispatcherCallback {
    public:
      RenewLogin(idl_ac::AccessControl* _access_control);
      void callback(CORBA::Dispatcher* dispatcher, Event event);
    private:
      idl_ac::AccessControl* _access_control;
  };
#endif
}

#endif
