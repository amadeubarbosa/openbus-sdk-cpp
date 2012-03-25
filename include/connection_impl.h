#ifndef TECGRAF_CONNECTION_IMPL_H_
#define TECGRAF_CONNECTION_IMPL_H_

#include "connection.h"

namespace openbus {
  struct Login {
    idl_ac::LoginInfo* loginInfo;
    idl::OctetSeq_var encodedCallerPubKey;
    EVP_PKEY* key;
  };
  
  class LoginCache {
    public:
      LoginCache() { }
      LoginCache(Connection* c) : _conn(c) { }
      void connection(Connection* c) { _conn = c; }
      Login* validateLogin(char* id);
    private:
      Connection* _conn;
      std::map<std::string, Login*> _id_Login;
      std::map<std::string, unsigned long> _validity;
  };
  
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
