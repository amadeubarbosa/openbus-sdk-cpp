#include <connection.h>
#include <connection_impl.h>
#include <util/openssl.h>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <log.h>

namespace openbus {
  
Connection::Connection(
  const std::string h,
  const unsigned int p,
  CORBA::ORB* orb,
  const interceptors::ORBInitializer* ini,
  ConnectionManager* m) 
  throw(CORBA::Exception)
  : _host(h), _port(p), _orb(orb), _orbInitializer(ini), _onInvalidLogin(0), _manager(m)
{
  log_scope l(log.general_logger(), info_level, "Connection::Connection");
  std::stringstream corbaloc;
  corbaloc << "corbaloc::" << _host << ":" << _port << "/" << idl::BusObjectKey;
  CORBA::Object_var obj = _orb->string_to_object(corbaloc.str().c_str());
  assert(!CORBA::is_nil(obj));
  _clientInterceptor = _orbInitializer->clientInterceptor();
  _serverInterceptor = _orbInitializer->serverInterceptor();
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  assert(!CORBA::is_nil(init_ref));
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
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
    _busid = _access_control->busid();
    _loginInfo.reset();
    _buskeyOctetSeq = _access_control->buskey();
  }

  /* armazenando a chave pública do barramento. */
  _busKey = openssl::byteSeq2EVPPkey(_buskeyOctetSeq->get_buffer(), _buskeyOctetSeq->length());

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

Connection::~Connection() { logout(); }

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
  unsigned char* bufKey = openssl::EVPPkey2byteSeq(_key, len);

  /* criando uma hash da minha chave. */
  SHA256(bufKey, len, loginAuthenticationInfo.hash);

  /* CDR da estrura LoginAuthenticationInfo que foi montada acima. */
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo= _orbInitializer->codec()->encode_value(any);

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública do barramento. */
  unsigned char* encrypted = openssl::encrypt(_busKey, encodedLoginAuthenticationInfo->get_buffer(), 
    encodedLoginAuthenticationInfo->length());
  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted, 256);
  OPENSSL_free(encrypted);
    
  idl_ac::ValidityTime validityTime;
  idl::OctetSeq_var keyOctetSeq = new idl::OctetSeq(len, len,static_cast<CORBA::Octet*> (bufKey));
  idl_ac::LoginInfo* loginInfo;
  //[doubt] segundo openbus.idl eu não posso repassar WrongEncoding, devo mapear para uma 
  //AccessDenied?
  try {
    loginInfo = _access_control->loginByPassword(entity, keyOctetSeq, encryptedBlock, validityTime);
  } catch (idl_ac::WrongEncoding&) {
    throw AccessDenied();
  }

  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);

  #ifdef OPENBUS_SDK_MULTITHREAD
  _renewLogin.reset(new RenewLogin(this, _manager, validityTime));
  _renewLogin->start();
  #else
  _renewLogin.reset(new RenewLogin(this, _manager, validityTime));  
  _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif
}

void Connection::loginByCertificate(const char* entity, EVP_PKEY* privateKey)
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
    //_access_control->startLoginByCertificate: throw (idl_ac::MissingCertificate, ServiceFailure)
    loginProcess = _access_control->startLoginByCertificate(entity,challenge);
  }
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
  unsigned char* bufKey = openssl::EVPPkey2byteSeq(_key, len);
  SHA256(bufKey, len, loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo= _orbInitializer->codec()->encode_value(any);

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública do barramento. */
  unsigned char* encrypted = openssl::encrypt(_busKey, encodedLoginAuthenticationInfo->get_buffer(), 
    encodedLoginAuthenticationInfo->length());
  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted, 256);
  OPENSSL_free(encrypted);

  idl::OctetSeq_var keyOctetSeq = new idl::OctetSeq(len, len,static_cast<CORBA::Octet*> (bufKey));

  interceptors::IgnoreInterceptor _i(_piCurrent);
  idl_ac::ValidityTime validityTime;    
  //[doubt] o que eu devo fazer com exceção idl_ac::WrongEncoding ?
  idl_ac::LoginInfo* loginInfo;
  try {
    loginInfo = loginProcess->login(keyOctetSeq, encryptedBlock, validityTime);
  } catch (idl_ac::AccessDenied& e) {
    throw WrongPrivateKey();
  }
  
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
#ifdef OPENBUS_SDK_MULTITHREAD
  _renewLogin.reset(new RenewLogin(this, _manager, validityTime));
  _renewLogin->start();
