#include <connection.h>
#include <connection_impl.h>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/sha.h>

namespace openbus {
  Connection::Connection(
    const std::string host,
    const unsigned int port,
    ORB* orb,
    const interceptors::ORBInitializer* orbInitializer) 
    throw(CORBA::Exception)
    : _host(host), _port(port), _orb(orb), _orbInitializer(orbInitializer), _onInvalidLogin(0)
  {
    std::stringstream corbaloc;
    corbaloc << "corbaloc::" << _host << ":" << _port << "/" << idl::BusObjectKey;
    CORBA::Object_var obj = _orb->orb()->string_to_object(corbaloc.str().c_str());
    assert(!CORBA::is_nil(obj));
    _clientInterceptor = _orbInitializer->getClientInterceptor();
    _serverInterceptor = _orbInitializer->getServerInterceptor();
    _clientInterceptor->allowRequestWithoutCredential = true;
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
    buskeyOctetSeq = _access_control->buskey();
    _clientInterceptor->allowRequestWithoutCredential = false;
    //[doubt] não seria melhor trocar o assert por uma estrutura condicional?
    const unsigned char* buf = buskeyOctetSeq->get_buffer();
    assert(_busKey = d2i_PUBKEY(0, &buf, buskeyOctetSeq->length()));

    _prvKey = 0;
    EVP_PKEY_CTX* ctx;
    if (!((ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, 0)) &&
      (EVP_PKEY_keygen_init(ctx) > 0) &&
      (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) > 0) &&
      (EVP_PKEY_keygen(ctx, &_prvKey) > 0))
    )
      //[doubt] trocar assert por exceção ?
      assert(0);
    
    _clientInterceptor->setConnection(this);
    _serverInterceptor->setConnection(this);
    _orb->getConnectionMultiplexer()->addConnection(this);
    _loginCache = std::auto_ptr<LoginCache> (new LoginCache(this));
  }

  Connection::~Connection() {
    if (login())
      //[OBS] logout trata uma exceção qualquer.
      logout();
  }

  void Connection::loginByPassword(const char* entity, const char* password)
    throw (
      AlreadyLogged, 
      idl_ac::AccessDenied, 
      idl_ac::WrongEncoding,
      idl::services::ServiceFailure,
      CORBA::Exception)
  {
    if (login())
      throw AlreadyLogged();
    _clientInterceptor->allowRequestWithoutCredential = true;
    idl_ac::LoginAuthenticationInfo_var loginAuthenticationInfo = 
      new idl_ac::LoginAuthenticationInfo;
    idl::OctetSeq_var passwordOctetSeq = new idl::OctetSeq(
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
    if (!((ctx = EVP_PKEY_CTX_new(_busKey, 0)) &&
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

    idl::EncryptedBlock encryptedBlock;
    memcpy(encryptedBlock, encrypted, 256);

    OPENSSL_free(encrypted);
    EVP_PKEY_CTX_free(ctx);

    idl::OctetSeq_var prvkeyOctetSeq = new idl::OctetSeq(
      len,
      len,
      static_cast<CORBA::Octet*> (encodedPrvKey));

    idl_ac::ValidityTime validityTime;
    idl_ac::LoginInfo* loginInfo = _access_control->loginByPassword(
      entity, 
      prvkeyOctetSeq,
      encryptedBlock,
      validityTime);
    _clientInterceptor->allowRequestWithoutCredential = false;

    _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
    _clientInterceptor->allowRequestWithoutCredential = false;
  #ifdef MULTITHREAD
    _renewLogin.reset(new RenewLogin(this, validityTime));
    _renewLogin->start();
  #else
    _renewLogin.reset(new RenewLogin(_access_control));  
    _orb->orb()->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif
  }

  void Connection::loginByCertificate(const char* entity, EVP_PKEY* privateKey)
    throw (
      CorruptedPrivateKey, 
      CorruptedBusCertificate,
      WrongPrivateKey,
      AlreadyLogged, 
      idl_ac::MissingCertificate, 
      idl_ac::AccessDenied, 
      idl_ac::WrongEncoding,
      idl::services::ServiceFailure,
      CORBA::Exception)
  {
    if (login()) throw AlreadyLogged();
    idl::EncryptedBlock challenge;
    _clientInterceptor->allowRequestWithoutCredential = true;
    idl_ac::LoginProcess_var loginProcess = _access_control->startLoginByCertificate(
      entity, 
      challenge);
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
    idl_ac::LoginAuthenticationInfo_var loginAuthenticationInfo = 
      new idl_ac::LoginAuthenticationInfo;
    idl::OctetSeq_var secretOctetSeq = new idl::OctetSeq(
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
    if (!((ctx = EVP_PKEY_CTX_new(_busKey, 0)) &&
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

    idl::EncryptedBlock encryptedBlock;
    memcpy(encryptedBlock, encrypted, 256);

    OPENSSL_free(encrypted);
    EVP_PKEY_CTX_free(ctx);

    idl::OctetSeq_var prvkeyOctetSeq = new idl::OctetSeq(
      len,
      len,
      static_cast<CORBA::Octet*> (encodedPrvKey));

    idl_ac::ValidityTime validityTime;
    _clientInterceptor->allowRequestWithoutCredential = true;
    idl_ac::LoginInfo* loginInfo = loginProcess->login(
      prvkeyOctetSeq,
      encryptedBlock,
      validityTime);
    _clientInterceptor->allowRequestWithoutCredential = false;
    _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
    _clientInterceptor->allowRequestWithoutCredential = false;
  #ifdef MULTITHREAD
    _renewLogin.reset(new RenewLogin(this, validityTime));
    _renewLogin->start();
  #else
    _renewLogin.reset(new RenewLogin(_access_control));  
    _orb->orb()->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif
  }

  void Connection::loginByCertificate(const char* entity, const char* privateKeyFilename)
    throw (
      CorruptedPrivateKey, 
      CorruptedBusCertificate,
      WrongPrivateKey,
      AlreadyLogged, 
      idl_ac::MissingCertificate, 
      idl_ac::AccessDenied, 
      idl_ac::WrongEncoding,
      idl::services::ServiceFailure,
      CORBA::Exception)
  {
    FILE* privateKeyFile = fopen(privateKeyFilename, "r");
    if (!privateKeyFile) throw CorruptedPrivateKey();
    EVP_PKEY* privateKey = PEM_read_PrivateKey(privateKeyFile, 0, 0, 0);
    fclose(privateKeyFile);
    if (!privateKey) throw CorruptedPrivateKey();
    loginByCertificate(entity, privateKey);
  }

  std::pair <idl_ac::LoginProcess*, unsigned char*> Connection::startSingleSignOn() 
    throw (idl::services::ServiceFailure)
  {
    unsigned char* challenge = new unsigned char[256];
    idl_ac::LoginProcess* loginProcess = _access_control->startLoginBySingleSignOn(challenge);

    EVP_PKEY_CTX* ctx;
    unsigned char* secret;
    size_t secretLen;
    if (!((ctx = EVP_PKEY_CTX_new(_prvKey, 0)) &&
      (EVP_PKEY_decrypt_init(ctx) > 0) &&
      (EVP_PKEY_decrypt(
        ctx,
        0,
        &secretLen,
        challenge,
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
    
    //[todo] CorruptedPrivateKey
    return std::make_pair(loginProcess, secret);
  }
    
  void Connection::loginBySingleSignOn(
    idl_ac::LoginProcess* loginProcess, 
    unsigned char* secret)
  throw (idl::services::ServiceFailure)
  {
    if (login()) throw AlreadyLogged();
    idl_ac::LoginAuthenticationInfo_var loginAuthenticationInfo = 
      new idl_ac::LoginAuthenticationInfo;
    idl::OctetSeq_var secretOctetSeq = new idl::OctetSeq(
      static_cast<CORBA::ULong> (strlen((const char*) secret)),
      static_cast<CORBA::ULong> (strlen((const char*) secret)),
      (CORBA::Octet*) CORBA::string_dup((const char*) secret));
    loginAuthenticationInfo->data = secretOctetSeq;
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
    EVP_PKEY_CTX* ctx;
    unsigned char* encrypted;
    size_t encryptedLen;
    if (!((ctx = EVP_PKEY_CTX_new(_busKey, 0)) &&
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

    idl::EncryptedBlock encryptedBlock;
    memcpy(encryptedBlock, encrypted, 256);

    OPENSSL_free(encrypted);
    EVP_PKEY_CTX_free(ctx);

    idl::OctetSeq_var prvkeyOctetSeq = new idl::OctetSeq(
      len,
      len,
      static_cast<CORBA::Octet*> (encodedPrvKey));

    idl_ac::ValidityTime validityTime;
    _clientInterceptor->allowRequestWithoutCredential = true;
    idl_ac::LoginInfo* loginInfo = loginProcess->login(
      prvkeyOctetSeq,
      encryptedBlock,
      validityTime);
    _clientInterceptor->allowRequestWithoutCredential = false;
    _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
    _clientInterceptor->allowRequestWithoutCredential = false;
  #ifdef MULTITHREAD
    _renewLogin.reset(new RenewLogin(this, validityTime));
    _renewLogin->start();
  #else
    _renewLogin.reset(new RenewLogin(_access_control));  
    _orb->orb()->dispatcher()->tm_event(_renewLogin.get(), validityTime*1000);
  #endif    
  }

  void Connection::close() {
    if (login()) logout();
    _clientInterceptor->setConnection(0);
    _serverInterceptor->setConnection(0);
    _orb->getConnectionMultiplexer()->removeConnection(this);
  }

  bool Connection::logout() {
    if (login()) {
      try {
        _access_control->logout();
      #ifdef MULTITHREAD
        _renewLogin->stop();
        _renewLogin->wait();
        _renewLogin.reset();
      #else
        _orb->orb()->dispatcher()->remove(_renewLogin.get(), CORBA::Dispatcher::Timer);
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
  
  void Connection::joinChain(CallerChain* chain) {
    CORBA::Object_var init_ref = _orb->orb()->resolve_initial_references("PICurrent");
    assert(!CORBA::is_nil(init_ref));
    PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
    CORBA::Any signedCallChainAny;
    signedCallChainAny <<= *(chain->signedCallChain());
    piCurrent->set_slot(
      _orbInitializer->slotId_joinedCallChain(),
      signedCallChainAny);
  }

  void Connection::exitChain() {
    CORBA::Object_var init_ref = _orb->orb()->resolve_initial_references("PICurrent");
    assert(!CORBA::is_nil(init_ref));
    PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
    CORBA::Any any;
    piCurrent->set_slot(
      _orbInitializer->slotId_joinedCallChain(),
      any);    
  }

  CallerChain* Connection::getJoineChain() {
    CORBA::Object_var init_ref = _orb->orb()->resolve_initial_references("PICurrent");
    assert(!CORBA::is_nil(init_ref));
    PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
    CORBA::Any_var signedCallChainAny = piCurrent->get_slot(
      _orbInitializer->slotId_joinedCallChain());
    idl_ac::SignedCallChain signedCallChain;
    if (*signedCallChainAny >>= signedCallChain) {
      CallerChain* c = new CallerChain();
      CORBA::Any_var callChainAny = _orbInitializer->codec()->decode_value(
        signedCallChain.encoded,
        idl_ac::_tc_CallChain);
      idl_ac::CallChain callChain;
      callChainAny >>= callChain;
      c->signedCallChain(signedCallChain);
      c->busid = callChain.target;
      c->callers = callChain.callers;
      return c;
    } else
      return 0;
  }

  CallerChain* Connection::getCallerChain() {
    CORBA::Object_var init_ref = _orb->orb()->resolve_initial_references("PICurrent");
    assert(!CORBA::is_nil(init_ref));
    PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
    CORBA::Any* signedCallChainAny = piCurrent->get_slot(_orbInitializer->slotId_signedCallChain());
    CORBA::Any* busidAny = piCurrent->get_slot(_orbInitializer->slotId_busid());
    const char* busid;
    *busidAny >>= busid;
    idl_ac::SignedCallChain signedCallChain;
    *signedCallChainAny >>= signedCallChain;
    CORBA::Any_var callChainAny = _orbInitializer->codec()->decode_value(
      signedCallChain.encoded,
      idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    *callChainAny >>= callChain;
    CallerChain* callerChain = new CallerChain();
    callerChain->callers = callChain.callers;
    callerChain->busid = CORBA::string_dup(busid);
    callerChain->signedCallChain(signedCallChain);
    return callerChain;
  }
  
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
    if (login->time2live > (time(0) - _timeUpdated)) 
      return login;
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
      for (it=keys.begin(); it<keys.end(); ++i, ++it)
        ids[i] = CORBA::string_dup((*it).c_str());
      idl_ac::ValidityTimeSeq_var validity = _conn->login_registry()->getValidity(ids);
      for (unsigned int i=0; i<validity->length(); ++i) {
        Login* l = _id_Login->fetch(std::string(ids[i]));
        l->time2live = validity[i];
      }
      if (login->time2live > 0) return login;
    }
    return 0;
  }
  
  CORBA::ORB* Connection::orb() const { return _orb->orb(); }
  
#ifdef MULTITHREAD
  RenewLogin::RenewLogin(
    Connection* connection, 
    idl_ac::ValidityTime validityTime) 
    : connection(connection), validityTime(validityTime), sigINT(false) 
  { }

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
      else 
        //[doubt] try-catch
        validityTime = _access_control->renew();
    }
  }
#else
  RenewLogin::RenewLogin(idl_ac::AccessControl* _access_control) 
    : _access_control(_access_control) 
  { }

  void RenewLogin::callback(CORBA::Dispatcher* dispatcher, Event event) {
    dispatcher->tm_event(this, _access_control->renew()*1000);
  }
#endif
}
