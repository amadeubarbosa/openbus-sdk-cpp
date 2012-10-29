#include <openbus/util/OpenSSL.h>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <openssl/x509.h>
#include <openssl/sha.h>
#include <CORBA.h>

#include "openbus/log.h"
#include "openbus/Connection.h"
#include "openbus/util/AutoLock_impl.h"

namespace openbus {

const size_t RSASize = 2048;

openssl::pkey Connection::fetchBusKey() {
  /* armazenando a chave p�blica do barramento. */
  idl::OctetSeq_var _buskeyOctetSeq = _access_control->buskey();
  return openssl::byteSeq2PubKey(_buskeyOctetSeq->get_buffer(), _buskeyOctetSeq->length());
}

void Connection::checkBusid() const {
  if (strcmp(_busid, _access_control->busid())) throw BusChanged();
}

Connection::Connection(
  const std::string h,
  const unsigned short p,
  CORBA::ORB *orb,
  IOP::Codec *c, 
  PortableInterceptor::SlotId s1, 
  PortableInterceptor::SlotId s2,
  PortableInterceptor::SlotId s3,
  PortableInterceptor::SlotId s4,
  ConnectionManager *m, std::vector<std::string> props) 
  : _host(h), _port(p), _orb(orb), _codec(c), _slotId_joinedCallChain(s1), 
  _slotId_signedCallChain(s2), _slotId_legacyCallChain(s3), _slotId_receiveConnection(s4), 
  _renewLogin(0), _loginInfo(0), 
  _onInvalidLogin(0), _state(UNLOGGED), _manager(m), _busid(0), _legacyDelegate(CALLER)
{
  log_scope l(log.general_logger(), info_level, "Connection::Connection");
  std::stringstream corbaloc;
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent.in()));
  corbaloc << "corbaloc::" << _host << ":" << _port << "/" << idl::BusObjectKey;
  CORBA::Object_var obj;
  try {
    obj = _orb->string_to_object(corbaloc.str().c_str());
  } catch (CORBA::BAD_PARAM &){
    throw InvalidBusAddress();
  }
  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    _iComponent = scs::core::IComponent::_narrow(obj);
    if (CORBA::is_nil(_iComponent)) throw InvalidBusAddress();
    obj = _iComponent->getFacet(idl_ac::_tc_AccessControl->id());
    _access_control = idl_ac::AccessControl::_narrow(obj);
    assert(!CORBA::is_nil(_access_control.in()));
    obj = _iComponent->getFacet(idl_or::_tc_OfferRegistry->id());
    _offer_registry = idl_or::OfferRegistry::_narrow(obj);
    assert(!CORBA::is_nil(_offer_registry.in()));
    obj = _iComponent->getFacet(idl_ac::_tc_LoginRegistry->id());
    _login_registry = idl_ac::LoginRegistry::_narrow(obj);
    assert(!CORBA::is_nil(_login_registry.in()));
  }
	
  /* criando um par de chaves para esta conex�o. */
  {
    openssl::pkey_ctx ctx ( EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, 0) );
    if (!ctx) throw InvalidPrivateKey();
    if(EVP_PKEY_keygen_init(ctx.get()) <= 0) throw InvalidPrivateKey();
    if(EVP_PKEY_CTX_set_rsa_keygen_bits(ctx.get(), RSASize) <= 0) throw InvalidPrivateKey();
    EVP_PKEY* key = 0;
    if(EVP_PKEY_keygen(ctx.get(), &key) <= 0 || !key) throw InvalidPrivateKey();
    _key = openssl::pkey(key);
  }

  _loginCache = std::auto_ptr<LoginCache> (new LoginCache(_login_registry));
  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    _busid = _access_control->busid();
    _buskey = fetchBusKey();
  }
  
  for (std::vector<std::string>::const_iterator it = props.begin(); it != props.end(); ++it)
    if (*it == "legacydelegate")
      if (++it != props.end())
        if (*it == "originator") _legacyDelegate = ORIGINATOR;
        else if (*it == "caller") _legacyDelegate = CALLER;
        else throw InvalidPropertyValue("legacydelegate", *it);
      else throw InvalidPropertyValue("legacydelegate", *it);
}

