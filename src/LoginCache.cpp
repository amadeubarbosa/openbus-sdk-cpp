// -*- coding: iso-8859-1 -*-
#include "openbus/LoginCache.hpp"

#include <cstddef>

const std::size_t cacheSize = 128;

namespace openbus
{
LoginCache::LoginCache(idl_ac::LoginRegistry_ptr p)
  : _login_registry(p), _loginLRUCache(cacheSize) 
{ 
}

Login *LoginCache::validateLogin(const std::string &id) 
{
  /* este login est� no cache? */
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  Login *login = _loginLRUCache.fetch(id);
  if (!login) 
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.unlock();
#endif
    login = new Login;
    login->time2live = -1;
    try 
    {
      login->loginInfo = 
        _login_registry->getLoginInfo(id.c_str(), 
                                      login->encodedCallerPubKey);
    } 
    catch (const idl_ac::InvalidLogins &) 
    { 
      return 0; 
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
  
  /* se time2live � zero ent�o o login � inv�lido. */
  if (!login->time2live) 
  {
    return 0;
  }
  
  /* se time2live � maior do que o intervalo de tempo de atualiza��o,
   * o login � v�lido. */
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
    /* o login de interesse, ap�s atualiza��o da cache, ainda � v�lido? */
    if (login->time2live > 0) 
    {
      return login;
    }
  }
  return 0;
}

}
