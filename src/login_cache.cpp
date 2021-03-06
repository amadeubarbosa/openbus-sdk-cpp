// -*- coding: iso-8859-1-unix -*-

#include "openbus/idl.hpp"
#include "openbus/detail/login_cache.hpp"
#include "openbus/detail/openssl/public_key.hpp"

#include <cstddef>
#include <boost/make_shared.hpp>

const std::size_t cacheSize(128);

namespace openbus
{
LoginCache::LoginCache(idl::access::LoginRegistry_var p)
  : _login_registry(p), _loginLRUCache(cacheSize) 
{ 
}

boost::shared_ptr<Login> LoginCache::validateLogin(const std::string &id) 
{
  boost::unique_lock<boost::mutex> lock(_mutex);
  boost::shared_ptr<Login> login(_loginLRUCache.fetch(id));
  if (!login) 
  {
    login = boost::make_shared<Login>();
    lock.unlock();
    try
    {
      login->loginInfo = _login_registry->getLoginInfo(
        id.c_str(), login->encodedCallerPubKey);
    }
    catch (const idl::access::InvalidLogins &)
    {
      return boost::shared_ptr<Login>();
    }
    lock.lock();
    boost::shared_ptr<Login> again(_loginLRUCache.fetch(id));
    if (again)
    {
      login = again;
    }
    else
    {
      login->time2live = -1;
      login->pubKey.reset(new PublicKey(login->encodedCallerPubKey));
      login->timeUpdated = time(0);
      _loginLRUCache.insert(id, login);
    }
  }
  lock.unlock();
  if (login->time2live > (time(0) - login->timeUpdated)) 
  {
    return login;
  }
  else 
  {
    login->time2live = _login_registry->getLoginValidity(id.c_str());
    login->timeUpdated = time(0);
    if (login->time2live > 0) 
    {
      return login;
    }
  }
  return boost::shared_ptr<Login>();
}

}
