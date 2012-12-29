// -*- coding: iso-8859-1 -*-
#include "openbus/log.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/util/AutoLock_impl.hpp"

#include <sstream>
#include <unistd.h>
#include <cstring>
#include <ctime>
#include <CORBA.h>

namespace openbus 
{

void Connection::checkBusid() const 
{
  if (strcmp(_busid.c_str(), _access_control->busid())) 
  {
    throw BusChanged();
  }
}

Connection::Connection(const std::string h, const unsigned short p, 
                       CORBA::ORB *orb, IOP::Codec *c, 
                       PortableInterceptor::SlotId s1, 
                       PortableInterceptor::SlotId s2,
                       PortableInterceptor::SlotId s3,
                       PortableInterceptor::SlotId s4,
                       OpenBusContext *m, std::vector<std::string> props) 
  : _host(h), _port(p), _orb(orb), _codec(c), _slotId_joinedCallChain(s1), 
    _slotId_signedCallChain(s2), _slotId_legacyCallChain(s3), 
    _slotId_receiveConnection(s4), _renewLogin(0), _loginInfo(0), 
    _onInvalidLogin(0), _state(UNLOGGED), _openbusContext(m), 
    _legacyDelegate(CALLER)
{
  log_scope l(log.general_logger(), info_level, "Connection::Connection");
  std::stringstream corbaloc;
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent.in()));
  corbaloc << "corbaloc::" << _host << ":" << _port << "/" << idl::BusObjectKey;
  CORBA::Object_var obj;
  obj = _orb->string_to_object(corbaloc.str().c_str());
  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    _iComponent = scs::core::IComponent::_narrow(obj);
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
	
  _loginCache = std::auto_ptr<LoginCache> (new LoginCache(_login_registry));
  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    _busid = _access_control->busid();
    CORBA::OctetSeq_var o = _access_control->buskey();
    _buskey = std::auto_ptr<PublicKey> (new PublicKey(o));
  }
  
  for (std::vector<std::string>::const_iterator it = props.begin(); 
       it != props.end(); ++it)
  {
    if (*it == "legacydelegate")
    {
      if (++it != props.end())
      {
        if (*it == "originator")
        {
          _legacyDelegate = ORIGINATOR;
        }
        else if (*it == "caller")
        {
          _legacyDelegate = CALLER;
        }
        else
        {
          throw InvalidPropertyValue("legacydelegate", *it);
        }
      }
      else
      {
        throw InvalidPropertyValue("legacydelegate", *it);
      }
    }
  }
}

Connection::~Connection() 
{ 
  log_scope l(log.general_logger(), info_level, "Connection::~Connection");
  try
  {
    _logout(true);
  }
  catch(...)
  {
    try
    {
      l.log("Exception thrown in destructor. Ignoring exception");
    }
    catch(...)
    {
    }
  }
  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) 
  {
    _renewLogin->stop();
    _renewLogin->wait();
  }
  #endif
}

