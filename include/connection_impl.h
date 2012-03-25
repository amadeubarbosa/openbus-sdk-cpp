#ifndef TECGRAF_CONNECTION_IMPL_H_
#define TECGRAF_CONNECTION_IMPL_H_

#include "connection.h"
#include "util/lru_cache.h"

#define LOGINCACHE_LRU_SIZE 128

namespace openbus {
  struct Login {
    idl_ac::LoginInfo* loginInfo;
    idl::OctetSeq_var encodedCallerPubKey;
    EVP_PKEY* key;
    int indexSeq;
    long time2live;
  };
  
  typedef LRUCache<std::string, Login*> LoginLRUCache;

  class LoginCache {
    public:
      LoginCache(Connection* c) : _conn(c) {
        _id_Login = new LoginLRUCache(LOGINCACHE_LRU_SIZE);
      }
      Login* validateLogin(char* id);
      void connection(Connection* c) { _conn = c; }
    private:
      Connection* _conn;
      LoginLRUCache* _id_Login;
      time_t _timeUpdated;
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
