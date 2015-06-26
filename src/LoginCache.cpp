// -*- coding: iso-8859-1-unix -*-
#include "openbus/detail/LoginCache.hpp"
#include "openbus/crypto/PublicKey.hpp"

#include <cstddef>
#include <boost/make_shared.hpp>

const std::size_t cacheSize(128);

namespace openbus
{
LoginCache::LoginCache(idl_ac::LoginRegistry_var p)
  : _login_registry(p), _loginLRUCache(cacheSize) 
{ 
}

boost::shared_ptr<Login> LoginCache::validateLogin(const std::string &id) 
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  boost::shared_ptr<Login> login(_loginLRUCache.fetch(id));
  if (!login) 
  {
    login = boost::make_shared<Login>();
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.unlock();
#endif
    try
    {
      login->loginInfo = _login_registry->getLoginInfo(
        id.c_str(), login->encodedCallerPubKey);
    }
    catch (const idl_ac::InvalidLogins &)
    {
      return boost::shared_ptr<Login>();
    }
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.lock();
#endif
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
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif
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
