#include <openbus/util/OpenSSL.h>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <openssl/x509.h>
#include <openssl/sha.h>
#include <CORBA.h>

#include <openbus/log.h>
#include <openbus/Connection.h>
#ifdef OPENBUS_SDK_MULTITHREAD
#include <openbus/util/AutoLock_impl.h>
#endif

namespace openbus {

EVP_PKEY* Connection::fetchBusKey() {
  /* armazenando a chave pública do barramento. */
  idl::OctetSeq_var _buskeyOctetSeq = _access_control->buskey();
  return openssl::byteSeq2PubKey(_buskeyOctetSeq->get_buffer(), _buskeyOctetSeq->length());
}

Connection::Connection(
  const std::string h,
  const unsigned short p,
  CORBA::ORB* orb,
  IOP::Codec* c, 
  PortableInterceptor::SlotId s1, 
  PortableInterceptor::SlotId s2,
  PortableInterceptor::SlotId s3,
  ConnectionManager* m) 
  : _host(h), _port(p), _orb(orb), _codec(c), _slotId_joinedCallChain(s1), 
  _slotId_signedCallChain(s2), _slotId_legacyCallChain(s3), _manager(m), _key(0), _renewLogin(0), 
  _loginInfo(0), _busid(0), _onInvalidLogin(0)
{
  log_scope l(log.general_logger(), info_level, "Connection::Connection");
  std::stringstream corbaloc;
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent.in()));
  corbaloc << "corbaloc::" << _host << ":" << _port << "/" << idl::BusObjectKey;
  CORBA::Object_var obj = _orb->string_to_object(corbaloc.str().c_str());
  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    _iComponent = scs::core::IComponent::_narrow(obj);
    obj = _iComponent->getFacetByName(idl_ac::AccessControlFacet);
    _access_control = idl_ac::AccessControl::_narrow(obj);
    assert(!CORBA::is_nil(_access_control.in()));
    obj = _iComponent->getFacetByName(idl_or::OfferRegistryFacet);
    _offer_registry = idl_or::OfferRegistry::_narrow(obj);
    assert(!CORBA::is_nil(_offer_registry.in()));
    obj = _iComponent->getFacetByName(idl_ac::LoginRegistryFacet);
    _login_registry = idl_ac::LoginRegistry::_narrow(obj);
    assert(!CORBA::is_nil(_login_registry.in()));
  }
	
  /* criando um par de chaves para esta conexão. */
  EVP_PKEY_CTX* ctx;
  if (!((ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, 0)) &&
    (EVP_PKEY_keygen_init(ctx) > 0) &&
    (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) > 0) &&
    (EVP_PKEY_keygen(ctx, &_key) > 0))
  ) assert(0);

  _loginCache = std::auto_ptr<LoginCache> (new LoginCache(_login_registry));
}

Connection::~Connection() { 
  log_scope l(log.general_logger(), info_level, "Connection::~Connection");
  _logout(true);
  #ifdef OPENBUS_SDK_MULTITHREAD
  _renewLogin->stop();
  _renewLogin->wait();
  #endif
}

void Connection::loginByPassword(const char* entity, const char* password) {
  log_scope l(log.general_logger(), info_level, "Connection::loginByPassword");
  AutoLock m(&_mutex);
  if (_login()) throw AlreadyLoggedIn();
  m.unlock();
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  
  /* representação do password em uma cadeia de bytes. */
  CORBA::ULong lenpass = static_cast<CORBA::ULong> (strlen(password));
  idl::OctetSeq_var passwordOctetSeq = new idl::OctetSeq(lenpass, lenpass,
    (CORBA::Octet*) CORBA::string_dup(password));
  loginAuthenticationInfo.data = passwordOctetSeq;
  
  /* representação da minha chave em uma cadeia de bytes. */
  size_t len;
  unsigned char* bufKey = openssl::PubKey2byteSeq(_key, len);
  
  /* criando uma hash da minha chave. */
  SHA256(bufKey, len, loginAuthenticationInfo.hash);

  /* CDR da estrura LoginAuthenticationInfo que foi montada acima. */
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo= _codec->encode_value(any);

  EVP_PKEY* buskey = fetchBusKey();

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública do barramento. */
  unsigned char* encrypted = openssl::encrypt(buskey, encodedLoginAuthenticationInfo->get_buffer(), 
    encodedLoginAuthenticationInfo->length());
  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted, 256);
  OPENSSL_free(encrypted);
    
  idl_ac::ValidityTime validityTime;
  idl::OctetSeq_var keyOctetSeq = new idl::OctetSeq(len, len,static_cast<CORBA::Octet*> (bufKey));
  idl_ac::LoginInfo* loginInfo;
  try {
    loginInfo = _access_control->loginByPassword(entity, keyOctetSeq, encryptedBlock, validityTime);
  } catch (idl_ac::WrongEncoding&) {
    throw idl::services::ServiceFailure();
  }

  m.lock();
  if (_login()) throw AlreadyLoggedIn();
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _busid = _access_control->busid();
  //[todo] leak
  _buskey = buskey;

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
  _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

