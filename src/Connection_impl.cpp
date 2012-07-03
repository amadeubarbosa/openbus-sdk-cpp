#include <sstream>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/sha.h>

#include "openbus/Connection_impl.h"
#include "openbus/log.h"

namespace openbus {
LoginCache::LoginCache(idl_ac::LoginRegistry_ptr p)
  : _login_registry(p), _loginLRUCache(LOGINCACHE_LRU_SIZE) { }

Login* LoginCache::validateLogin(char* id) {
  std::string sid(id);

  /* este login está no cache? */
  Login* login = _loginLRUCache.fetch(sid);
  if (!login) {
    /* criando uma entrada no cache para o login. */
    login = new Login;
    login->time2live = -1;
    try {
      login->loginInfo = _login_registry->getLoginInfo(id, login->encodedCallerPubKey);
    } catch (idl_ac::InvalidLogins& e) { return 0; }
    const unsigned char* buf = login->encodedCallerPubKey->get_buffer();
    login->key = d2i_PUBKEY(0, &buf, login->encodedCallerPubKey->length());
    _loginLRUCache.insert(sid, login);
  }

  /* se time2live é zero então o login é inválido. */
  if (!login->time2live) return 0;
  
  /* se time2live é maior do que o intervalo de tempo de atualização, o login é válido. */
  if (login->time2live > (time(0) - _timeUpdated)) return login;
  
  /* preciso consultar o barramento para validar o login. */
  else {
    /*
    ** a implementação atual da LRU não permite que o  cache  avise  ao  SDK  o  elemento 
    ** que está sendo substituído, desta forma eu preciso construir uma  IdentifierSeq  
    ** antes de cada chamada getValidity(). */
    _timeUpdated = time(0);
    idl::IdentifierSeq ids(LOGINCACHE_LRU_SIZE);
    ids.length(_loginLRUCache.size());
    std::vector<std::string> keys = _loginLRUCache.get_all_keys();
    std::vector<std::string>::iterator it;
    int i = 0;
    for (it=keys.begin(); it<keys.end(); ++i, ++it) ids[i] = CORBA::string_dup((*it).c_str());
    /* validando no barramento todos os logins que estão na cache. */
    idl_ac::ValidityTimeSeq_var validity = _login_registry->getValidity(ids);
    for (unsigned int i=0; i<validity->length(); ++i) {
      Login* l = _loginLRUCache.fetch(std::string(ids[i]));
      l->time2live = validity[i];
    }
    /* o login de interesse, após atualização da cache, ainda é válido? */
    if (login->time2live > 0) return login;
  }
  return 0;
}

#ifdef OPENBUS_SDK_MULTITHREAD
RenewLogin::RenewLogin(Connection* c, idl_ac::AccessControl_ptr a, ConnectionManager* m, 
  idl_ac::ValidityTime t)
  : _conn(c), _access_control(a), _manager(m), _validityTime(t), _pause(false), _stop(false), 
  _condVar(&_mutex)
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
  } catch (CORBA::Exception&) {
    l.level_vlog(warning_level, "Falha na renovacao da credencial.");
  }
  return validityTime;
}

void RenewLogin::_run(void*) {
  log_scope l(log.general_logger(), debug_level, "RenewLogin::_run");
  _manager->setRequester(_conn);
  _mutex.lock();
  while (!_stop) {
    while (!_pause && _condVar.timedwait(_validityTime*1000)) {
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

RenewLogin::RenewLogin(Connection* c, idl_ac::AccessControl_ptr a, ConnectionManager* m, 
  idl_ac::ValidityTime t)
  : _conn(c), _access_control(a), _manager(m), _validityTime(t) 
{ 
  log_scope l(log.general_logger(), info_level, "RenewLogin::RenewLogin");
}

void RenewLogin::callback(CORBA::Dispatcher* dispatcher, Event event) {
  _validityTime = renew(dispatcher);
  dispatcher->tm_event(this, _validityTime*1000);
}

idl_ac::ValidityTime RenewLogin::renew(CORBA::Dispatcher* dispatcher) {
  log_scope l(log.general_logger(), info_level, "RenewLogin::renew");
  _manager->setRequester(_conn);
  assert(_access_control);
  idl_ac::ValidityTime validityTime = _validityTime;
  try {
    validityTime = _access_control->renew();
  } catch (CORBA::Exception&) {
    l.level_vlog(warning_level, "Falha na renovacao da credencial.");
    dispatcher->remove(this, CORBA::Dispatcher::Timer);
  }
  return validityTime;
}
#endif
}
