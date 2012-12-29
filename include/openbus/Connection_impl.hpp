// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_CONNECTION_IMPL_H_
#define TECGRAF_SDK_CONNECTION_IMPL_H_

#include "stubs/scs.h"
#include "stubs/core.h"
#include "stubs/credential.h"
#include "stubs/access_control.h"
#include "stubs/offer_registry.h"
#include "openbus/util/PublicKey.hpp"

#include <CORBA.h>

namespace openbus 
{
  namespace idl = tecgraf::openbus::core::v2_0;
  namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
  namespace idl_accesscontrol = idl_ac;
  namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;
  namespace idl_offerregistry = idl_or;
  namespace idl_cr = tecgraf::openbus::core::v2_0::credential;
  namespace idl_credential = idl_cr;
}

namespace openbus 
{
  struct Login;
  class LoginCache;
  class RenewLogin;
  class OpenBusContext;
}

#include "openbus/util/Mutex_impl.hpp"
#ifndef TECGRAF_SDK_LRUCACHE_H_
#define TECGRAF_SDK_LRUCACHE_H_
#include "openbus/util/LRUCache_impl.hpp"
#endif

#define LOGINCACHE_LRU_SIZE 128

namespace openbus 
{

struct Login 
{
  idl_ac::LoginInfo *loginInfo;
  idl::OctetSeq_var encodedCallerPubKey;
  std::auto_ptr<PublicKey> pubKey;
  long time2live;
  time_t timeUpdated;
};
  
typedef LRUCache<std::string, Login *> LoginLRUCache;

class LoginCache 
{
public:
  LoginCache(idl_ac::LoginRegistry_ptr);
  Login *validateLogin(const std::string id);
private:
  idl_ac::LoginRegistry_ptr _login_registry;
  LoginLRUCache _loginLRUCache;
  Mutex _mutex;
};
  
#ifdef OPENBUS_SDK_MULTITHREAD
class RenewLogin : public MICOMT::Thread 
{
public:
  RenewLogin(Connection *, idl_ac::AccessControl_ptr, OpenBusContext *, 
             idl_ac::ValidityTime);
  ~RenewLogin();
  void _run(void *);
  void stop();
  void pause();
  void run();
private:
  Mutex _mutex;
  Connection *_conn;
  idl_ac::AccessControl_ptr _access_control;
  OpenBusContext *_openbusContext;
  idl_ac::ValidityTime _validityTime;
  bool _pause;
  bool _stop;
  MICOMT::CondVar _condVar;
  idl_ac::ValidityTime renew();
};
#else
class RenewLogin : public CORBA::DispatcherCallback 
{
public:
  RenewLogin(CORBA::ORB_ptr, Connection *, idl_ac::AccessControl_ptr, 
             OpenBusContext *, idl_ac::ValidityTime);
  ~RenewLogin();
  void callback(CORBA::Dispatcher *, Event);
private:
  CORBA::ORB_ptr _orb;
  Connection *_conn;
  idl_ac::AccessControl_ptr _access_control;
  OpenBusContext *_openbusContext;
  idl_ac::ValidityTime _validityTime;
  idl_ac::ValidityTime renew(CORBA::Dispatcher *);
};
#endif
}

#endif