void Connection::loginByPassword(std::string entity, std::string password) 
{
  log_scope l(log.general_logger(), info_level, "Connection::loginByPassword");
  AutoLock m(&_mutex);
  bool state = _state;
  m.unlock();
  if (state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  checkBusid();  
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  
  /* representa��o do password em uma cadeia de bytes. */
  CORBA::ULong passSize = static_cast<CORBA::ULong> (password.size());
  idl::OctetSeq_var passOctetSeq = 
    new idl::OctetSeq(passSize, passSize, (CORBA::Octet *) 
                      CORBA::string_dup(password.c_str()));
  loginAuthenticationInfo.data = passOctetSeq;
  
  /* representa��o da minha chave em uma cadeia de bytes. */
  CORBA::OctetSeq bufKey = _key.pubKey();
  
  /* criando uma hash da minha chave. */
  SHA256(bufKey.get_buffer(), bufKey.length(), loginAuthenticationInfo.hash);

  /* CDR da estrura LoginAuthenticationInfo que foi montada acima. */
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo= _codec->encode_value(any);

  /* cifrando a estrutura LoginAuthenticationInfo com a chave p�blica
   * do barramento. */
  CORBA::OctetSeq encrypted = 
    _buskey->encrypt(encodedLoginAuthenticationInfo->get_buffer(), 
                     encodedLoginAuthenticationInfo->length());

  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted.get_buffer(), idl::EncryptedBlockSize);
    
  idl_ac::ValidityTime validityTime;
  idl_ac::LoginInfo *loginInfo;
  try 
  {
    loginInfo = _access_control->loginByPassword(entity.c_str(), bufKey, 
                                                 encryptedBlock, validityTime);
  } 
  catch (const idl_ac::WrongEncoding &) 
  {
    throw idl::services::ServiceFailure();
  }

  m.lock();
  if (_state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _state = LOGGED;

  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) 
  {
    m.unlock();
    _renewLogin->run();
    m.lock();
  } 
  else 
  {
    _renewLogin = std::auto_ptr<RenewLogin> 
      (new RenewLogin(this, _access_control, _openbusContext, validityTime));
    m.unlock();
    _renewLogin->start();
    m.lock();
  }
  #else
  assert(!_renewLogin.get());
  _renewLogin = std::auto_ptr<RenewLogin> 
    (new RenewLogin(_orb, this, _access_control, _openbusContext, 
                    validityTime));
  #endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

void Connection::loginByCertificate(std::string entity, 
                                    const PrivateKey &privKey) 
{
  log_scope l(log.general_logger(), info_level, 
              "Connection::loginByCertificate");
  AutoLock m(&_mutex);
  bool state = _state;
  m.unlock();
  if (state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  
  idl::EncryptedBlock challenge;
  idl_ac::LoginProcess_var loginProcess;
  {
    interceptors::IgnoreInterceptor _i(_piCurrent);
    checkBusid();
    loginProcess = _access_control->startLoginByCertificate(entity.c_str(),
                                                            challenge);
  }
  
  /* decifrar o desafio usando a chave privada do usu�rio. */
  CORBA::OctetSeq secret = privKey.decrypt((unsigned char*) challenge, 
                                           idl::EncryptedBlockSize);

  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  loginAuthenticationInfo.data = secret;
  
  /* representa��o da minha chave em uma cadeia de bytes. */
  CORBA::OctetSeq bufKey = _key.pubKey();
  SHA256(bufKey.get_buffer(), bufKey.length(), loginAuthenticationInfo.hash);
  
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var 
    encodedLoginAuthenticationInfo = _codec->encode_value(any);
  
  /* cifrando a estrutura LoginAuthenticationInfo com a chave p�blica
   * do barramento. */
  CORBA::OctetSeq encrypted = 
    _buskey->encrypt(encodedLoginAuthenticationInfo->get_buffer(), 
                     encodedLoginAuthenticationInfo->length());

  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted.get_buffer(), idl::EncryptedBlockSize);
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  idl_ac::ValidityTime validityTime;    
  idl_ac::LoginInfo *loginInfo;
  try 
  {
    loginInfo = loginProcess->login(bufKey, encryptedBlock, validityTime);

  } 
  catch (idl_ac::WrongEncoding&) 
  {
    throw idl_ac::AccessDenied();
  }
  
  m.lock();
  if (_state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _state = LOGGED;

  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) 
  {
    m.unlock();
    _renewLogin->run();
    m.lock();
  } 
  else 
  {
    _renewLogin = std::auto_ptr<RenewLogin> (new RenewLogin(this, 
                                                            _access_control, 
                                                            _openbusContext, 
                                                            validityTime));
    m.unlock();
    _renewLogin->start();
    m.lock();
  }
  #else
  assert(!_renewLogin.get());
  _renewLogin = std::auto_ptr<RenewLogin> 
    (new RenewLogin(_orb, this, _access_control, _openbusContext, 
                    validityTime));
  #endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

std::pair <idl_ac::LoginProcess_ptr, idl::OctetSeq> 
Connection::startSharedAuth() 
{
  log_scope l(log.general_logger(), info_level, "Connection::startSharedAuth");
  idl::EncryptedBlock challenge;
  Connection *c = 0;
  idl_ac::LoginProcess_ptr loginProcess;
  try 
  {
    c = _openbusContext->getCurrentConnection();
    _openbusContext->setCurrentConnection(this);
    loginProcess = _access_control->startLoginBySharedAuth(challenge);
    _openbusContext->setCurrentConnection(c);
  } 
  catch (...) 
  {
    _openbusContext->setCurrentConnection(c);
    throw;
  }
  CORBA::OctetSeq secretBuf = _key.decrypt(challenge, idl::EncryptedBlockSize);
  return std::make_pair(loginProcess, secretBuf);
}
  
void Connection::loginBySharedAuth(idl_ac::LoginProcess_ptr loginProcess, 
                                   const idl::OctetSeq &secret)
{
  log_scope l(log.general_logger(), info_level, 
              "Connection::loginBySharedAuth");
  AutoLock m(&_mutex);
  bool state = _state;
  m.unlock();
  if (state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  checkBusid();  
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  loginAuthenticationInfo.data = secret;
  
  /* representa��o da minha chave em uma cadeia de bytes. */
  CORBA::OctetSeq bufKey = _key.pubKey();
  SHA256(bufKey.get_buffer(), bufKey.length(), loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var 
    encodedLoginAuthenticationInfo = _codec->encode_value(any);

  /* cifrando a estrutura LoginAuthenticationInfo com a chave p�blica
   * do barramento. */
  CORBA::OctetSeq encrypted = 
    _buskey->encrypt(encodedLoginAuthenticationInfo->get_buffer(), 
                     encodedLoginAuthenticationInfo->length());

  idl::EncryptedBlock encryptedBlock;
  memcpy(encryptedBlock, encrypted.get_buffer(), idl::EncryptedBlockSize);

  idl_ac::ValidityTime validityTime;
  idl_ac::LoginInfo *loginInfo; 
  try 
  {
    loginInfo = loginProcess->login(bufKey, encryptedBlock, validityTime);
  } 
  catch (const idl_ac::WrongEncoding &)
  {
    throw idl_ac::AccessDenied();
  }

  m.lock();
  if (_state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _state = LOGGED;

  #ifdef OPENBUS_SDK_MULTITHREAD
  if (_renewLogin.get()) 
  {
    m.unlock();
    _renewLogin->run();
    m.lock();
  } 
  else 
  {
    _renewLogin = std::auto_ptr<RenewLogin> (new RenewLogin(this, 
                                                            _access_control, 
                                                            _openbusContext, 
                                                            validityTime));
    m.unlock();
    _renewLogin->start();
    m.lock();
  }
  #else
  assert(!_renewLogin.get());
  _renewLogin = std::auto_ptr<RenewLogin> (new RenewLogin(_orb, this, 
                                                          _access_control, 
                                                          _openbusContext, 
                                                          validityTime));
  #endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

bool Connection::_logout(bool local) 
{
  bool success = false;
  AutoLock m(&_mutex);
  bool state = _state;
  m.unlock();
  if (state == LOGGED) 
  {
    #ifdef OPENBUS_SDK_MULTITHREAD
    _renewLogin->pause();
    #else
    _renewLogin.reset();
    #endif
    if (!local)
    {
      struct save_connection
      {
        save_connection(OpenBusContext &context, Connection *self)
          : context(context), old(context.getCurrentConnection())
        {
          context.setCurrentConnection(self);
        }
        ~save_connection()
        {
          context.setCurrentConnection(old);
        }

        OpenBusContext &context;
        Connection *old;
      } save_connection_(*_openbusContext, this);

      static_cast<void>(save_connection_); // avoid warnings
      try
      {
        _access_control->logout();
        success = true;
      }
      catch (CORBA::NO_PERMISSION const& e)
      { 
        success = false; 
        if(e.minor() != idl_ac::NoLoginCode)
        {
          throw;
        }
      }
    }
    m.lock();
    _loginInfo.reset();
    _state = UNLOGGED;
    m.unlock();
  } 
  else if (state == INVALID) 
  {
    m.lock();
    _loginInfo.reset();
    _state = UNLOGGED;
    m.unlock();
  } 
  else 
  {
    return false;
  }
  return success;    
}

bool Connection::logout() 
{ 
  log_scope l(log.general_logger(), info_level, "Connection::logout");
  return _logout(false); 
}

void Connection::onInvalidLogin(Connection::InvalidLoginCallback_t p) 
{ 
  AutoLock m(&_mutex);
  _onInvalidLogin = p; 
}

Connection::InvalidLoginCallback_t Connection::onInvalidLogin() 
{ 
  AutoLock m(&_mutex);
  return _onInvalidLogin; 
}

const idl_ac::LoginInfo *Connection::login() 
{
  AutoLock m(&_mutex);
  if (_state == INVALID) 
  {
    return 0;
  }
  else
  {
    return _loginInfo.get();
  }
}

const std::string Connection::busid() 
{ 
  AutoLock m(&_mutex);
//[DOUBT] isso e necessario?
  if (_state == INVALID)
  {
    std::string empty;
    return empty;
  }
  else
  {
    return _busid;
  }
}
}
