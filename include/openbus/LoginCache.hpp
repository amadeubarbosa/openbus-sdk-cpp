// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_LOGIN_CACHE_H_
#define TECGRAF_SDK_LOGIN_CACHE_H_

#include "stubs/core.h"
#include "stubs/access_control.h"
#include "openbus/lock/Mutex_impl.hpp"
#ifndef TECGRAF_SDK_LRUCACHE_H_
#define TECGRAF_SDK_LRUCACHE_H_
#include "openbus/LRUCache_impl.hpp"
#endif
#include "openbus/crypto/PublicKey.hpp"
#include "openbus/lock/AutoLock_impl.hpp"

#include <string>

namespace openbus 
{
namespace idl = tecgraf::openbus::core::v2_0;
namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;

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
}
#endif
