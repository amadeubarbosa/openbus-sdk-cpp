// -*- coding: iso-8859-1 -*-
#include "openbus/Connection.hpp"
#include "openbus/log.hpp"
#include "openbus/OpenBusContext.hpp"

#include <CORBA.h>
#include <boost/bind.hpp>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif

#ifndef _WIN32
#include <unistd.h>
#endif
#include <sstream>
#include <ctime>

namespace openbus 
{
class Connection;

#ifdef OPENBUS_SDK_MULTITHREAD
void Connection::renewLogin(Connection &conn, idl_ac::AccessControl_ptr acs, 
                            OpenBusContext &ctx, idl_ac::ValidityTime t)
{
  log_scope l(log().general_logger(), info_level, "Connection::renewLogin()");
  ctx.setCurrentConnection(&conn);
  while (true)
  {
    try
    {
      boost::this_thread::sleep(boost::posix_time::seconds(t));
      t = acs->renew();
      l.level_log(info_level, "Credencial renovada.");
    } 
    catch (const CORBA::Exception &)
    {
      l.level_log(warning_level, "Falha na renovacao da credencial.");
    }
    catch (const boost::thread_interrupted &)
    {
      l.level_log(info_level, "Thread Connection::renewLogin encerrada.");
      break;
    }
  }
}
  
#else

class RenewLogin : public CORBA::DispatcherCallback 
{
public:
  RenewLogin(CORBA::ORB_ptr o, Connection &c, 
             idl_ac::AccessControl_ptr a, OpenBusContext &m, 
             idl_ac::ValidityTime t)
   : _orb(o), _conn(c), _access_control(a), _openbusContext(m), 
     _validityTime(t) 
  { 
    log_scope l(log().general_logger(), info_level, "RenewLogin::RenewLogin");
    _orb->dispatcher()->tm_event(this, _validityTime*1000);
  }

  ~RenewLogin() 
  {
    _orb->dispatcher()->remove(this, CORBA::Dispatcher::Timer);    
  }

  void callback(CORBA::Dispatcher *dispatcher, Event event) 
  {
    _validityTime = renew(dispatcher);
    dispatcher->tm_event(this, _validityTime*1000);
  }

  idl_ac::ValidityTime renew(CORBA::Dispatcher *dispatcher) 
  {
    log_scope l(log().general_logger(), info_level, "RenewLogin::renew");
    assert(_access_control);
    idl_ac::ValidityTime validityTime = _validityTime;
    Connection *c = 0;
    try 
    {
      c = _openbusContext.getCurrentConnection();
      _openbusContext.setCurrentConnection(&_conn);
      validityTime = _access_control->renew();
      _openbusContext.setCurrentConnection(c);
    } 
    catch (const CORBA::Exception &) 
    {
      l.level_vlog(warning_level, "Falha na renovacao da credencial.");
      _openbusContext.setCurrentConnection(c);
    }
    return validityTime;
  }
private:
  CORBA::ORB_ptr _orb;
  Connection &_conn;
  idl_ac::AccessControl_ptr _access_control;
  OpenBusContext &_openbusContext;
  idl_ac::ValidityTime _validityTime;
};
#endif

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
                       OpenBusContext &m, std::vector<std::string> props) 
  : _host(h), _port(p), _orb(orb), _codec(c), _slotId_joinedCallChain(s1), 
    _slotId_signedCallChain(s2), _slotId_legacyCallChain(s3), 
    _slotId_receiveConnection(s4), _loginInfo(0), 
    _onInvalidLogin(0), _state(UNLOGGED), _openbusContext(m), 
    _legacyDelegate(CALLER)
{
  log_scope l(log().general_logger(), info_level, "Connection::Connection");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent.in()));
  std::stringstream corbaloc;
  corbaloc << "corbaloc::" << _host << ":" << _port << "/" << idl::BusObjectKey;
  CORBA::Object_var obj = _orb->string_to_object(corbaloc.str().c_str());
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
  log_scope l(log().general_logger(), info_level, "Connection::~Connection");
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
  _renewLogin.interrupt();
  _renewLogin.join();
#endif
}