Connection::~Connection() { 
  log_scope l(log.general_logger(), info_level, "Connection::~Connection");
  _logout(true);
  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) {
    _renewLogin->stop();
    _renewLogin->wait();
  }
  #endif
}

void Connection::loginByPassword(const char *entity, const char *password) {
  log_scope l(log.general_logger(), info_level, "Connection::loginByPassword");
  AutoLock m(&_mutex);
  bool state = _state;
  m.unlock();
  if (state == LOGGED) throw AlreadyLoggedIn();
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  checkBusid();  
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  
  /* representa��o do password em uma cadeia de bytes. */
  CORBA::ULong passSize = static_cast<CORBA::ULong> (strlen(password));
  idl::OctetSeq_var passOctetSeq = new idl::OctetSeq(passSize, passSize,
    (CORBA::Octet*) CORBA::string_dup(password));
  loginAuthenticationInfo.data = passOctetSeq;
  
  /* representa��o da minha chave em uma cadeia de bytes. */
  CORBA::OctetSeq bufKey = openssl::PubKey2byteSeq(_key);
  
  /* criando uma hash da minha chave. */
  SHA256(bufKey.get_buffer(), bufKey.length(), loginAuthenticationInfo.hash);

  /* CDR da estrura LoginAuthenticationInfo que foi montada acima. */
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo= _codec->encode_value(any);

  /* cifrando a estrutura LoginAuthenticationInfo com a chave p�blica do barramento. */
  CORBA::OctetSeq encrypted = openssl::encrypt(_buskey, 
                                               encodedLoginAuthenticationInfo->get_buffer(), 
                                               encodedLoginAuthenticationInfo->length());
  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted.get_buffer(), idl::EncryptedBlockSize);
    
  idl_ac::ValidityTime validityTime;
  idl_ac::LoginInfo *loginInfo;
  try {
    loginInfo = _access_control->loginByPassword(entity, bufKey, encryptedBlock, validityTime);
  } catch (idl_ac::WrongEncoding&) {
    throw idl::services::ServiceFailure();
  }

  m.lock();
  if (_state == LOGGED) throw AlreadyLoggedIn();
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _state = LOGGED;

  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) {
    m.unlock();
    _renewLogin->run();
    m.lock();
  } else {
    _renewLogin = std::auto_ptr<RenewLogin> 
      (new RenewLogin(this, _access_control, _manager, validityTime));
    m.unlock();
    _renewLogin->start();
    m.lock();
  }
  #else
  assert(!_renewLogin.get());
  _renewLogin = std::auto_ptr<RenewLogin> 
    (new RenewLogin(_orb, this, _access_control, _manager, validityTime));
  #endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

void Connection::loginByCertificate(const char *entity, const idl::OctetSeq &privKey) {
  log_scope l(log.general_logger(), info_level, "Connection::loginByCertificate");
  AutoLock m(&_mutex);
  bool state = _state;
  m.unlock();
  if (state == LOGGED) throw AlreadyLoggedIn();
  
  idl::EncryptedBlock challenge;
  idl_ac::LoginProcess_var loginProcess;
  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    checkBusid();
    loginProcess = _access_control->startLoginByCertificate(entity,challenge);
  }
  
  openssl::pkey privateKey = openssl::byteSeq2PrvKey(privKey.get_buffer(), privKey.length());
  
  /* decifrar o desafio usando a chave privada do usu�rio. */
  CORBA::OctetSeq secret = openssl::decrypt(privateKey, (unsigned char*) challenge, 
                                            idl::EncryptedBlockSize);

  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  loginAuthenticationInfo.data = secret;
  
  /* representa��o da minha chave em uma cadeia de bytes. */
  CORBA::OctetSeq bufKey = openssl::PubKey2byteSeq(_key);
  SHA256(bufKey.get_buffer(), bufKey.length(), loginAuthenticationInfo.hash);
  
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo = _codec->encode_value(any);
  
  /* cifrando a estrutura LoginAuthenticationInfo com a chave p�blica do barramento. */
  CORBA::OctetSeq encrypted = openssl::encrypt(_buskey, encodedLoginAuthenticationInfo->get_buffer(), 
                                               encodedLoginAuthenticationInfo->length());
  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted.get_buffer(), idl::EncryptedBlockSize);
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  idl_ac::ValidityTime validityTime;    
  idl_ac::LoginInfo *loginInfo;
  try {
    loginInfo = loginProcess->login(bufKey, encryptedBlock, validityTime);
  } catch (idl_ac::WrongEncoding&) {
    throw idl_ac::AccessDenied();
  }
  
  m.lock();
  if (_state == LOGGED) throw AlreadyLoggedIn();
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _state = LOGGED;

  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) {
    m.unlock();
    _renewLogin->run();
    m.lock();
  } else {
    _renewLogin = std::auto_ptr<RenewLogin> (new RenewLogin(this, _access_control, 
                                                            _manager, validityTime));
    m.unlock();
    _renewLogin->start();
    m.lock();
  }
  #else
  assert(!_renewLogin.get());
  _renewLogin = std::auto_ptr<RenewLogin> 
    (new RenewLogin(_orb, this, _access_control, _manager, validityTime));
  #endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

