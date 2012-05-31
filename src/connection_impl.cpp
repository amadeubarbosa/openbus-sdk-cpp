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

  /* este login está no cache? */
  Login* login = _loginLRUCache->fetch(sid);
  if (!login) {
    /* criando uma entrada no cache para o login. */
    login = new Login;
    login->time2live = -1;
    try {
      login->loginInfo = _conn->login_registry()->getLoginInfo(id, login->encodedCallerPubKey);
    } catch (idl_ac::InvalidLogins& e) { return 0; }
    const unsigned char* buf = login->encodedCallerPubKey->get_buffer();
    login->key = d2i_PUBKEY(0, &buf, login->encodedCallerPubKey->length());
    _loginLRUCache->insert(sid, login);
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
    ids.length(_loginLRUCache->size());
    std::vector<std::string> keys = _loginLRUCache->get_all_keys();
    std::vector<std::string>::iterator it;
    int i = 0;
    for (it=keys.begin(); it<keys.end(); ++i, ++it) ids[i] = CORBA::string_dup((*it).c_str());
    /* validando no barramento todos os logins que estão na cache. */
    idl_ac::ValidityTimeSeq_var validity = _conn->login_registry()->getValidity(ids);
    for (unsigned int i=0; i<validity->length(); ++i) {
      Login* l = _loginLRUCache->fetch(std::string(ids[i]));
      l->time2live = validity[i];
    }
    /* o login de interesse, após atualização da cache, ainda é válido? */
    if (login->time2live > 0) return login;
  }
  return 0;
}

#ifdef OPENBUS_SDK_MULTITHREAD
RenewLogin::RenewLogin(Connection* c, ConnectionManager* m, idl_ac::ValidityTime t)
  : _conn(c), _manager(m), _validityTime(t), _sigINT(false) 
{
  log_scope l(log.general_logger(), info_level, "RenewLogin::RenewLogin");
}

RenewLogin::~RenewLogin() { }

idl_ac::ValidityTime RenewLogin::renew() {
  #ifdef OPENBUS_SDK_MULTITHREAD
  Mutex m(&(_conn->_mutex));
  #endif
  log_scope l(log.general_logger(), info_level, "RenewLogin::renew");
  return _conn->access_control()->renew();
}

bool RenewLogin::_sleep(unsigned int time) {
  for(unsigned int x=0; x<time; ++x) {
    if (_sigINT) {
      _sigINT = false;
      return true;
    }
    sleep(1);
  }
  return false;
}

void RenewLogin::_run(void*) {
  _manager->setRequester(_conn);
  while (true) {
    if (_sleep(_validityTime)) break;
    else _validityTime = renew();
  }
}
#else
RenewLogin::RenewLogin(Connection* c, ConnectionManager* m, idl_ac::ValidityTime t)
  : _conn(c), _manager(m), _validityTime(t) 
{ 
  log_scope l(log.general_logger(), info_level, "RenewLogin::RenewLogin");
}

void RenewLogin::callback(CORBA::Dispatcher* dispatcher, Event event) {
  _validityTime = renew();
  dispatcher->tm_event(this, _validityTime*1000);
}

idl_ac::ValidityTime RenewLogin::renew() {
  log_scope l(log.general_logger(), info_level, "RenewLogin::renew");
  _manager->setRequester(_conn);
  return _conn->access_control()->renew();
}
#endif
}