void Connection::loginByPassword(const std::string &entity, 
                                 const std::string &password) 
{
  log_scope l(log().general_logger(), info_level, "Connection::loginByPassword");
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  bool state = _state;
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif
  if (state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  checkBusid();  
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  
  /* representação do password em uma cadeia de bytes. */
  CORBA::ULong passSize = static_cast<CORBA::ULong> (password.size());
  idl::OctetSeq_var passOctetSeq = 
    new idl::OctetSeq(passSize, passSize, (CORBA::Octet *) 
                      CORBA::string_dup(password.c_str()));
  loginAuthenticationInfo.data = passOctetSeq;
  
  /* representação da minha chave em uma cadeia de bytes. */
  CORBA::OctetSeq bufKey = _key.pubKey();
  
  /* criando uma hash da minha chave. */
  SHA256(bufKey.get_buffer(), bufKey.length(), loginAuthenticationInfo.hash);

  /* CDR da estrura LoginAuthenticationInfo que foi montada acima. */
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo= _codec->encode_value(any);

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública
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

#ifdef OPENBUS_SDK_MULTITHREAD
  lock.lock();
#endif
  if (_state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _state = LOGGED;

#ifdef OPENBUS_SDK_MULTITHREAD
  _renewLogin = boost::thread(
    boost::bind(renewLogin, boost::ref(*this), _access_control, 
                boost::ref(_openbusContext), validityTime));
#else
  assert(!_renewLogin.get());
  _renewLogin = std::auto_ptr<RenewLogin> 
    (new RenewLogin(_orb, *this, _access_control, _openbusContext, 
                    validityTime));
#endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

void Connection::loginByCertificate(const std::string &entity, 
                                    const PrivateKey &privKey) 
{
  log_scope l(log().general_logger(), info_level, 
              "Connection::loginByCertificate");
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);;
#endif
  bool state = _state;
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif
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
  
  /* decifrar o desafio usando a chave privada do usuário. */
  CORBA::OctetSeq secret = privKey.decrypt((unsigned char*) challenge, 
                                           idl::EncryptedBlockSize);

  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  loginAuthenticationInfo.data = secret;
  
  /* representação da minha chave em uma cadeia de bytes. */
  CORBA::OctetSeq bufKey = _key.pubKey();
  SHA256(bufKey.get_buffer(), bufKey.length(), loginAuthenticationInfo.hash);
  
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var 
    encodedLoginAuthenticationInfo = _codec->encode_value(any);
  
  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública
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
  
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.lock();
#endif
  if (_state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _state = LOGGED;

#ifdef OPENBUS_SDK_MULTITHREAD
  _renewLogin = boost::thread(
    boost::bind(renewLogin, boost::ref(*this), _access_control, 
                boost::ref(_openbusContext), validityTime));
#else
  assert(!_renewLogin.get());
  _renewLogin = std::auto_ptr<RenewLogin> 
    (new RenewLogin(_orb, *this, _access_control, _openbusContext, 
                   validityTime));
#endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

std::pair <idl_ac::LoginProcess_ptr, idl::OctetSeq> 
Connection::startSharedAuth() 
{
  log_scope l(log().general_logger(), info_level, "Connection::startSharedAuth");
  idl::EncryptedBlock challenge;
  Connection *c = 0;
  idl_ac::LoginProcess_ptr loginProcess;
  try 
  {
    c = _openbusContext.getCurrentConnection();
    _openbusContext.setCurrentConnection(this);
    loginProcess = _access_control->startLoginBySharedAuth(challenge);
    _openbusContext.setCurrentConnection(c);
  } 
  catch (...) 
  {
    _openbusContext.setCurrentConnection(c);
    throw;
  }
  CORBA::OctetSeq secretBuf = _key.decrypt(challenge, idl::EncryptedBlockSize);
  return std::make_pair(loginProcess, secretBuf);
}
  
void Connection::loginBySharedAuth(idl_ac::LoginProcess_ptr loginProcess, 
                                   const idl::OctetSeq &secret)
{
  log_scope l(log().general_logger(), info_level, 
              "Connection::loginBySharedAuth");
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  bool state = _state;
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif
  if (state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  
  interceptors::IgnoreInterceptor _i(_piCurrent);
  checkBusid();  
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  loginAuthenticationInfo.data = secret;
  
  /* representação da minha chave em uma cadeia de bytes. */
  CORBA::OctetSeq bufKey = _key.pubKey();
  SHA256(bufKey.get_buffer(), bufKey.length(), loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var 
    encodedLoginAuthenticationInfo = _codec->encode_value(any);

  /* cifrando a estrutura LoginAuthenticationInfo com a chave pública
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

#ifdef OPENBUS_SDK_MULTITHREAD
  lock.lock();
#endif
  if (_state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  _loginInfo = std::auto_ptr<idl_ac::LoginInfo> (loginInfo);
  _state = LOGGED;

#ifdef OPENBUS_SDK_MULTITHREAD
  _renewLogin = boost::thread(
    boost::bind(renewLogin, boost::ref(*this), _access_control, 
                boost::ref(_openbusContext), validityTime));
#else
  assert(!_renewLogin.get());
  _renewLogin = std::auto_ptr<RenewLogin> (
    new RenewLogin(_orb, *this, _access_control, _openbusContext,
                   validityTime));
#endif
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

bool Connection::_logout(bool local) 
{
  bool success = false;
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  bool state = _state;
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif
  if (state == LOGGED) 
  {
    #ifdef OPENBUS_SDK_MULTITHREAD
    _renewLogin.interrupt();
    _renewLogin.join();
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
      } save_connection_(_openbusContext, this);

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
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.lock();
#endif
    _loginInfo.reset();
    _state = UNLOGGED;
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.unlock();
#endif
  } 
  else if (state == INVALID) 
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.lock();
#endif
    _loginInfo.reset();
    _state = UNLOGGED;
#ifdef OPENBUS_SDK_MULTITHREAD
    lock.unlock();
#endif
  } 
  else 
  {
    return false;
  }
  return success;    
}

bool Connection::logout() 
{ 
  log_scope l(log().general_logger(), info_level, "Connection::logout");
  return _logout(false); 
}

void Connection::onInvalidLogin(Connection::InvalidLoginCallback_t p) 
{ 
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
  _onInvalidLogin = p; 
}

Connection::InvalidLoginCallback_t Connection::onInvalidLogin() 
{ 
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
  return _onInvalidLogin; 
}

const idl_ac::LoginInfo *Connection::login() 
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
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
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
//[DOUBT] isso e necessario?
  if (_state == INVALID)
  {
    return std::string();
  }
  else
  {
    return _busid;
  }
}
}