#else
  _renewLogin.reset(new RenewLogin(this, _manager, validityTime));  
  _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
#endif
}

void Connection::loginByCertificate(const char* entity, const char* privateKeyFilename)
  throw (CorruptedPrivateKey, WrongPrivateKey, AlreadyLoggedIn, idl_ac::MissingCertificate, 
  idl::services::ServiceFailure, CORBA::Exception)
{
  log_scope l(log.general_logger(), info_level, "Connection::loginByCertificate");
  FILE* privateKeyFile = fopen(privateKeyFilename, "r");
  if (!privateKeyFile) throw CorruptedPrivateKey();
  EVP_PKEY* privateKey = PEM_read_PrivateKey(privateKeyFile, 0, 0, 0);
  fclose(privateKeyFile);
  if (!privateKey) throw CorruptedPrivateKey();
  loginByCertificate(entity, privateKey);
}

std::pair <idl_ac::LoginProcess*, unsigned char*> Connection::startSingleSignOn() 
  throw (idl::services::ServiceFailure, CORBA::Exception)
{
  log_scope l(log.general_logger(), info_level, "Connection::startSingleSignOn");
  #ifdef OPENBUS_SDK_MULTITHREAD
  Mutex m(&_mutex);
  #endif
  unsigned char* challenge = new unsigned char[256];
  idl_ac::LoginProcess* loginProcess = _access_control->startLoginBySingleSignOn(challenge);

  unsigned char* secret = openssl::decrypt(_key, (unsigned char*) challenge, 256);
  return std::make_pair(loginProcess, secret);
}
  
void Connection::loginBySingleSignOn(idl_ac::LoginProcess* loginProcess, unsigned char* secret)
	throw(WrongSecret, InvalidLoginProcess, AlreadyLoggedIn, idl::services::ServiceFailure, 
	CORBA::Exception)
{
  log_scope l(log.general_logger(), info_level, "Connection::loginBySingleSignOn");
  #ifdef OPENBUS_SDK_MULTITHREAD
  Mutex m(&_mutex);
  #endif
  if (login()) throw AlreadyLoggedIn();
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  idl::OctetSeq_var secretOctetSeq = new idl::OctetSeq(
    static_cast<CORBA::ULong> (strlen((const char*) secret)),
    static_cast<CORBA::ULong> (strlen((const char*) secret)),
    (CORBA::Octet*) CORBA::string_dup((const char*) secret));
  loginAuthenticationInfo.data = secretOctetSeq;
  
  /* representação da minha chave em uma cadeia de bytes. */
  size_t len;
  unsigned char* bufKey = openssl::EVPPkey2byteSeq(_key, len);
  SHA256(bufKey, len, loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo =_orbInitializer->codec()->encode_value(any);

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública do barramento. */
  unsigned char* encrypted = openssl::encrypt(_busKey, encodedLoginAuthenticationInfo->get_buffer(), 
    encodedLoginAuthenticationInfo->length());
  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted, 256);
  OPENSSL_free(encrypted);

  idl::OctetSeq_var keyOctetSeq = new idl::OctetSeq(len,len, static_cast<CORBA::Octet*> (bufKey));

  interceptors::IgnoreInterceptor _i(_piCurrent);
  idl_ac::ValidityTime validityTime;
  // loginProcess->login(): throw (WrongEncoding, AccessDenied, ServiceFailure)
  //[doubt] InvalidLoginProcess
  //[doubt] WrongEncoding deve ser mapeado para o que?
  idl_ac::LoginInfo* loginInfo; 
  try {
    loginInfo = loginProcess->login(keyOctetSeq, encryptedBlock, validityTime);
  } catch(idl_ac::AccessDenied&) {
    throw WrongSecret();
  }

  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  #ifdef OPENBUS_SDK_MULTITHREAD
  _renewLogin.reset(new RenewLogin(this, _manager, validityTime));
  _renewLogin->start();
  #else
  _renewLogin.reset(new RenewLogin(this, _manager, validityTime));  
  _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif    
}

