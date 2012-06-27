#include <connection.h>
#include <connection_impl.h>
#include <util/openssl.h>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <log.h>
#include <openssl/x509.h>
#include <openssl/sha.h>

namespace openbus {

void Connection::fetchBusKey() {
  /* armazenando a chave pública do barramento. */
  idl::OctetSeq_var _buskeyOctetSeq = _access_control->buskey();
  _busKey = openssl::byteSeq2PubKey(_buskeyOctetSeq->get_buffer(), _buskeyOctetSeq->length());
}

Connection::Connection(
  const std::string h,
  const unsigned short p,
  CORBA::ORB* orb,
  const interceptors::ORBInitializer* ini,
  ConnectionManager* m) 
  throw(CORBA::Exception)
  : _host(h), _port(p), _orb(orb), _orbInitializer(ini), _renewLogin(0), _busid(0), 
  _onInvalidLogin(0), _manager(m)
{
  log_scope l(log.general_logger(), info_level, "Connection::Connection");
  std::stringstream corbaloc;
  corbaloc << "corbaloc::" << _host << ":" << _port << "/" << idl::BusObjectKey;
  CORBA::Object_var obj = _orb->string_to_object(corbaloc.str().c_str());
  assert(!CORBA::is_nil(obj));
  _clientInterceptor = _orbInitializer->clientInterceptor();
  _serverInterceptor = _orbInitializer->serverInterceptor();
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent));
  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    _iComponent = scs::core::IComponent::_narrow(obj);
    obj = _iComponent->getFacetByName(idl_ac::AccessControlFacet);
    assert(!CORBA::is_nil(obj));
    _access_control = idl_ac::AccessControl::_narrow(obj);
    obj = _iComponent->getFacetByName(idl_or::OfferRegistryFacet);
    assert(!CORBA::is_nil(obj));
    _offer_registry = idl_or::OfferRegistry::_narrow(obj);
    obj = _iComponent->getFacetByName(idl_ac::LoginRegistryFacet);
    assert(!CORBA::is_nil(obj));
    _login_registry = idl_ac::LoginRegistry::_narrow(obj);
    _loginInfo.reset();
  }
	
  /* criando um par de chaves para esta conexão. */
  _key = 0;
  EVP_PKEY_CTX* ctx;
  if (!((ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, 0)) &&
    (EVP_PKEY_keygen_init(ctx) > 0) &&
    (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) > 0) &&
    (EVP_PKEY_keygen(ctx, &_key) > 0))
  ) assert(0);

  _loginCache = std::auto_ptr<LoginCache> (new LoginCache(this));
}

Connection::~Connection() { 
  _logout(true);
  _renewLogin->stop();
  _renewLogin->wait();
}

