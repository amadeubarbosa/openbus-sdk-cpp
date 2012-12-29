// -*- coding: iso-8859-1 -*-
#include "openbus/util/AutoLock_impl.h"
#include "openbus/Connection.h"
#include "openbus/OpenBusContext.h"
#include "openbus/log.h"

#include <sstream>
#include <unistd.h>
#include <cstring>
#include <ctime>

namespace openbus {
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
      login->loginInfo = _login_registry->getLoginInfo(id.c_str(), login->encodedCallerPubKey);
    } 
    catch (const idl_ac::InvalidLogins &e) 
    { 
      return 0; 
    }
    login->pubKey = std::auto_ptr<PublicKey> (new PublicKey(login->encodedCallerPubKey));
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
  
  /* se time2live é maior do que o intervalo de tempo de atualização, o login é válido. */
  if (login->time2live > (time(0) - login->timeUpdated)) 
  {
    return login;
  }
  else 
  {
    /* preciso consultar o barramento para validar o login. */
    idl_ac::ValidityTime validity = _login_registry->getLoginValidity(id.c_str());
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

#ifdef OPENBUS_SDK_MULTITHREAD
RenewLogin::RenewLogin(Connection *c, idl_ac::AccessControl_ptr a, OpenBusContext *m, 
                       idl_ac::ValidityTime t)
  : _conn(c), _access_control(a), _openbusContext(m), _validityTime(t), _pause(false), 
    _stop(false), _condVar(_mutex.mutex())
{
  log_scope l(log.general_logger(), info_level, "RenewLogin::RenewLogin");
}

RenewLogin::~RenewLogin() 
{ 
  log_scope l(log.general_logger(), info_level, "RenewLogin::~RenewLogin");
}

idl_ac::ValidityTime RenewLogin::renew() 
{
  log_scope l(log.general_logger(), info_level, "RenewLogin::renew");
  assert(_access_control);
  idl_ac::ValidityTime validityTime = _validityTime;
  try 
  {
    l.level_log(debug_level, "access_control()->renew()");
    validityTime = _access_control->renew();
  } 
  catch (const CORBA::Exception &) 
  {
    l.level_vlog(warning_level, "Falha na renovacao da credencial.");
  }
  return validityTime;
}

void RenewLogin::_run(void *) 
{
  log_scope l(log.general_logger(), debug_level, "RenewLogin::_run");
  _openbusContext->setCurrentConnection(_conn);
  _mutex.lock();
  do 
  {
    while (!_pause && _condVar.timedwait(_validityTime * 1000)) 
    {
      _mutex.unlock();
      l.log("chamando RenewLogin::renew() ...");
      _validityTime = renew();
      _mutex.lock();
    }
  } 
  while (!_stop && _condVar.wait());
  _mutex.unlock();
}

void RenewLogin::stop() 
{
  log_scope l(log.general_logger(), debug_level, "RenewLogin::stop");
  _mutex.lock();
  _stop = true;
  l.log("condVar.signal()");
  _condVar.signal();
  _mutex.unlock();
}

void RenewLogin::pause() 
{
  log_scope l(log.general_logger(), debug_level, "RenewLogin::pause");
  _mutex.lock();
  _pause = true;
  l.log("condVar.signal()");
  _condVar.signal();
  _mutex.unlock();
}

void RenewLogin::run() 
{
  log_scope l(log.general_logger(), debug_level, "RenewLogin::run");
  _mutex.lock();
  _pause = false;
  l.log("condVar.signal()");
  _condVar.signal();
  _mutex.unlock();
}

#else

RenewLogin::RenewLogin(CORBA::ORB_ptr o, Connection *c, idl_ac::AccessControl_ptr a, 
                       OpenBusContext *m, idl_ac::ValidityTime t)
  : _orb(o), _conn(c), _access_control(a), _openbusContext(m), _validityTime(t) 
{ 
  log_scope l(log.general_logger(), info_level, "RenewLogin::RenewLogin");
  _orb->dispatcher()->tm_event(this, _validityTime*1000);
}

RenewLogin::~RenewLogin() 
{
  _orb->dispatcher()->remove(this, CORBA::Dispatcher::Timer);    
}

void RenewLogin::callback(CORBA::Dispatcher *dispatcher, Event event) 
{
  _validityTime = renew(dispatcher);
  dispatcher->tm_event(this, _validityTime*1000);
}

idl_ac::ValidityTime RenewLogin::renew(CORBA::Dispatcher *dispatcher) 
{
  log_scope l(log.general_logger(), info_level, "RenewLogin::renew");
  assert(_access_control);
  idl_ac::ValidityTime validityTime = _validityTime;
  Connection *c = 0;
  try 
  {
    c = _openbusContext->getCurrentConnection();
    _openbusContext->setCurrentConnection(_conn);
    validityTime = _access_control->renew();
    _openbusContext->setCurrentConnection(c);
  } 
  catch (const CORBA::Exception &) 
  {
    l.level_vlog(warning_level, "Falha na renovacao da credencial.");
    _openbusContext->setCurrentConnection(c);
  }
  return validityTime;
}
#endif
}
