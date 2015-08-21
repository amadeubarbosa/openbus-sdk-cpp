// -*- coding: iso-8859-1-unix -*-
#include "openbus/Connection.hpp"
#include "openbus/detail/interceptors/server.hpp"
#include "openbus/log.hpp"
#include "openbus/openbus_context.hpp"
#include "openbus/detail/login_cache.hpp"
#include "openbus/detail/openssl/public_key.hpp"

#include <boost/bind.hpp>
#include <boost/thread.hpp>
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

void Connection::renewLogin(Connection &conn, idl::access::AccessControl_ptr acs, 
                            OpenBusContext &ctx, idl::access::ValidityTime t)
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
    catch (const boost::thread_interrupted &)
    {
      l.level_log(info_level, "Thread Connection::renewLogin encerrada.");
      break;
    }
    catch (...)
    {
      l.level_log(warning_level, "Falha na renovacao da credencial.");
    }
  }
}

SharedAuthSecret::SharedAuthSecret()
  : login_process_(idl::access::LoginProcess::_nil())
  , legacy_login_process_(idl::legacy::access::LoginProcess::_nil())
{
}

SharedAuthSecret::SharedAuthSecret(
  const std::string &busid,
  idl::access::LoginProcess_var login_process,
  idl::legacy::access::LoginProcess_var legacy_login_process,
  const idl::core::OctetSeq &secret,
  interceptors::ORBInitializer *init)
  : busid_(busid)
  , login_process_(login_process)
  , legacy_login_process_(legacy_login_process)
  , secret_(secret)
  , orb_initializer_(init)
{
}

void SharedAuthSecret::cancel()
{
  interceptors::ignore_interceptor i(orb_initializer_);
  login_process_->cancel();
}

Connection::Connection(
  CORBA::Object_var ref,
  CORBA::ORB_ptr orb, 
  interceptors::ORBInitializer *orb_init,
  OpenBusContext &m, 
  EVP_PKEY *access_key,
  bool legacy_support)
  : _key(access_key),
    _component_ref(ref),
    _iComponent(scs::core::IComponent::_nil()),
    _port(0),
    _orb_init(orb_init),
    _orb(orb),
    _loginInfo(0),
    _invalid_login(0),
    _onInvalidLogin(0),
    _state(UNLOGGED),
    _openbusContext(m),
    _legacy_support(legacy_support),
    _legacy_access_control(idl::legacy::access::AccessControl::_nil()),
    _legacy_converter(idl::legacy_support::LegacyConverter::_nil()),
    _profile2login(LRUSize),
    _login2session(LRUSize)
{
}