void Connection::loginByPassword(const char* entity, const char* password)
  throw (AlreadyLoggedIn, AccessDenied, idl::services::ServiceFailure, CORBA::Exception) 
{
  log_scope l(log.general_logger(), info_level, "Connection::loginByPassword");
  #ifdef OPENBUS_SDK_MULTITHREAD
  Mutex m(&_mutex);
  #endif
  if (login()) throw AlreadyLoggedIn();
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  
  /* representação do password em uma cadeia de bytes. */
  idl::OctetSeq_var passwordOctetSeq = new idl::OctetSeq(
    static_cast<CORBA::ULong> (strlen(password)),
    static_cast<CORBA::ULong> (strlen(password)),
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
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo= _orbInitializer->codec()->encode_value(any);

  fetchBusKey();

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública do barramento. */
  unsigned char* encrypted = openssl::encrypt(_busKey, encodedLoginAuthenticationInfo->get_buffer(), 
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

  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _busid = _access_control->busid();

  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) _renewLogin->run();
  else {
    _renewLogin = std::auto_ptr<RenewLogin> (new RenewLogin(this, _manager, validityTime));
    _renewLogin->start();
  }
  #else
  _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif
}

void Connection::loginByCertificate(const char* entity, const idl::OctetSeq& privKey)
  throw (CorruptedPrivateKey, WrongPrivateKey, AlreadyLoggedIn, idl_ac::MissingCertificate,
  idl::services::ServiceFailure, CORBA::Exception)
{
  log_scope l(log.general_logger(), info_level, "Connection::loginByCertificate");
  #ifdef OPENBUS_SDK_MULTITHREAD
  Mutex m(&_mutex);
  #endif
  if (login()) throw AlreadyLoggedIn();
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
  idl::OctetSeq_var secretOctetSeq = new idl::OctetSeq(
    static_cast<CORBA::ULong> (strlen((const char*) secret)),
    static_cast<CORBA::ULong> (strlen((const char*) secret)),
    (CORBA::Octet*) CORBA::string_dup((const char*) secret));
  OPENSSL_free(secret);
  loginAuthenticationInfo.data = secretOctetSeq;

  /* representação da minha chave em uma cadeia de bytes. */
  size_t len;
  unsigned char* bufKey = openssl::PubKey2byteSeq(_key, len);
  SHA256(bufKey, len, loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo = _orbInitializer->codec()->encode_value(any);

  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    fetchBusKey();
  }

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública do barramento. */
  unsigned char* encrypted = openssl::encrypt(_busKey, encodedLoginAuthenticationInfo->get_buffer(), 
    encodedLoginAuthenticationInfo->length());
  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted, 256);
  OPENSSL_free(encrypted);

  idl::OctetSeq_var keyOctetSeq = new idl::OctetSeq(len, len,static_cast<CORBA::Octet*> (bufKey));

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
  
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _busid = _access_control->busid();

  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) _renewLogin->run();
  else {
    _renewLogin = std::auto_ptr<RenewLogin> (new RenewLogin(this, _manager, validityTime));
    _renewLogin->start();
  }
  #else
  _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif
}

std::pair <idl_ac::LoginProcess*, const unsigned char*> Connection::startSingleSignOn() 
  throw (idl::services::ServiceFailure, CORBA::Exception)
{
  log_scope l(log.general_logger(), info_level, "Connection::startSingleSignOn");
  #ifdef OPENBUS_SDK_MULTITHREAD
  Mutex m(&_mutex);
  #endif
  unsigned char* challenge = new unsigned char[256];
  idl_ac::LoginProcess* loginProcess = _access_control->startLoginBySingleSignOn(challenge);

  const unsigned char* secret = openssl::decrypt(_key, (unsigned char*) challenge, 256);
  return std::make_pair(loginProcess, secret);
}
  
