#include <connection.h>
#include <connection_impl.h>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/sha.h>

namespace openbus {
  Connection::Connection(
    const std::string host,
    const unsigned int port,
    CORBA::ORB* orb,
    const interceptors::ORBInitializer* orbInitializer) throw(CORBA::Exception)
    : _host(host), _port(port), _orb(orb), _orbInitializer(orbInitializer) 
  {
    std::stringstream corbaloc;
    corbaloc << "corbaloc::" << _host << ":" << _port << "/" << openbusidl::BusObjectKey;
    CORBA::Object_var obj = _orb->string_to_object(corbaloc.str().c_str());
    assert(!CORBA::is_nil(obj));
    _clientInterceptor = _orbInitializer->getClientInterceptor();
    _clientInterceptor->allowRequestWithoutCredential = true;
    _iComponent = scs::core::IComponent::_narrow(obj);
    obj = _iComponent->getFacetByName(openbusidl_access_control::AccessControlFacet);
    assert(!CORBA::is_nil(obj));
    _access_control = openbusidl_access_control::AccessControl::_narrow(obj);
    obj = _iComponent->getFacetByName(openbusidl_offer_registry::OfferRegistryFacet);
    assert(!CORBA::is_nil(obj));
    _offer_registry = openbusidl_offer_registry::OfferRegistry::_narrow(obj);
    _busId = _access_control->busid();
    _loginInfo.reset();
    buskeyOctetSeq = _access_control->buskey();
    _clientInterceptor->allowRequestWithoutCredential = false;
    //[doubt] não seria melhor trocar o assert por uma estrutura condicional?
    const unsigned char* buf = buskeyOctetSeq->get_buffer();
    assert(_buskey = d2i_PUBKEY(0, &buf, buskeyOctetSeq->length()));

    _prvKey = 0;
    EVP_PKEY_CTX* ctx;
    if (!((ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, 0)) &&
        (EVP_PKEY_keygen_init(ctx) > 0) &&
        (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) > 0) &&
        (EVP_PKEY_keygen(ctx, &_prvKey) > 0))
    )
      //[doubt] trocar assert por exceção ?
      assert(0);
    
    //[obs] auto_ptr evitando *memory leak* se o construtor lançar uma exceção.
    // _callerChain = std::auto_ptr <openbusidl_access_control::CallerChain> 
    //   (new openbusidl_access_control::CallerChain);
    // _callerChain->busid = CORBA::string_dup(_busId);
    // openbusidl_access_control::LoginInfoSeq* loginInfoSeq = 
    //   new openbusidl_access_control::LoginInfoSeq(256);
    // loginInfoSeq->length(0);
    // _callerChain->logins = *loginInfoSeq;
    _clientInterceptor->addConnection(this);
  }

  Connection::~Connection() {
    if (loginInfo())
      //[OBS] logout trata uma exceção qualquer.
      logout();
  }

  void Connection::loginByPassword(const char* entity, const char* password)
    throw (
      AlreadyLogged, 
      openbusidl_access_control::AccessDenied, 
      openbusidl_access_control::WrongEncoding,
      openbusidl::services::ServiceFailure,
      CORBA::Exception)
  {
    if (loginInfo())
      throw AlreadyLogged();
    _clientInterceptor->allowRequestWithoutCredential = true;
    openbusidl_access_control::LoginAuthenticationInfo_var loginAuthenticationInfo = 
      new openbusidl_access_control::LoginAuthenticationInfo;
    openbusidl::OctetSeq_var passwordOctetSeq = new openbusidl::OctetSeq(
      static_cast<CORBA::ULong> (strlen(password)),
      static_cast<CORBA::ULong> (strlen(password)),
      (CORBA::Octet*) CORBA::string_dup(password));
    loginAuthenticationInfo->data = passwordOctetSeq;
    unsigned char* encodedPrvKey = 0;
    size_t len = i2d_PUBKEY(_prvKey, &encodedPrvKey);
    if (len < 0)
      //[doubt] trocar assert por exceção ?
      assert(0);
    SHA256(encodedPrvKey, len, loginAuthenticationInfo->hash);

    CORBA::Any any;
    any <<= *loginAuthenticationInfo;
    CORBA::OctetSeq_var encodedLoginAuthenticationInfo = 
      _orbInitializer->codec()->encode_value(any);

    EVP_PKEY_CTX* ctx;
    unsigned char* encrypted;
    size_t encryptedLen;
    if (!((ctx = EVP_PKEY_CTX_new(_buskey, 0)) &&
        (EVP_PKEY_encrypt_init(ctx) > 0) &&
        (EVP_PKEY_encrypt(
          ctx, 
          0, 
          &encryptedLen,
          encodedLoginAuthenticationInfo->get_buffer(),
          encodedLoginAuthenticationInfo->length()) > 0))
    )
      //[doubt] trocar assert por exceção ?
      assert(0);

    assert(encrypted = (unsigned char*) OPENSSL_malloc(encryptedLen));
    if (EVP_PKEY_encrypt(
      ctx, 
      encrypted, 
      &encryptedLen,
      encodedLoginAuthenticationInfo->get_buffer(),
      encodedLoginAuthenticationInfo->length()) <= 0
    )
      //[doubt] trocar assert por exceção ?
      assert(0);

    openbusidl::EncryptedBlock encryptedBlock;
    memcpy(encryptedBlock, encrypted, 256);

    OPENSSL_free(encrypted);
    EVP_PKEY_CTX_free(ctx);

    openbusidl::OctetSeq_var prvkeyOctetSeq = new openbusidl::OctetSeq(
      len,
      len,
      static_cast<CORBA::Octet*> (encodedPrvKey));

    openbusidl_access_control::ValidityTime validityTime;
    char* id = _access_control->loginByPassword(
      entity, 
      prvkeyOctetSeq,
      encryptedBlock,
      validityTime);
    _clientInterceptor->allowRequestWithoutCredential = false;

    openbusidl_access_control::LoginInfo* _loginInfo = new openbusidl_access_control::LoginInfo();
    _loginInfo->id = id;
    _loginInfo->entity = CORBA::string_dup(entity);
    this->_loginInfo = std::auto_ptr<openbusidl_access_control::LoginInfo> 
      (_loginInfo);
    _clientInterceptor->allowRequestWithoutCredential = false;
  // #ifdef MULTITHREAD
  //   _renewLogin.reset(new RenewLogin(this, validityTime));
  //   _renewLogin->start();
  // #else
  //   _renewLogin.reset(new RenewLogin(_access_control));  
  //   _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  // #endif
  }

  void Connection::loginByCertificate(
    const char* entity, 
    EVP_PKEY* privateKey)
    throw (
      CorruptedPrivateKey, 
      CorruptedBusCertificate,
      WrongPrivateKey,
      AlreadyLogged, 
      openbusidl_access_control::MissingCertificate, 
      openbusidl_access_control::AccessDenied, 
      openbusidl_access_control::WrongEncoding,
      openbusidl::services::ServiceFailure,
      CORBA::Exception)
  {
    if (loginInfo())
      throw AlreadyLogged();
    openbusidl::EncryptedBlock challenge;
    _clientInterceptor->allowRequestWithoutCredential = true;
    openbusidl_access_control::LoginByCertificate_var loginByCertificate =
      _access_control->startLoginByCertificate(entity, challenge);
    _clientInterceptor->allowRequestWithoutCredential = false;

    EVP_PKEY_CTX* ctx;
    unsigned char* secret;
    size_t secretLen;
    if (!((ctx = EVP_PKEY_CTX_new(privateKey, 0)) &&
        (EVP_PKEY_decrypt_init(ctx) > 0) &&
        (EVP_PKEY_decrypt(
          ctx,
          0,
          &secretLen,
          (unsigned char*) challenge,
          256) > 0))
    )
      //[doubt] trocar assert por exceção ?
      assert(0);

    assert(secret = (unsigned char*) OPENSSL_malloc(secretLen));
    if (EVP_PKEY_decrypt(
          ctx,
          secret,
          &secretLen,
          challenge,
          256) <= 0
    )
      //[doubt] trocar assert por exceção ?
      assert(0);
    secret[secretLen] = '\0';
    openbusidl_access_control::LoginAuthenticationInfo_var loginAuthenticationInfo = 
      new openbusidl_access_control::LoginAuthenticationInfo;
    openbusidl::OctetSeq_var secretOctetSeq = new openbusidl::OctetSeq(
      static_cast<CORBA::ULong> (strlen((const char*) secret)),
      static_cast<CORBA::ULong> (strlen((const char*) secret)),
      (CORBA::Octet*) CORBA::string_dup((const char*) secret));
    loginAuthenticationInfo->data = secretOctetSeq;
    OPENSSL_free(secret);
    EVP_PKEY_CTX_free(ctx);
    unsigned char* encodedPrvKey = 0;
    size_t len = i2d_PUBKEY(_prvKey, &encodedPrvKey);
    if (len < 0)
      //[doubt] trocar assert por exceção ?
      assert(0);
    SHA256(encodedPrvKey, len, loginAuthenticationInfo->hash);

    CORBA::Any any;
    any <<= *loginAuthenticationInfo;
    CORBA::OctetSeq_var encodedLoginAuthenticationInfo = 
      _orbInitializer->codec()->encode_value(any);

    // EVP_PKEY_CTX* ctx;
    unsigned char* encrypted;
    size_t encryptedLen;
    if (!((ctx = EVP_PKEY_CTX_new(_buskey, 0)) &&
        (EVP_PKEY_encrypt_init(ctx) > 0) &&
        (EVP_PKEY_encrypt(
          ctx, 
          0, 
          &encryptedLen,
          encodedLoginAuthenticationInfo->get_buffer(),
          encodedLoginAuthenticationInfo->length()) > 0))
    )
      //[doubt] trocar assert por exceção ?
      assert(0);

    assert(encrypted = (unsigned char*) OPENSSL_malloc(encryptedLen));
    if (EVP_PKEY_encrypt(
          ctx, 
          encrypted, 
          &encryptedLen,
          encodedLoginAuthenticationInfo->get_buffer(),
          encodedLoginAuthenticationInfo->length()) <= 0
    )
      //[doubt] trocar assert por exceção ?
      assert(0);

    openbusidl::EncryptedBlock encryptedBlock;
    memcpy(encryptedBlock, encrypted, 256);

    OPENSSL_free(encrypted);
    EVP_PKEY_CTX_free(ctx);

    openbusidl::OctetSeq_var prvkeyOctetSeq = new openbusidl::OctetSeq(
      len,
      len,
      static_cast<CORBA::Octet*> (encodedPrvKey));

    openbusidl_access_control::ValidityTime validityTime;
    _clientInterceptor->allowRequestWithoutCredential = true;
    char* id = loginByCertificate->login(
      prvkeyOctetSeq,
      encryptedBlock,
      validityTime);
    _clientInterceptor->allowRequestWithoutCredential = false;
    openbusidl_access_control::LoginInfo* _loginInfo = new openbusidl_access_control::LoginInfo();
    _loginInfo->id = id;
    _loginInfo->entity = CORBA::string_dup(entity);
    this->_loginInfo = std::auto_ptr<openbusidl_access_control::LoginInfo> 
      (_loginInfo);
    _clientInterceptor->allowRequestWithoutCredential = false;
  #ifdef MULTITHREAD
    _renewLogin.reset(new RenewLogin(this, validityTime));
    _renewLogin->start();
  #else
    _renewLogin.reset(new RenewLogin(_access_control));  
    _orb->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif
  }

  void Connection::loginByCertificate(
    const char* entity, 
    const char* privateKeyFilename)
    throw (
      CorruptedPrivateKey, 
      CorruptedBusCertificate,
      WrongPrivateKey,
      AlreadyLogged, 
      openbusidl_access_control::MissingCertificate, 
      openbusidl_access_control::AccessDenied, 
      openbusidl_access_control::WrongEncoding,
      openbusidl::services::ServiceFailure,
      CORBA::Exception)
  {
    FILE* privateKeyFile = fopen(privateKeyFilename, "r");
    if (!privateKeyFile)
      throw CorruptedPrivateKey();
    EVP_PKEY* privateKey = PEM_read_PrivateKey(privateKeyFile, 0, 0, 0);
    fclose(privateKeyFile);
    if (!privateKey)
      throw CorruptedPrivateKey();
    loginByCertificate(entity, privateKey);
  }

  void Connection::close() {
    if (loginInfo())
      logout();
    _clientInterceptor->removeConnection(this);
  }

  bool Connection::logout() {
    if (loginInfo()) {
      try {
        _access_control->logout();
      #ifdef MULTITHREAD
        _renewLogin->stop();
        _renewLogin->wait();
        _renewLogin.reset();
      #else
        _orb->dispatcher()->remove(_renewLogin.get(), CORBA::Dispatcher::Timer);
        _renewLogin.reset();
      #endif
        _loginInfo.reset();
        return true;
      } catch(...) {
        return false;
      }
    }
    return false;
  }

#ifdef MULTITHREAD
  RenewLogin::RenewLogin(
    Connection* connection, 
    openbusidl_access_control::ValidityTime validityTime) 
    :  connection(connection), validityTime(validityTime), sigINT(false) { }

  RenewLogin::~RenewLogin() { }

  bool RenewLogin::_sleep(unsigned int time) {
    for(unsigned int x=0; x<time; ++x) {
      if (sigINT) {
        sigINT = false;
        return true;
      }
      sleep(1);
    }
    return false;
  }

  void RenewLogin::_run(void*) {
    while (true) {
      if (_sleep(validityTime)) 
        break;
      else {
        //[doubt] try-catch
        validityTime = _access_control->renew();
      }
    }
  }
#else
  RenewLogin::RenewLogin(openbusidl_access_control::AccessControl* _access_control) 
    : _access_control(_access_control) { }

  void RenewLogin::callback(CORBA::Dispatcher* dispatcher, Event event) {
    dispatcher->tm_event(this, _access_control->renew()*1000);
  }
#endif
}