std::pair <idl_ac::LoginProcess_ptr, idl::OctetSeq> Connection::startSharedAuth() {
  log_scope l(log.general_logger(), info_level, "Connection::startSharedAuth");
  idl::EncryptedBlock challenge;
  Connection *c = 0;
  idl_ac::LoginProcess_ptr loginProcess;
  try {
    c = _manager->getRequester();
    _manager->setRequester(this);
    loginProcess = _access_control->startLoginBySharedAuth(challenge);
    _manager->setRequester(c);
  } catch (...) {
    _manager->setRequester(c);
    throw;
  }
  CORBA::OctetSeq secretBuf = openssl::decrypt(_key, challenge, idl::EncryptedBlockSize);
  return std::make_pair(loginProcess, secretBuf);
}
  
void Connection::loginBySharedAuth(idl_ac::LoginProcess_ptr loginProcess, 
  const idl::OctetSeq &secret)
{
  log_scope l(log.general_logger(), info_level, "Connection::loginBySharedAuth");
  AutoLock m(&_mutex);
  bool state = _state;
  m.unlock();
  if (state == LOGGED) throw AlreadyLoggedIn();
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  checkBusid();  
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  loginAuthenticationInfo.data = secret;
  
  /* representa��o da minha chave em uma cadeia de bytes. */
  CORBA::OctetSeq bufKey = openssl::PubKey2byteSeq(_key);
  SHA256(bufKey.get_buffer(), bufKey.length(), loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo = _codec->encode_value(any);

  /* cifrando a estrutura LoginAuthenticationInfo com a chave p�blica do barramento. */
  CORBA::OctetSeq encrypted = openssl::encrypt(_buskey, encodedLoginAuthenticationInfo->get_buffer(), 
    encodedLoginAuthenticationInfo->length());
  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted.get_buffer(), idl::EncryptedBlockSize);

  idl_ac::ValidityTime validityTime;
  idl_ac::LoginInfo *loginInfo; 
  try {
    loginInfo = loginProcess->login(bufKey, encryptedBlock, validityTime);
  } catch (idl_ac::WrongEncoding&) {
    throw idl_ac::AccessDenied();
  }

  m.lock();
  if (_state == LOGGED) throw AlreadyLoggedIn();
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _state = LOGGED;

  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) {
    m.unlock();
    _renewLogin->run();
    m.lock();
  } else {
    _renewLogin = std::auto_ptr<RenewLogin> (new RenewLogin(this, _access_control, 
                                                            _manager, validityTime));
    m.unlock();
    _renewLogin->start();
    m.lock();
  }
  #else
  assert(!_renewLogin.get());
  _renewLogin = std::auto_ptr<RenewLogin> (new RenewLogin(_orb, this, _access_control, 
                                                          _manager, validityTime));
  #endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

