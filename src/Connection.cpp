// -*- coding: iso-8859-1-unix -*-
#include "openbus/Connection.hpp"
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/log.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/LoginCache.hpp"
#include "openbus/crypto/PublicKey.hpp"

#include <CORBA.h>
#include <boost/bind.hpp>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#ifndef _WIN32
  #include <unistd.h>
#endif
#include <sstream>
#include <cstring>
#include <ctime>

const std::size_t LRUSize(128);

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
  RenewLogin(CORBA::ORB_ptr o, Connection &c, idl_ac::AccessControl_ptr a, 
             OpenBusContext &m, idl_ac::ValidityTime t)
   : _orb(o), _conn(c), _access_control(a), _openbusContext(m), _validityTime(t)
  { 
    log_scope l(log().general_logger(), info_level, "RenewLogin::RenewLogin");
    _orb->dispatcher()->tm_event(this, _validityTime*1000);
  }

  ~RenewLogin() 
  {
    log_scope l(log().general_logger(), info_level, "RenewLogin::~RenewLogin");
    try
    {
      _orb->dispatcher()->remove(this, CORBA::Dispatcher::Timer);    
    }
    catch (...)
    {
      try
      {
        l.log("Exception thrown in ~RenewLogin. Ignoring exception");
      }
      catch (...)
      {
      }
    }
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
    idl_ac::ValidityTime validityTime(_validityTime);
    Connection *conn(0);
    try 
    {
      conn = _openbusContext.getCurrentConnection();
      _openbusContext.setCurrentConnection(&_conn);
      validityTime = _access_control->renew();
      _openbusContext.setCurrentConnection(conn);
    } 
    catch (const CORBA::Exception &) 
    {
      l.level_vlog(warning_level, "Falha na renovacao da credencial.");
      _openbusContext.setCurrentConnection(conn);
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

SharedAuthSecret::SharedAuthSecret()
{
}

SharedAuthSecret::SharedAuthSecret(
  const std::string &busid,
  idl_ac::LoginProcess_var login_process,
  const CORBA::OctetSeq &secret,
  boost::shared_ptr<interceptors::orb_info> orb_info)
  : busid_(busid), login_process_(login_process), secret_(secret),
    orb_info_(orb_info)
{
}
  
void SharedAuthSecret::cancel()
{
  interceptors::ignore_interceptor i (orb_info_);
  login_process_->cancel();
}

Connection::Connection(
  const std::string host, const unsigned short port, CORBA::ORB_ptr orb, 
  boost::shared_ptr<interceptors::orb_info> i, OpenBusContext &m, 
  const ConnectionProperties &props) 
  : _host(host), _port(port), _orb(orb), _orb_info(i), _codec(_orb_info->codec),
    _loginInfo(0), _invalid_login(0), _onInvalidLogin(0), _state(UNLOGGED),
    _openbusContext(m), _legacyDelegate(CALLER), _legacyEnabled(true),
    _profile2login(LRUSize), _login2session(LRUSize)
{
  log_scope l(log().general_logger(), info_level, "Connection::Connection");
  CORBA::Object_var init_ref(_orb->resolve_initial_references("PICurrent"));
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent.in()));
  std::stringstream corbaloc;
  corbaloc << "corbaloc::" << _host << ":" << _port << "/" << idl::BusObjectKey;
  CORBA::Object_var obj(_orb->string_to_object(corbaloc.str().c_str()));
  {
    interceptors::ignore_interceptor _i(_orb_info);
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
	
  _loginCache.reset(new LoginCache(_login_registry));
  {
    interceptors::ignore_interceptor _i(_orb_info);
    _busid = _access_control->busid();
    CORBA::OctetSeq_var o(_access_control->buskey());
    _buskey.reset(new PublicKey(o));
  }
  
  for (ConnectionProperties::const_iterator it = props.begin(); 
       it != props.end(); ++it)
  {
    std::pair<std::string, std::string> prop (*it);
    if (prop.first == "legacy.delegate")
    {
      if (prop.second == "originator")
      {
        _legacyDelegate = ORIGINATOR;
      }
      else if (prop.second == "caller")
      {
        _legacyDelegate = CALLER;
      }
      else
      {
        throw InvalidPropertyValue("legacy.delegate", prop.second);
      }
    }
    else if (prop.first == "legacy.disable")
    {
      _legacyEnabled = false;
    }
  }
}

Connection::~Connection() 
{ 
  log_scope l(log().general_logger(), info_level, "Connection::~Connection");
  try
  {
    _logout();
  }
  catch (...) 
  {
    try
    {
      l.log("Exception thrown in destructor. Ignoring exception");
    }
    catch (...)
    {
    }
  }
}

void Connection::login(idl_ac::LoginInfo &loginInfo, 
                       idl_ac::ValidityTime validityTime)
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  if (_state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  _loginInfo.reset(&loginInfo);
  _invalid_login.reset();
  _state = LOGGED;

#ifdef OPENBUS_SDK_MULTITHREAD
  _renewLogin = boost::thread(
    boost::bind(renewLogin, boost::ref(*this), _access_control, 
                boost::ref(_openbusContext), validityTime));
#else
  assert(!_renewLogin.get());
  _renewLogin.reset(new RenewLogin(_orb, *this, _access_control, 
                                   _openbusContext, validityTime));
#endif
}

void Connection::loginByPassword(const std::string &entity, 
                                 const std::string &password) 
{
  log_scope l(log().general_logger(), info_level, 
              "Connection::loginByPassword");
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  State state(_state);
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif
  if (state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  
  interceptors::ignore_interceptor _i(_orb_info);
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  
  std::size_t password_size(password.size());
  loginAuthenticationInfo.data.length(password_size);
  std::memcpy(loginAuthenticationInfo.data.get_buffer(),
              password.c_str(),
              password_size);
  
  SHA256(_key.pubKey().get_buffer(), _key.pubKey().length(), 
         loginAuthenticationInfo.hash);
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo(_codec->encode_value(any));

  CORBA::OctetSeq encrypted(
    _buskey->encrypt(encodedLoginAuthenticationInfo->get_buffer(), 
                     encodedLoginAuthenticationInfo->length()));

  idl::EncryptedBlock encryptedBlock;
  std::memcpy(encryptedBlock, encrypted.get_buffer(), idl::EncryptedBlockSize);
    
  idl_ac::ValidityTime validityTime;
  idl_ac::LoginInfo *loginInfo(0);
  try 
  {
    loginInfo = _access_control->loginByPassword(entity.c_str(), _key.pubKey(), 
                                                 encryptedBlock, validityTime);
  } 
  catch (const idl_ac::WrongEncoding &) 
  {
    throw idl::services::ServiceFailure();
  }
  login(*loginInfo, validityTime);
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
  State state(_state);
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
    interceptors::ignore_interceptor _i(_orb_info);
    loginProcess = _access_control->startLoginByCertificate(entity.c_str(),
                                                            challenge);
  }
  
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  loginAuthenticationInfo.data = privKey.decrypt((unsigned char*) challenge, 
                                                 idl::EncryptedBlockSize);
  
  SHA256(_key.pubKey().get_buffer(), _key.pubKey().length(), 
         loginAuthenticationInfo.hash);
  
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo(_codec->encode_value(any));
  
  CORBA::OctetSeq encrypted(_buskey->encrypt(
    encodedLoginAuthenticationInfo->get_buffer(), 
    encodedLoginAuthenticationInfo->length()));

  idl::EncryptedBlock encryptedBlock;
  std::memcpy(encryptedBlock, encrypted.get_buffer(), idl::EncryptedBlockSize);
  
  interceptors::ignore_interceptor _i(_orb_info);
  idl_ac::ValidityTime validityTime;
  idl_ac::LoginInfo *loginInfo(0);
  try 
  {
    loginInfo = loginProcess->login(_key.pubKey(), encryptedBlock, 
                                    validityTime);
  } 
  catch (const idl_ac::WrongEncoding &) 
  {
    throw idl_ac::AccessDenied();
  }
  login(*loginInfo, validityTime);
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

SharedAuthSecret
Connection::startSharedAuth() 
{
  log_scope l(log().general_logger(), info_level, 
              "Connection::startSharedAuth");
  idl::EncryptedBlock challenge;
  Connection *conn(0);
  idl_ac::LoginProcess_ptr login_process;
  try 
  {
    conn = _openbusContext.getCurrentConnection();
    _openbusContext.setCurrentConnection(this);
    login_process = _access_control->startLoginBySharedAuth(challenge);
    _openbusContext.setCurrentConnection(conn);
  } 
  catch (...) 
  {
    interceptors::ignore_interceptor i(_orb_info);
    login_process->cancel();
    _openbusContext.setCurrentConnection(conn);
    throw;
  }
  CORBA::OctetSeq secret(_key.decrypt(challenge, idl::EncryptedBlockSize));
  return SharedAuthSecret(busid(), login_process, secret, _orb_info);
}
  
void Connection::loginBySharedAuth(const SharedAuthSecret &secret)
{
  log_scope l(log().general_logger(), info_level, 
              "Connection::loginBySharedAuth");
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  State state(_state);
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif
  if (state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  
  interceptors::ignore_interceptor _i(_orb_info);
  idl_ac::LoginAuthenticationInfo loginAuthenticationInfo;
  loginAuthenticationInfo.data = secret.secret_;
  
  SHA256(_key.pubKey().get_buffer(), _key.pubKey().length(), 
         loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo(
    _orb_info->codec->encode_value(any));

  CORBA::OctetSeq encrypted(
    _buskey->encrypt(encodedLoginAuthenticationInfo->get_buffer(), 
                     encodedLoginAuthenticationInfo->length()));

  idl::EncryptedBlock encryptedBlock;
  std::memcpy(encryptedBlock, encrypted.get_buffer(), idl::EncryptedBlockSize);

  idl_ac::ValidityTime validityTime;
  idl_ac::LoginInfo *loginInfo(0);
  try 
  {
    loginInfo = secret.login_process_->login(_key.pubKey(), encryptedBlock, 
                                             validityTime);
  } 
  catch (const idl_ac::WrongEncoding &)
  {
    throw idl_ac::AccessDenied();
  }
  catch (const CORBA::OBJECT_NOT_EXIST &)
  {
    throw InvalidLoginProcess();
  }
  login(*loginInfo, validityTime);
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

bool Connection::_logout(bool local) 
{
  log_scope l(log().general_logger(), info_level, "Connection::_logout");
  bool success(false);
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::unique_lock<boost::mutex> lock(_mutex);
#endif
  State state(_state);
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();
#endif
  if (state == UNLOGGED)
  {
    return false;
  }
  else if (state == LOGGED) 
  {
  #ifdef OPENBUS_SDK_MULTITHREAD
    _renewLogin.interrupt();
    _renewLogin.join();
  #else
    _renewLogin.reset();
  #endif
    if (!local)
    {
      struct save_state
      {
        save_state(OpenBusContext &context, Connection *self)
          : context(context), previous_conn(context.getCurrentConnection()),
            previous_chain(context.getJoinedChain())
        {
          context.exitChain();
          context.setCurrentConnection(self);
        }
        ~save_state()
        {
          context.setCurrentConnection(previous_conn);
          context.joinChain(previous_chain);
        }

        OpenBusContext &context;
        Connection *previous_conn;
        CallerChain previous_chain;
      } save_state_(_openbusContext, this);

      static_cast<void>(save_state_); // avoid warnings
      try
      {
        interceptors::ignore_invalid_login i(_orb_info);        
        _access_control->logout();
        success = true;
      }
      catch (const CORBA::NO_PERMISSION &e)
      {
        if (idl_ac::InvalidLoginCode == e.minor())
        {
          success = true;
        }
        else
        {
          l.level_vlog(warning_level, "Falha durante chamada remota de logout: CORBA::NO_PERMISSION with rep_id '%s' and minor '%s'", e._repoid(), e.minor());
          success = false;
        }
      }
      catch (const CORBA::SystemException &e)
      {
        //Mico does not implement CORBA::Exception_rep_id()
        l.level_vlog(warning_level, "Falha durante chamada remota de logout: CORBA::SystemException with rep_id '%s' and minor '%s'", e._repoid(), e.minor());
        success = false;
      }
    }
  }
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.lock();
#endif
  _loginInfo.reset();
  _invalid_login.reset();
  _state = UNLOGGED;
#ifdef OPENBUS_SDK_MULTITHREAD
  lock.unlock();  
#endif
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

Connection::InvalidLoginCallback_t Connection::onInvalidLogin() const
{ 
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
  return _onInvalidLogin; 
}

const idl_ac::LoginInfo *Connection::login() const 
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
  return ((_state == INVALID) ? 0 : _loginInfo.get());
}

const std::string Connection::busid() const
{ 
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
  return ((_state == INVALID) ? std::string() : _busid);
}

idl_ac::LoginInfo Connection::get_login()
{
  log_scope l(log().general_logger(), info_level, "Connection::get_login");
  idl_ac::LoginInfo login, invalid_login;
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
    login = _loginInfo.get() ? *(_loginInfo.get()) : idl_ac::LoginInfo();
    invalid_login = _invalid_login.get() ?
      *(_invalid_login.get()) : idl_ac::LoginInfo();
  }
  if (!std::string(login.id.in()).empty())
  {
    return login;
  }
  while (!std::string(invalid_login.id.in()).empty())
  {
    if (!onInvalidLogin()) 
    {
#ifdef OPENBUS_SDK_MULTITHREAD
      boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
      _invalid_login.reset();
      break;
    }
    try
    {
      onInvalidLogin()(*this, invalid_login);
    }
    catch (...)
    {
      l.level_log(warning_level, 
                  "Falha na execucao da callback OnInvalidLogin.");
    }
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
    idl_ac::LoginInfo curr;
    if (_invalid_login.get())
    {
      curr = *_invalid_login.get();
    }
    if (!std::string(curr.id.in()).empty() && curr == invalid_login)
    {
      _invalid_login.reset();
      invalid_login = idl_ac::LoginInfo();
    }
    else
    {
      invalid_login = curr;
    }
  }
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
  return *(_loginInfo.get());
}

}
