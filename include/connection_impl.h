#ifndef TECGRAF_CONNECTION_IMPL_H_
#define TECGRAF_CONNECTION_IMPL_H_

#include "connection.h"

#ifndef TECGRAF_LRUCACHE_H_
#define TECGRAF_LRUCACHE_H_
#include "util/lru_cache_impl.h"
#endif

#define LOGINCACHE_LRU_SIZE 128

/* forward declarations */
namespace openbus {
  class Connection;
  class ConnectionManager;
}

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
    LoginCache(idl_ac::LoginRegistry_ptr);
    Login* validateLogin(char* id);
  private:
    idl_ac::LoginRegistry_ptr _login_registry;
    LoginLRUCache _loginLRUCache;
    time_t _timeUpdated;
  };
  
#ifdef OPENBUS_SDK_MULTITHREAD
  class RenewLogin : public MICOMT::Thread {
  public:
    RenewLogin(Connection*, idl_ac::AccessControl_ptr, ConnectionManager*, 
      idl_ac::ValidityTime validityTime);
    ~RenewLogin();
    void _run(void*);
    void stop();
    void pause();
    void run();
  private:
    MICOMT::Mutex _mutex;
    Connection* _conn;
    idl_ac::AccessControl_ptr _access_control;
    ConnectionManager* _manager;
    idl_ac::ValidityTime _validityTime;
    bool _pause;
    bool _stop;
    MICOMT::CondVar _condVar;
    idl_ac::ValidityTime renew();
  };
#else
  class RenewLogin : public CORBA::DispatcherCallback {
  public:
    RenewLogin(Connection*, idl_ac::AccessControl_ptr, ConnectionManager*, 
      idl_ac::ValidityTime validityTime);
    void callback(CORBA::Dispatcher*, Event);
  private:
    Connection* _conn;
    idl_ac::AccessControl_ptr _access_control;
    ConnectionManager* _manager;
    idl_ac::ValidityTime _validityTime;
    idl_ac::ValidityTime renew(CORBA::Dispatcher*);
  };
#endif
}

#endif