bool Connection::_logout(bool local) {
  bool sucess = false;
  if (login()) {
    #ifdef OPENBUS_SDK_MULTITHREAD
    _renewLogin->stop();
    _renewLogin->wait();
    _renewLogin.reset();
    #else
    _orb->dispatcher()->remove(_renewLogin.get(), CORBA::Dispatcher::Timer);
    _renewLogin.reset();
    #endif
    _loginInfo.reset();
    if (!local) {
      try {
        _access_control->logout();
        sucess = true;
      } catch(...) {
        sucess = false;
      }
    }
  } else sucess = false;
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
  assert(!CORBA::is_nil(init_ref));
  PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  CORBA::Any* signedCallChainAny = piCurrent->get_slot(_orbInitializer->slotId_signedCallChain());
  CORBA::Any* busidAny = piCurrent->get_slot(_orbInitializer->slotId_busid());
  const char* busid;
  CallerChain* callerChain = 0;
  idl_ac::CallChain callChain;
  if (*busidAny >>= busid) {
    idl_cr::SignedCallChain signedCallChain;
    if (*signedCallChainAny >>= signedCallChain) {
      CORBA::Any_var callChainAny = _orbInitializer->codec()->decode_value(signedCallChain.encoded,
        idl_ac::_tc_CallChain);
      *callChainAny >>= callChain;
      callerChain = new CallerChain();
      callerChain->_callers = callChain.callers;
      callerChain->_busid = CORBA::string_dup(busid);
      callerChain->signedCallChain(signedCallChain);
    } else return 0;
  } else {
    CORBA::Any* legacyChainAny = piCurrent->get_slot(_orbInitializer->slotId_legacyCallChain());
    if (*legacyChainAny >>= callChain) {
      callerChain = new CallerChain();
      callerChain->_callers = callChain.callers;
      callerChain->_busid = 0;
    } else return 0;
  }
  return callerChain;
}

void Connection::joinChain(CallerChain* chain) throw (CORBA::Exception) {
  log_scope l(log.general_logger(), info_level, "Connection::joinChain");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  assert(!CORBA::is_nil(init_ref));
  PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  CORBA::Any signedCallChainAny;
  signedCallChainAny <<= *(chain->signedCallChain());
  piCurrent->set_slot(_orbInitializer->slotId_joinedCallChain(), signedCallChainAny);
}

void Connection::exitChain() throw (CORBA::Exception) {
  log_scope l(log.general_logger(), info_level, "Connection::exitChain");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  assert(!CORBA::is_nil(init_ref));
  PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  CORBA::Any any;
  piCurrent->set_slot(_orbInitializer->slotId_joinedCallChain(), any);    
}

CallerChain* Connection::getJoinedChain() throw (CORBA::Exception) {
  log_scope l(log.general_logger(), info_level, "Connection::getJoinedChain");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  assert(!CORBA::is_nil(init_ref));
  PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  CORBA::Any_var signedCallChainAny= piCurrent->get_slot(_orbInitializer->slotId_joinedCallChain());
  idl_cr::SignedCallChain signedCallChain;
  if (*signedCallChainAny >>= signedCallChain) {
    CallerChain* c = new CallerChain();
    CORBA::Any_var callChainAny = _orbInitializer->codec()->decode_value(signedCallChain.encoded,
      idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain) {
      c->signedCallChain(signedCallChain);
      c->_busid = callChain.target;
      c->_callers = callChain.callers;
      return c;
    } else return 0;
  } else return 0;
}

}
