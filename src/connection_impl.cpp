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

  /* coleta de informações pertinentes ao login id. (getLoginInfo) */
  Login* login = _id_Login->fetch(sid);
  if (!login) {
    login = new Login;
    login->time2live = -1;
    try {
      login->loginInfo = _conn->login_registry()->getLoginInfo(id, login->encodedCallerPubKey);
    } catch (idl_ac::InvalidLogins& e) { return 0; }
    const unsigned char* buf = login->encodedCallerPubKey->get_buffer();
    login->key = d2i_PUBKEY(0, &buf, login->encodedCallerPubKey->length());
    _id_Login->insert(sid, login);
  }

  /* validação do login. (getValidity) */
  if (!login->time2live) return 0;
  if (login->time2live > (time(0) - _timeUpdated)) return login;
  else {
    /*
    * Substituir a implementação da LRU para permitir que o cache avise ao SDK o  elemento 
    * que está sendo substituído, permitindo assim que eu possa manter uma IdentifierSeq e
    * ValidityTimeSeq.
    * Esforço atual: O(3n) !?
    */
    _timeUpdated = time(0);
    idl::IdentifierSeq ids(LOGINCACHE_LRU_SIZE);
    ids.length(_id_Login->size());
    std::vector<std::string> keys = _id_Login->get_all_keys();
    std::vector<std::string>::iterator it;
    int i = 0;
    for (it=keys.begin(); it<keys.end(); ++i, ++it) ids[i] = CORBA::string_dup((*it).c_str());
    idl_ac::ValidityTimeSeq_var validity = _conn->login_registry()->getValidity(ids);
    for (unsigned int i=0; i<validity->length(); ++i) {
      Login* l = _id_Login->fetch(std::string(ids[i]));
      l->time2live = validity[i];
    }
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
