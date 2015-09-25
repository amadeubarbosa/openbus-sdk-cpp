// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_LOGIN_CACHE_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_LOGIN_CACHE_HPP

#include "openbus/idl.hpp"
#include "openbus/detail/decl.hpp"
#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_LRUCACHE_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_LRUCACHE_HPP
  #include "openbus/detail/lru_cache.hpp"
#endif

#include <boost/shared_ptr.hpp>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/thread.hpp>
#pragma clang diagnostic pop
#include <string>

namespace openbus 
{
class PublicKey;

struct OPENBUS_SDK_DECL Login 
{
  idl::access::LoginInfo_var loginInfo;
  idl::core::OctetSeq_var encodedCallerPubKey;
  std::auto_ptr<PublicKey> pubKey;
  long time2live;
  time_t timeUpdated;
};

typedef LRUCache<std::string, boost::shared_ptr<Login> > LoginLRUCache;

class OPENBUS_SDK_DECL LoginCache 
{
public:
  LoginCache(idl::access::LoginRegistry_var);
  boost::shared_ptr<Login> validateLogin(const std::string &id);
private:
  idl::access::LoginRegistry_var _login_registry;
  LoginLRUCache _loginLRUCache;
  boost::mutex _mutex;
};
}
#endif
