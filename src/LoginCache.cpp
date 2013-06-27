// -*- coding: iso-8859-1 -*-
#include "openbus/LoginCache.hpp"

#include <cstddef>
#include <boost/make_shared.hpp>

const std::size_t cacheSize = 128;

namespace openbus
{
LoginCache::LoginCache(idl_ac::LoginRegistry_ptr p)
  : _login_registry(p), _loginLRUCache(cacheSize) 
{ 
}

boost::shared_ptr<Login> LoginCache::validateLogin(const std::string &id) 
{
  /* este login está no cache? */
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  boost::shared_ptr<Login> login = _loginLRUCache.fetch(id);
  if (!login) 
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.unlock();
#endif
    login = boost::make_shared<Login>();
    login->time2live = -1;
    try 
    {
      login->loginInfo = 
        _login_registry->getLoginInfo(id.c_str(), 
                                      login->encodedCallerPubKey);
    } 
    catch (const idl_ac::InvalidLogins &) 
    { 
      return boost::shared_ptr<Login>();
    }
    login->pubKey.reset(new PublicKey(login->encodedCallerPubKey));
    login->timeUpdated = time(0);
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.lock();
#endif
    _loginLRUCache.insert(id, login);
  }
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif
  
  /* se time2live é zero então o login é inválido. */
  if (!login->time2live) 
  {
    return boost::shared_ptr<Login>();
  }
  
  /* se time2live é maior do que o intervalo de tempo de atualização,
   * o login é válido. */
  if (login->time2live > (time(0) - login->timeUpdated)) 
  {
    return login;
  }
  else 
  {
    /* preciso consultar o barramento para validar o login. */
    idl_ac::ValidityTime validity = 
      _login_registry->getLoginValidity(id.c_str());
    login->time2live = validity;
    login->timeUpdated = time(0);
    /* o login de interesse, após atualização da cache, ainda é válido? */
    if (login->time2live > 0) 
    {
      return login;
    }
  }
  return boost::shared_ptr<Login>();
}

}