Connection::Connection(
  const std::string host,
  const unsigned short port, CORBA::ORB_ptr orb, 
  interceptors::ORBInitializer *orb_init,
  OpenBusContext &m, 
  EVP_PKEY *access_key,
  bool legacy_support)
  : _key(access_key),
    _iComponent(scs::core::IComponent::_nil()),
    _host(host),
    _port(port),
    _orb_init(orb_init),
    _orb(orb),
    _loginInfo(0),
    _invalid_login(0),
    _onInvalidLogin(0),
    _state(UNLOGGED),
    _openbusContext(m),
    _legacy_support(legacy_support),
    _legacy_access_control(idl::legacy::access::AccessControl::_nil()),
    _legacy_converter(idl::legacy_support::LegacyConverter::_nil()),
    _profile2login(LRUSize),
    _login2session(LRUSize)
{
  log_scope l(log().general_logger(), info_level, "Connection::Connection");
  std::stringstream corbaloc;
  corbaloc << "corbaloc::" << _host << ":" << _port << "/" << idl::core::BusObjectKey;
  try
  {
    _component_ref = _orb->string_to_object(corbaloc.str().c_str());
  } catch (const CORBA::Exception &)
  {
    throw InvalidBusAddress();
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

void Connection::init()
{
  scs::core::IComponent_var iComponent;
  idl::access::AccessControl_var access_control;
  idl::legacy::access::AccessControl_var legacy_access_control;
  idl::legacy_support::LegacyConverter_var legacy_converter;
  idl::offers::OfferRegistry_var offer_registry;
  idl::access::LoginRegistry_var login_registry;
  log_scope l(log().general_logger(), info_level, "Connection::init");
  {
    interceptors::ignore_interceptor _i(_orb_init);
    iComponent = scs::core::IComponent::_narrow(_component_ref);
    
    CORBA::Object_var obj = iComponent->getFacet(idl::access::_tc_AccessControl->id());
    access_control = idl::access::AccessControl::_narrow(obj);
    assert(!CORBA::is_nil(access_control.in()));
    
    if (_legacy_support)
    {
      obj = iComponent->getFacetByName("LegacySupport");
      scs::core::IComponent_var bus_2_0_comp(scs::core::IComponent::_narrow(obj));
      obj = bus_2_0_comp->getFacet(idl::legacy::access::_tc_AccessControl->id());
      legacy_access_control = idl::legacy::access::AccessControl::_narrow(obj);
      assert(!CORBA::is_nil(legacy_access_control.in()));

      obj = bus_2_0_comp->getFacet(idl::legacy_support::_tc_LegacyConverter->id());
      legacy_converter = idl::legacy_support::LegacyConverter::_narrow(obj);
      assert(!CORBA::is_nil(legacy_converter.in()));
    }
    
    obj = iComponent->getFacet(idl::offers::_tc_OfferRegistry->id());
    offer_registry = idl::offers::OfferRegistry::_narrow(obj);
    assert(!CORBA::is_nil(offer_registry.in()));
    
    obj = iComponent->getFacet(idl::access::_tc_LoginRegistry->id());
    login_registry = idl::access::LoginRegistry::_narrow(obj);
    assert(!CORBA::is_nil(login_registry.in()));
    
  }
	
  std::string busid; 
  idl::core::OctetSeq_var o;
  {
    interceptors::ignore_interceptor _i(_orb_init);
    busid = access_control->busid();
    o = access_control->certificate();
  }
  openssl::pX509 crt(openssl::byteSeq2x509(o->get_buffer(), o->length()));
  openssl::pkey pub_key(X509_get_pubkey(crt.get()));
  
  boost::unique_lock<boost::mutex> m(_mutex);
  _iComponent = iComponent;
  _access_control = access_control;
  _legacy_access_control = legacy_access_control;
  _legacy_converter = legacy_converter;
  _offer_registry = offer_registry;
  _login_registry = login_registry;
  _busid = busid;
  _loginCache.reset(new LoginCache(_login_registry));
  _buskey.reset(new PublicKey(pub_key));
}

void Connection::login(idl::access::LoginInfo &loginInfo, 
                       idl::access::ValidityTime validityTime)
{
  boost::unique_lock<boost::mutex> lock(_mutex);
  if (_state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  _loginInfo.reset(&loginInfo);
  _invalid_login.reset();
  _state = LOGGED;

  _renewLogin = boost::thread(
    boost::bind(renewLogin, boost::ref(*this), _access_control, 
                boost::ref(_openbusContext), validityTime));
}

void Connection::loginByPassword(const std::string &entity, 
                                 const std::string &password,
                                 const std::string &domain) 
{
  log_scope l(log().general_logger(), info_level, 
              "Connection::loginByPassword");
  boost::unique_lock<boost::mutex> lock(_mutex);
  bool initialized(CORBA::is_nil(_iComponent));
  lock.unlock();
  if (initialized)
  {
    init();
  }
  lock.lock();
  State state(_state);
  lock.unlock();
  if (state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  
  interceptors::ignore_interceptor _i(_orb_init);
  idl::access::LoginAuthenticationInfo loginAuthenticationInfo;
  
  std::size_t password_size(password.size());
  loginAuthenticationInfo.data.length(static_cast<CORBA::ULong>(password_size));
  std::memcpy(loginAuthenticationInfo.data.get_buffer(),
              password.c_str(),
              password_size);
  
  SHA256(_key.pubKey().get_buffer(), _key.pubKey().length(), 
         loginAuthenticationInfo.hash);
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo(_orb_init->codec->encode_value(any));

  idl::core::OctetSeq encrypted(
    _buskey->encrypt(encodedLoginAuthenticationInfo->get_buffer(), 
                     encodedLoginAuthenticationInfo->length()));

  idl::core::EncryptedBlock encryptedBlock;
  std::memcpy(encryptedBlock, encrypted.get_buffer(), idl::core::EncryptedBlockSize);
    
  idl::access::ValidityTime validityTime;
  idl::access::LoginInfo *loginInfo(0);
  try 
  {
    loginInfo = _access_control->loginByPassword(
      entity.c_str(),
      domain.c_str(),
      _key.pubKey(),
      encryptedBlock,
      validityTime);
  } 
  catch (const idl::access::WrongEncoding &) 
  {
    throw idl::services::ServiceFailure();
  }
  login(*loginInfo, validityTime);
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

void Connection::loginByCertificate(const std::string &entity, 
                                    EVP_PKEY *key) 
{
  log_scope l(log().general_logger(), info_level, 
              "Connection::loginByCertificate");
  boost::unique_lock<boost::mutex> lock(_mutex);
  bool initialized(CORBA::is_nil(_iComponent));
  lock.unlock();
  if (initialized)
  {
    init();
  }
  lock.lock();
  State state(_state);
  lock.unlock();
  if (state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }
  
  idl::core::EncryptedBlock challenge;
  idl::access::LoginProcess_var loginProcess;
  {
    interceptors::ignore_interceptor _i(_orb_init);
    loginProcess = _access_control->startLoginByCertificate(entity.c_str(),
                                                            challenge);
  }
  
  idl::access::LoginAuthenticationInfo loginAuthenticationInfo;
  PrivateKey privKey(key);
  loginAuthenticationInfo.data = privKey.decrypt((unsigned char*) challenge, 
                                                 idl::core::EncryptedBlockSize);
  
  SHA256(_key.pubKey().get_buffer(), _key.pubKey().length(), 
         loginAuthenticationInfo.hash);
  
  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo(_orb_init->codec->encode_value(any));
  
  idl::core::OctetSeq encrypted(_buskey->encrypt(
    encodedLoginAuthenticationInfo->get_buffer(), 
    encodedLoginAuthenticationInfo->length()));

  idl::core::EncryptedBlock encryptedBlock;
  std::memcpy(encryptedBlock, encrypted.get_buffer(), idl::core::EncryptedBlockSize);
  
  interceptors::ignore_interceptor _i(_orb_init);
  idl::access::ValidityTime validityTime;
  idl::access::LoginInfo *loginInfo(0);
  try 
  {
    loginInfo = loginProcess->login(_key.pubKey(), encryptedBlock, 
                                    validityTime);
  } 
  catch (const idl::access::WrongEncoding &) 
  {
    throw idl::access::AccessDenied();
  }
  login(*loginInfo, validityTime);
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

SharedAuthSecret
Connection::startSharedAuth() 
{
  log_scope l(log().general_logger(), info_level, 
              "Connection::startSharedAuth");
  idl::core::EncryptedBlock challenge;
  Connection *conn(0);
  idl::access::LoginProcess_ptr login_process;
  idl::legacy::access::LoginProcess_ptr legacy_login_process;
  try 
  {
    conn = _openbusContext.getCurrentConnection();
    _openbusContext.setCurrentConnection(this);
    login_process = _access_control->startLoginBySharedAuth(challenge);
    if (_legacy_support)
    {
      legacy_login_process = _legacy_converter->convertSharedAuth(login_process);
    }
    _openbusContext.setCurrentConnection(conn);
  } 
  catch (...) 
  {
    interceptors::ignore_interceptor i(_orb_init);
    login_process->cancel();
    _openbusContext.setCurrentConnection(conn);
    throw;
  }
  idl::core::OctetSeq secret(_key.decrypt(challenge, idl::core::EncryptedBlockSize));
  return SharedAuthSecret(busid(), login_process, legacy_login_process, secret, _orb_init);
}
  
void Connection::loginBySharedAuth(const SharedAuthSecret &secret)
{
  log_scope l(log().general_logger(), info_level, 
              "Connection::loginBySharedAuth");
  boost::unique_lock<boost::mutex> lock(_mutex);
  bool initialized(CORBA::is_nil(_iComponent));
  lock.unlock();
  if (initialized)
  {
    init();
  }
  lock.lock();
  State state(_state);
  lock.unlock();
  if (state == LOGGED) 
  {
    throw AlreadyLoggedIn();
  }

  if (secret.busid() != busid())
  {
    throw WrongBus();
  }
  interceptors::ignore_interceptor _i(_orb_init);
  idl::access::LoginAuthenticationInfo loginAuthenticationInfo;
  loginAuthenticationInfo.data = secret.secret_;
  
  SHA256(_key.pubKey().get_buffer(), _key.pubKey().length(), 
         loginAuthenticationInfo.hash);

  CORBA::Any any;
  any <<= loginAuthenticationInfo;
  CORBA::OctetSeq_var encodedLoginAuthenticationInfo(
    _orb_init->codec->encode_value(any));

  idl::core::OctetSeq encrypted(
    _buskey->encrypt(encodedLoginAuthenticationInfo->get_buffer(), 
                     encodedLoginAuthenticationInfo->length()));

  idl::core::EncryptedBlock encryptedBlock;
  std::memcpy(encryptedBlock, encrypted.get_buffer(), idl::core::EncryptedBlockSize);

  idl::access::ValidityTime validityTime;
  idl::access::LoginInfo *loginInfo(0);
  try 
  {
    if (CORBA::is_nil(secret.login_process_))
    {
      idl::legacy::access::LoginInfo_var login(
        secret.legacy_login_process_->login(
          idl::legacy::core::OctetSeq(
            _key.pubKey().maximum(),
            _key.pubKey().length(),
            _key.pubKey().get_buffer()),
          encryptedBlock,
          validityTime));
      loginInfo = new idl::access::LoginInfo;
      loginInfo->id = login->id;
      loginInfo->entity = login->entity;
    }
    else
      loginInfo = secret.login_process_->login(_key.pubKey(), encryptedBlock, 
                                               validityTime);
  } 
  catch (const idl::access::WrongEncoding &)
  {
    throw idl::access::AccessDenied();
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
  boost::unique_lock<boost::mutex> lock(_mutex);
  State state(_state);
  lock.unlock();
  if (state == UNLOGGED)
  {
    return false;
  }
  else if (state == LOGGED) 
  {
    _renewLogin.interrupt();
    if (_renewLogin.get_id() != boost::this_thread::get_id())
    {
      _renewLogin.join();
    }
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
        interceptors::ignore_invalid_login i(_orb_init);        
        _access_control->logout();
        success = true;
      }
      catch (const CORBA::NO_PERMISSION &e)
      {
        if (idl::access::InvalidLoginCode == e.minor())
        {
          success = true;
        }
        else
        {
          l.level_vlog(warning_level, "Falha durante chamada remota de logout: CORBA::NO_PERMISSION with rep_id '%s' and minor '%s'", e._rep_id(), e.minor());
          success = false;
        }
      }
      catch (const CORBA::SystemException &e)
      {
        //Mico does not implement CORBA::Exception_rep_id()
        l.level_vlog(warning_level, "Falha durante chamada remota de logout: CORBA::SystemException with rep_id '%s' and minor '%s'", e._rep_id(), e.minor());
        success = false;
      }
    }
  }
  lock.lock();
  _loginInfo.reset();
  _invalid_login.reset();
  _state = UNLOGGED;
  lock.unlock();  
  return success;    
}

bool Connection::logout() 
{ 
  log_scope l(log().general_logger(), info_level, "Connection::logout");
  return _logout(false); 
}

void Connection::onInvalidLogin(Connection::InvalidLoginCallback_t p) 
{ 
  boost::lock_guard<boost::mutex> lock(_mutex);;
  _onInvalidLogin = p; 
}

Connection::InvalidLoginCallback_t Connection::onInvalidLogin() const
{ 
  boost::lock_guard<boost::mutex> lock(_mutex);;
  return _onInvalidLogin; 
}

const idl::access::LoginInfo *Connection::login() const 
{
  boost::lock_guard<boost::mutex> lock(_mutex);;
  return ((_state == INVALID) ? 0 : _loginInfo.get());
}

const std::string Connection::busid() const
{ 
  boost::lock_guard<boost::mutex> lock(_mutex);;
  return ((_state == INVALID) ? std::string() : _busid);
}

idl::access::LoginInfo Connection::get_login()
{
  log_scope l(log().general_logger(), info_level, "Connection::get_login");
  idl::access::LoginInfo login, invalid_login;
  {
    boost::lock_guard<boost::mutex> lock(_mutex);;
    login = _loginInfo.get() ? *(_loginInfo.get()) : idl::access::LoginInfo();
    invalid_login = _invalid_login.get() ?
      *(_invalid_login.get()) : idl::access::LoginInfo();
  }
  if (!std::string(login.id.in()).empty())
  {
    return login;
  }
  while (!std::string(invalid_login.id.in()).empty())
  {
    if (!onInvalidLogin()) 
    {
      boost::lock_guard<boost::mutex> lock(_mutex);;
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
    boost::lock_guard<boost::mutex> lock(_mutex);;
    idl::access::LoginInfo curr;
    if (_invalid_login.get())
    {
      curr = *_invalid_login.get();
    }
    if (!std::string(curr.id.in()).empty() && curr == invalid_login)
    {
      _invalid_login.reset();
      invalid_login = idl::access::LoginInfo();
    }
    else
    {
      invalid_login = curr;
    }
  }
  boost::lock_guard<boost::mutex> lock(_mutex);;
  return *(_loginInfo.get());
}

}
