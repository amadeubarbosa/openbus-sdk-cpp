// -*- coding: iso-8859-1 -*-
#include "openbus/LoginCache.hpp"

#define LOGINCACHE_LRU_SIZE 128

namespace openbus
{
LoginCache::LoginCache(idl_ac::LoginRegistry_ptr p)
  : _login_registry(p), _loginLRUCache(LOGINCACHE_LRU_SIZE) 
{ 
}

Login *LoginCache::validateLogin(const std::string id) 
{
  /* este login está no cache? */
  AutoLock m(&_mutex);
  Login *login = _loginLRUCache.fetch(id);
  if (!login) 
  {
    m.unlock();
    login = new Login;
    login->time2live = -1;
    try 
    {
      login->loginInfo = 
        _login_registry->getLoginInfo(id.c_str(), 
                                      login->encodedCallerPubKey);
    } 
    catch (const idl_ac::InvalidLogins &e) 
    { 
      return 0; 
    }
    login->pubKey = 
      std::auto_ptr<PublicKey> (new PublicKey(login->encodedCallerPubKey));
    login->timeUpdated = time(0);
    m.lock();
    _loginLRUCache.insert(id, login);
  }
  m.unlock();
  
  /* se time2live é zero então o login é inválido. */
  if (!login->time2live) 
  {
    return 0;
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
  return 0;
}

}