bool Connection::_logout(bool local) {
  bool sucess = false;
  AutoLock m(&_mutex);
  bool state = _state;
  m.unlock();
  if (state == LOGGED) {
    #ifdef OPENBUS_SDK_MULTITHREAD
    _renewLogin->pause();
    #else
    _renewLogin.reset();
    #endif
    if (!local) {
      Connection *c = 0;
      try {
        c = _manager->getRequester();
        _manager->setRequester(this);
        _access_control->logout();
        sucess = true;
        _manager->setRequester(c);
      } catch (...) { 
        sucess = false; 
        _manager->setRequester(c);
      }
    }
    m.lock();
    _loginInfo.reset();
    _state = UNLOGGED;
    m.unlock();
  } else if (state == INVALID) {
    m.lock();
    _loginInfo.reset();
    _state = UNLOGGED;
    m.unlock();
  } else return false;
  return sucess;    
}

bool Connection::logout() { 
  log_scope l(log.general_logger(), info_level, "Connection::logout");
  return _logout(false); 
}

CallerChain Connection::getCallerChain() {
  log_scope l(log.general_logger(), info_level, "Connection::getCallerChain");
  CORBA::Any_var connectionAddrAny = _piCurrent->get_slot(_slotId_receiveConnection);
  idl::OctetSeq connectionAddrOctetSeq;
  Connection *c = 0;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) {
    assert(connectionAddrOctetSeq.length() == sizeof(Connection*));
    std::memcpy(&c, connectionAddrOctetSeq.get_buffer(), sizeof(Connection*));
    if (c != this) return CallerChain();
  } else return CallerChain();

  CORBA::Any *sigCallChainAny = _piCurrent->get_slot(_slotId_signedCallChain);
  idl_ac::CallChain callChain;
  idl_cr::SignedCallChain sigCallChain;
  if (*sigCallChainAny >>= sigCallChain) {
    CORBA::Any_var callChainAny = _codec->decode_value(sigCallChain.encoded,
                                                       idl_ac::_tc_CallChain);
    *callChainAny >>= callChain;
    return CallerChain(_busid, callChain.originators, callChain.caller, sigCallChain);
  } else {
    CORBA::Any_var legacyChainAny = _piCurrent->get_slot(_slotId_legacyCallChain);
    if (legacyChainAny >>= callChain)
      return CallerChain(0, callChain.originators, callChain.caller);
    else return CallerChain();
  }
  return CallerChain();
}

void Connection::joinChain(CallerChain const& chain) {
  log_scope l(log.general_logger(), info_level, "Connection::joinChain");
  CORBA::Any sigCallChainAny;
  sigCallChainAny <<= *(chain.signedCallChain());
  _piCurrent->set_slot(_slotId_joinedCallChain, sigCallChainAny);
}

void Connection::exitChain() {
  log_scope l(log.general_logger(), info_level, "Connection::exitChain");
  CORBA::Any any;
  _piCurrent->set_slot(_slotId_joinedCallChain, any);    
}

CallerChain Connection::getJoinedChain() {
  log_scope l(log.general_logger(), info_level, "Connection::getJoinedChain");
  CORBA::Any_var sigCallChainAny=_piCurrent->get_slot(_slotId_joinedCallChain);
  idl_cr::SignedCallChain sigCallChain;
  if (*sigCallChainAny >>= sigCallChain) {
    CORBA::Any_var callChainAny = _codec->decode_value(sigCallChain.encoded, idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain) return CallerChain(callChain.target, callChain.originators, 
      callChain.caller, sigCallChain);
    else return CallerChain();
  } else return CallerChain();
}

void Connection::onInvalidLogin(Connection::InvalidLoginCallback_t p) { 
  AutoLock m(&_mutex);
  _onInvalidLogin = p; 
}

Connection::InvalidLoginCallback_t Connection::onInvalidLogin() { 
  AutoLock m(&_mutex);
  return _onInvalidLogin; 
}

const idl_ac::LoginInfo *Connection::login() {
  AutoLock m(&_mutex);
  if (_state == INVALID) return 0;
  else return _loginInfo.get(); 
}

const char *Connection::busid() { 
  AutoLock m(&_mutex);
  if (_state == INVALID) return 0;
  else return CORBA::string_dup(_busid); 
}

}