void Connection::loginByCertificate(const char* entity, const idl::OctetSeq& privKey) {
  log_scope l(log.general_logger(), info_level, "Connection::loginByCertificate");
  AutoLock m(&_mutex);
  if (_login()) throw AlreadyLoggedIn();
  m.unlock();
  
  idl::EncryptedBlock challenge;
  idl_ac::LoginProcess_var loginProcess;
  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    loginProcess = _access_control->startLoginByCertificate(entity,challenge);
  }
  
  EVP_PKEY* privateKey = openssl::byteSeq2PrvKey(privKey.get_buffer(), privKey.length());
  
  /* decifrar o desafio usando a chave privada do usuário. */
  unsigned char* secret = openssl::decrypt(privateKey, (unsigned char*) challenge, 256);
  if (!secret) throw CorruptedPrivateKey();
  
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  CORBA::ULong lensrc = static_cast<CORBA::ULong> (strlen((const char*) secret));
  idl::OctetSeq_var secretOctetSeq = new idl::OctetSeq(lensrc, lensrc, 
    (CORBA::Octet*) CORBA::string_dup((const char*) secret));
  OPENSSL_free(secret);
  loginAuthenticationInfo.data = secretOctetSeq;

  /* representação da minha chave em uma cadeia de bytes. */
  size_t len;
  unsigned char* bufKey = openssl::PubKey2byteSeq(_key, len);
  SHA256(bufKey, len, loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo = _codec->encode_value(any);

  EVP_PKEY* buskey;
  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    buskey = fetchBusKey();
  }

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública do barramento. */
  unsigned char* encrypted = openssl::encrypt(buskey, encodedLoginAuthenticationInfo->get_buffer(), 
    encodedLoginAuthenticationInfo->length());
  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted, 256);
  OPENSSL_free(encrypted);

  idl::OctetSeq_var keyOctetSeq = new idl::OctetSeq(len, len, static_cast<CORBA::Octet*> (bufKey));

  interceptors::IgnoreInterceptor _i(_piCurrent);
  idl_ac::ValidityTime validityTime;    
  idl_ac::LoginInfo* loginInfo;
  try {
    loginInfo = loginProcess->login(keyOctetSeq, encryptedBlock, validityTime);
  } catch (idl_ac::AccessDenied&) {
    throw WrongPrivateKey();
  } catch (idl_ac::WrongEncoding&) {
    throw idl::services::ServiceFailure();
  }
  
  m.lock();
  if (_login()) throw AlreadyLoggedIn();
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _busid = _access_control->busid();
  //[todo] leak
  _buskey = buskey;

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
  _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

std::pair <idl_ac::LoginProcess*, const unsigned char*> Connection::startSharedAuth() {
  log_scope l(log.general_logger(), info_level, "Connection::startSharedAuth");
  unsigned char* challenge = new unsigned char[256];
  idl_ac::LoginProcess* loginProcess = _access_control->startLoginBySharedAuth(challenge);
  const unsigned char* secret = openssl::decrypt(_key, challenge, 256);
  return std::make_pair(loginProcess, secret);
}
  
