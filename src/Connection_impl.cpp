#include <sstream>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/sha.h>

#include "openbus/util/AutoLock_impl.h"
#include "openbus/Connection_impl.h"
#include "openbus/log.h"

namespace openbus {
LoginCache::LoginCache(idl_ac::LoginRegistry_ptr p)
  : _login_registry(p), _loginLRUCache(LOGINCACHE_LRU_SIZE) { }

Login *LoginCache::validateLogin(char *id) {
  std::string sid(id);
  /* este login est� no cache? */
  AutoLock m(&_mutex);
  Login *login = _loginLRUCache.fetch(sid);
  if (!login) {
    m.unlock();
    login = new Login;
    login->time2live = -1;
    try {
      login->loginInfo = _login_registry->getLoginInfo(id, login->encodedCallerPubKey);
    } catch (idl_ac::InvalidLogins &e) { return 0; }
    const unsigned char *buf = login->encodedCallerPubKey->get_buffer();
    login->key = openssl::pkey
      (d2i_PUBKEY(0, &buf, login->encodedCallerPubKey->length()));
    if(!login->key)
      throw InvalidPrivateKey();
    login->timeUpdated = time(0);
    m.lock();
    _loginLRUCache.insert(sid, login);
  }
  m.unlock();
  
  /* se time2live � zero ent�o o login � inv�lido. */
  if (!login->time2live) return 0;
  
  /* se time2live � maior do que o intervalo de tempo de atualiza��o, o login � v�lido. */
  if (login->time2live > (time(0) - login->timeUpdated)) return login;
  else {
    /* preciso consultar o barramento para validar o login. */
    idl::IdentifierSeq ids(1);
    ids.length(1);
    ids[static_cast<CORBA::ULong> (0)] = CORBA::string_dup(id);
    idl_ac::ValidityTimeSeq_var validity = _login_registry->getValidity(ids);
    assert(validity->length());
    login->time2live = validity[static_cast<CORBA::ULong> (0)];
    login->timeUpdated = time(0);
    /* o login de interesse, ap�s atualiza��o da cache, ainda � v�lido? */
    if (login->time2live > 0) return login;
  }
  return 0;
}

#ifdef OPENBUS_SDK_MULTITHREAD
RenewLogin::RenewLogin(Connection *c, idl_ac::AccessControl_ptr a, ConnectionManager *m, 
  idl_ac::ValidityTime t)
  : _conn(c), _access_control(a), _manager(m), _validityTime(t), _pause(false), _stop(false), 
    _condVar(_mutex.mutex())
{
  log_scope l(log.general_logger(), info_level, "RenewLogin::RenewLogin");
}

RenewLogin::~RenewLogin() { 
  log_scope l(log.general_logger(), info_level, "RenewLogin::~RenewLogin");
}

idl_ac::ValidityTime RenewLogin::renew() {
  log_scope l(log.general_logger(), info_level, "RenewLogin::renew");
  assert(_access_control);
  idl_ac::ValidityTime validityTime = _validityTime;
  try {
    l.level_log(debug_level, "access_control()->renew()");
    validityTime = _access_control->renew();
  } catch (CORBA::Exception &) {
    l.level_vlog(warning_level, "Falha na renovacao da credencial.");
  }
  return validityTime;
}

void RenewLogin::_run(void*) {
  log_scope l(log.general_logger(), debug_level, "RenewLogin::_run");
  _manager->setRequester(_conn);
  _mutex.lock();
  while (!_stop) {
    while (!_pause &_condVar.timedwait(_validityTime*1000)) {
      _mutex.unlock();
      l.log("chamando RenewLogin::renew() ...");
      _validityTime = renew();
      _mutex.lock();
    }
    if (!_stop) {
      l.log("condVar.wait() ...");
      int r = _condVar.wait();
      assert(r);
    }
  }
  _mutex.unlock();
}

void RenewLogin::stop() {
  log_scope l(log.general_logger(), debug_level, "RenewLogin::stop");
  _mutex.lock();
  _stop = true;
  l.log("condVar.signal()");
  _condVar.signal();
  _mutex.unlock();
}

void RenewLogin::pause() {
  log_scope l(log.general_logger(), debug_level, "RenewLogin::pause");
  _mutex.lock();
  _pause = true;
  l.log("condVar.signal()");
  _condVar.signal();
  _mutex.unlock();
}

void RenewLogin::run() {
  log_scope l(log.general_logger(), debug_level, "RenewLogin::run");
  _mutex.lock();
  _pause = false;
  l.log("condVar.signal()");
  _condVar.signal();
  _mutex.unlock();
}

#else

RenewLogin::RenewLogin(Connection *c, idl_ac::AccessControl_ptr a, ConnectionManager *m, 
  idl_ac::ValidityTime t)
  : _conn(c), _access_control(a), _manager(m), _validityTime(t) 
{ 
  log_scope l(log.general_logger(), info_level, "RenewLogin::RenewLogin");
}

void RenewLogin::callback(CORBA::Dispatcher *dispatcher, Event event) {
  _validityTime = renew(dispatcher);
  dispatcher->tm_event(this, _validityTime*1000);
}

idl_ac::ValidityTime RenewLogin::renew(CORBA::Dispatcher *dispatcher) {
  log_scope l(log.general_logger(), info_level, "RenewLogin::renew");
  assert(_access_control);
  idl_ac::ValidityTime validityTime = _validityTime;
  Connection *c = 0;
  try {
    c = _manager->getRequester();
    _manager->setRequester(_conn);
    validityTime = _access_control->renew();
    _manager->setRequester(c);
  } catch (CORBA::Exception &) {
    l.level_vlog(warning_level, "Falha na renovacao da credencial.");
    _manager->setRequester(c);
  }
  return validityTime;
}
#endif
}
