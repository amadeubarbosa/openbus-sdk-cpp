// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_LOGIN_CACHE_HPP
#define TECGRAF_SDK_OPENBUS_LOGIN_CACHE_HPP

#include "openbus/detail/decl.hpp"
#include "openbus_core-2.1C.h"
#include "openbus_access-2.1C.h"
#ifndef TECGRAF_SDK_OPENBUS_LRUCACHE_HPP
#define TECGRAF_SDK_OPENBUS_LRUCACHE_HPP
  #include "openbus/detail/LRUCache.hpp"
#endif

#include <boost/shared_ptr.hpp>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <string>

namespace openbus 
{
namespace idl = tecgraf::openbus::core::v2_1;
namespace idl_ac = tecgraf::openbus::core::v2_1::services::access_control;

class PublicKey;

struct OPENBUS_SDK_DECL Login 
{
  idl_ac::LoginInfo_var loginInfo;
  idl::OctetSeq_var encodedCallerPubKey;
  std::auto_ptr<PublicKey> pubKey;
  long time2live;
  time_t timeUpdated;
};

typedef LRUCache<std::string, boost::shared_ptr<Login> > LoginLRUCache;

class OPENBUS_SDK_DECL LoginCache 
{
public:
  LoginCache(idl_ac::LoginRegistry_var);
  boost::shared_ptr<Login> validateLogin(const std::string &id);
private:
  idl_ac::LoginRegistry_var _login_registry;
  LoginLRUCache _loginLRUCache;
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::mutex _mutex;
#endif
};
}
#endif
