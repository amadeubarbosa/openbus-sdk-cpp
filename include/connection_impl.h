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
    LoginCache(Connection* c) : _conn(c), _loginLRUCache(LOGINCACHE_LRU_SIZE)
    {
    }
    //return 0: login inv�lido
    Login* validateLogin(char* id);
  private:
    Connection* _conn;
    LoginLRUCache _loginLRUCache;
    time_t _timeUpdated;
  };
  
#ifdef OPENBUS_SDK_MULTITHREAD
  class RenewLogin : public MICOMT::Thread {
  public:
    RenewLogin(Connection*, ConnectionManager*, idl_ac::ValidityTime validityTime);
    ~RenewLogin();
    void _run(void*);
    void stop();
    void pause();
    void run();
  private:
    MICOMT::Mutex _mutex;
    Connection* _conn;
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
    RenewLogin(Connection*, ConnectionManager*, idl_ac::ValidityTime validityTime);
    void callback(CORBA::Dispatcher* dispatcher, Event event);
  private:
    Connection* _conn;
    ConnectionManager* _manager;
    idl_ac::ValidityTime _validityTime;
    idl_ac::ValidityTime renew(CORBA::Dispatcher*);
  };
#endif
}

#endif