void Connection::loginBySharedAuth(idl_ac::LoginProcess* loginProcess, const unsigned char* secret){
  log_scope l(log.general_logger(), info_level, "Connection::loginBySharedAuth");
  AutoLock m(&_mutex);
  if (_login()) throw AlreadyLoggedIn();
  m.unlock();
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  CORBA::ULong lensrc = static_cast<CORBA::ULong> (strlen((const char*) secret));
  idl::OctetSeq_var secretOctetSeq = new idl::OctetSeq(lensrc, lensrc, 
    (CORBA::Octet*) CORBA::string_dup((const char*) secret));
  loginAuthenticationInfo.data = secretOctetSeq;
  
  /* representação da minha chave em uma cadeia de bytes. */
  size_t len;
  unsigned char* bufKey = openssl::PubKey2byteSeq(_key, len);
  SHA256(bufKey, len, loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo = _codec->encode_value(any);

  EVP_PKEY* buskey = fetchBusKey();

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública do barramento. */
  unsigned char* encrypted = openssl::encrypt(buskey, encodedLoginAuthenticationInfo->get_buffer(), 
    encodedLoginAuthenticationInfo->length());
  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted, 256);
  OPENSSL_free(encrypted);

  idl::OctetSeq_var keyOctetSeq = new idl::OctetSeq(len,len, static_cast<CORBA::Octet*> (bufKey));

  idl_ac::ValidityTime validityTime;
  idl_ac::LoginInfo* loginInfo; 
  try {
    loginInfo = loginProcess->login(keyOctetSeq, encryptedBlock, validityTime);
  } catch (idl_ac::AccessDenied&) {
    throw WrongSecret();
  } catch (idl_ac::WrongEncoding&) {
    throw idl::services::ServiceFailure();
  }

  m.lock();
  if (_login()) throw AlreadyLoggedIn();
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _busid = _access_control->busid();
  //[todo] leak
  _buskey = buskey;

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
  _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

bool Connection::_logout(bool local) {
  bool sucess = false;
  AutoLock m(&_mutex);
  if (_login()) {
    m.unlock();
    #ifdef OPENBUS_SDK_MULTITHREAD
    _renewLogin->pause();
    #else
    _orb->dispatcher()->remove(_renewLogin.get(), CORBA::Dispatcher::Timer);
    #endif
    m.lock();
    _loginInfo.reset();
    m.unlock();
    if (!local) {
      Connection* c = 0;
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
  } else sucess = false;
  CORBA::String_var busid = CORBA::string_dup(_busid);
  m.unlock();
  if (_manager->getDispatcher(busid)) _manager->clearDispatcher(busid);
  m.lock();
  _busid = 0;
  _buskey = 0;
  m.unlock();
  return sucess;    
}

bool Connection::logout() { 
  log_scope l(log.general_logger(), info_level, "Connection::logout");
  return _logout(false); 
}

CallerChain* Connection::getCallerChain() {
  log_scope l(log.general_logger(), info_level, "Connection::getCallerChain");
  CORBA::Any* sigCallChainAny = _piCurrent->get_slot(_slotId_signedCallChain);
  CallerChain* callerChain = 0;
  idl_ac::CallChain callChain;
  idl_cr::SignedCallChain sigCallChain;
  if (*sigCallChainAny >>= sigCallChain) {
    CORBA::Any_var callChainAny = _codec->decode_value(sigCallChain.encoded,
      idl_ac::_tc_CallChain);
    *callChainAny >>= callChain;
    AutoLock m(&_mutex);
    callerChain = new CallerChain(_busid, callChain.originators, callChain.caller, sigCallChain);
  } else {
    CORBA::Any_var legacyChainAny = _piCurrent->get_slot(_slotId_legacyCallChain);
    if (legacyChainAny >>= callChain)
      callerChain = new CallerChain(0, callChain.originators, callChain.caller);
    else return 0;
  }
  return callerChain;
}

void Connection::joinChain(CallerChain* chain) {
  log_scope l(log.general_logger(), info_level, "Connection::joinChain");
  CORBA::Any sigCallChainAny;
  sigCallChainAny <<= *(chain->signedCallChain());
  _piCurrent->set_slot(_slotId_joinedCallChain, sigCallChainAny);
}

void Connection::exitChain() {
  log_scope l(log.general_logger(), info_level, "Connection::exitChain");
  CORBA::Any any;
  _piCurrent->set_slot(_slotId_joinedCallChain, any);    
}

CallerChain* Connection::getJoinedChain() {
  log_scope l(log.general_logger(), info_level, "Connection::getJoinedChain");
  CORBA::Any_var sigCallChainAny=_piCurrent->get_slot(_slotId_joinedCallChain);
  idl_cr::SignedCallChain sigCallChain;
  if (*sigCallChainAny >>= sigCallChain) {
    CORBA::Any_var callChainAny = _codec->decode_value(sigCallChain.encoded, idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain) return new CallerChain(callChain.target, callChain.originators, 
      callChain.caller, sigCallChain);
    else return 0;
  } else return 0;
}

void Connection::onInvalidLogin(Connection::InvalidLoginCallback_ptr p) { 
  AutoLock m(&_mutex);
  _onInvalidLogin = p; 
}

Connection::InvalidLoginCallback_ptr Connection::onInvalidLogin() { 
  AutoLock m(&_mutex);
  return _onInvalidLogin; 
}

}