void Connection::loginBySingleSignOn(idl_ac::LoginProcess* loginProcess, const unsigned char* secret)
	throw(WrongSecret, InvalidLoginProcess, AlreadyLoggedIn, idl::services::ServiceFailure, 
	CORBA::Exception)
{
  log_scope l(log.general_logger(), info_level, "Connection::loginBySingleSignOn");
  #ifdef OPENBUS_SDK_MULTITHREAD
  Mutex m(&_mutex);
  #endif
  interceptors::IgnoreInterceptor _i(_piCurrent);
  if (login()) throw AlreadyLoggedIn();
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  idl::OctetSeq_var secretOctetSeq = new idl::OctetSeq(
    static_cast<CORBA::ULong> (strlen((const char*) secret)),
    static_cast<CORBA::ULong> (strlen((const char*) secret)),
    (CORBA::Octet*) CORBA::string_dup((const char*) secret));
  loginAuthenticationInfo.data = secretOctetSeq;
  
  /* representação da minha chave em uma cadeia de bytes. */
  size_t len;
  unsigned char* bufKey = openssl::PubKey2byteSeq(_key, len);
  SHA256(bufKey, len, loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo = _orbInitializer->codec()->encode_value(any);

  fetchBusKey();

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública do barramento. */
  unsigned char* encrypted = openssl::encrypt(_busKey, encodedLoginAuthenticationInfo->get_buffer(), 
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

  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _busid = _access_control->busid();

  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) _renewLogin->run();
  else {
    _renewLogin = std::auto_ptr<RenewLogin> (new RenewLogin(this, _manager, validityTime));
    _renewLogin->start();
  }
  #else
  _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif    
}

bool Connection::_logout(bool local) {
  Mutex m(&_mutex);
  bool sucess = false;
  if (login()) {
    #ifdef OPENBUS_SDK_MULTITHREAD
    _renewLogin->pause();
    #else
    _orb->dispatcher()->remove(_renewLogin.get(), CORBA::Dispatcher::Timer);
    #endif
    _loginInfo.reset();
    if (!local) {
      Connection* c = 0;
      try {
        c = _manager->getRequester();
        _manager->setRequester(this);
        _access_control->logout();
        sucess = true;
        _manager->setRequester(c);
      } catch(...) { 
        sucess = false; 
        _manager->setRequester(c);        
      }
    }
  } else sucess = false;
  _busid = 0;
  _busKey = 0;
  if (_manager->getDispatcher(this->_busid)) _manager->clearDispatcher(this->_busid);
  _clientInterceptor->resetCaches();
  _serverInterceptor->resetCaches();
  return sucess;    
}

bool Connection::logout() throw (CORBA::Exception) { 
  log_scope l(log.general_logger(), info_level, "Connection::logout");
  return _logout(false); 
}

CallerChain* Connection::getCallerChain() throw (CORBA::Exception) {
  log_scope l(log.general_logger(), info_level, "Connection::getCallerChain");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(piCurrent));
  CORBA::Any* signedCallChainAny = piCurrent->get_slot(_orbInitializer->slotId_signedCallChain());
  CallerChain* callerChain = 0;
  idl_ac::CallChain callChain;
  idl_cr::SignedCallChain signedCallChain;
  if (*signedCallChainAny >>= signedCallChain) {
    CORBA::Any_var callChainAny = _orbInitializer->codec()->decode_value(signedCallChain.encoded,
      idl_ac::_tc_CallChain);
    *callChainAny >>= callChain;
    callerChain = new CallerChain(this->_busid, callChain.originators, callChain.caller, 
      signedCallChain);
  } else {
    CORBA::Any_var legacyChainAny = piCurrent->get_slot(_orbInitializer->slotId_legacyCallChain());
    if (legacyChainAny >>= callChain) {
      callerChain = new CallerChain(0, callChain.originators, callChain.caller);
    } else return 0;
  }
  return callerChain;
}

void Connection::joinChain(CallerChain* chain) throw (CORBA::Exception) {
  log_scope l(log.general_logger(), info_level, "Connection::joinChain");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(piCurrent));
  CORBA::Any signedCallChainAny;
  signedCallChainAny <<= *(chain->signedCallChain());
  piCurrent->set_slot(_orbInitializer->slotId_joinedCallChain(), signedCallChainAny);
}

void Connection::exitChain() throw (CORBA::Exception) {
  log_scope l(log.general_logger(), info_level, "Connection::exitChain");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(piCurrent));
  CORBA::Any any;
  piCurrent->set_slot(_orbInitializer->slotId_joinedCallChain(), any);    
}

CallerChain* Connection::getJoinedChain() throw (CORBA::Exception) {
  log_scope l(log.general_logger(), info_level, "Connection::getJoinedChain");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(piCurrent));
  CORBA::Any_var signedCallChainAny= piCurrent->get_slot(_orbInitializer->slotId_joinedCallChain());
  idl_cr::SignedCallChain signedCallChain;
  if (*signedCallChainAny >>= signedCallChain) {
    CORBA::Any_var callChainAny = _orbInitializer->codec()->decode_value(signedCallChain.encoded,
      idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain)
      return new CallerChain(callChain.target, callChain.originators, callChain.caller, 
        signedCallChain);
    else return 0;
  } else return 0;
}

}
