#include <connection_impl.h>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/sha.h>
#include <log.h>

namespace openbus {
Login* LoginCache::validateLogin(char* id) {
  std::string sid(id);

  /* este login est� no cache? */
  Login* login = _loginLRUCache.fetch(sid);
  if (!login) {
    /* criando uma entrada no cache para o login. */
    login = new Login;
    login->time2live = -1;
    try {
      login->loginInfo = _conn->login_registry()->getLoginInfo(id, login->encodedCallerPubKey);
    } catch (idl_ac::InvalidLogins& e) { return 0; }
    const unsigned char* buf = login->encodedCallerPubKey->get_buffer();
    login->key = d2i_PUBKEY(0, &buf, login->encodedCallerPubKey->length());
    _loginLRUCache.insert(sid, login);
  }

  /* se time2live � zero ent�o o login � inv�lido. */
  if (!login->time2live) return 0;
  
  /* se time2live � maior do que o intervalo de tempo de atualiza��o, o login � v�lido. */
  if (login->time2live > (time(0) - _timeUpdated)) return login;
  
  /* preciso consultar o barramento para validar o login. */
  else {
    /*
    ** a implementa��o atual da LRU n�o permite que o  cache  avise  ao  SDK  o  elemento 
    ** que est� sendo substitu�do, desta forma eu preciso construir uma  IdentifierSeq  
    ** antes de cada chamada getValidity(). */
    _timeUpdated = time(0);
    idl::IdentifierSeq ids(LOGINCACHE_LRU_SIZE);
    ids.length(_loginLRUCache.size());
    std::vector<std::string> keys = _loginLRUCache.get_all_keys();
    std::vector<std::string>::iterator it;
    int i = 0;
    for (it=keys.begin(); it<keys.end(); ++i, ++it) ids[i] = CORBA::string_dup((*it).c_str());
    /* validando no barramento todos os logins que est�o na cache. */
    idl_ac::ValidityTimeSeq_var validity = _conn->login_registry()->getValidity(ids);
    for (unsigned int i=0; i<validity->length(); ++i) {
      Login* l = _loginLRUCache.fetch(std::string(ids[i]));
      l->time2live = validity[i];
    }
    /* o login de interesse, ap�s atualiza��o da cache, ainda � v�lido? */
    if (login->time2live > 0) return login;
  }
  return 0;
}

#ifdef OPENBUS_SDK_MULTITHREAD
RenewLogin::RenewLogin(Connection* c, ConnectionManager* m, idl_ac::ValidityTime t)
  : _conn(c), _manager(m), _validityTime(t), _running(true), _condVar(&_mutex)
{
  log_scope l(log.general_logger(), info_level, "RenewLogin::RenewLogin");
}

RenewLogin::~RenewLogin() { }

idl_ac::ValidityTime RenewLogin::renew() {
  #ifdef OPENBUS_SDK_MULTITHREAD
  Mutex m(&(_conn->_mutex));
  #endif
  log_scope l(log.general_logger(), info_level, "RenewLogin::renew");
  assert(_conn->access_control());
  idl_ac::ValidityTime validityTime = _validityTime;
  try {
    validityTime = _conn->access_control()->renew();
  } catch (CORBA::Exception&) {
    l.level_vlog(warning_level, "Falha na renovacao da credencial.");
  }
  return validityTime;
}

void RenewLogin::_run(void*) {
  _manager->setRequester(_conn);
  _mutex.lock();
  while (_running && _condVar.timedwait(_validityTime*1000)) _validityTime = renew();
  _mutex.unlock();
}

void RenewLogin::stop() {
  _mutex.lock();
  _running = false;
  _condVar.signal();
  _mutex.unlock();
}
#else
RenewLogin::RenewLogin(Connection* c, ConnectionManager* m, idl_ac::ValidityTime t)
  : _conn(c), _manager(m), _validityTime(t) 
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
  assert(_conn->access_control());
  idl_ac::ValidityTime validityTime = _validityTime;
  try {
    validityTime = _conn->access_control()->renew();
  } catch (CORBA::Exception&) {
    l.level_vlog(warning_level, "Falha na renovacao da credencial.");
    dispatcher()->remove(this, CORBA::Dispatcher::Timer);
  }
  return validityTime;
}
#endif
}
