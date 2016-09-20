// -*- coding: iso-8859-1-unix -*-

#include "openbus/connection.hpp"
#include "openbus/detail/interceptors/client.hpp"
#include "openbus/detail/interceptors/server.hpp"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include "openbus/log.hpp"
#pragma clang diagnostic pop
#include "openbus/openbus_context.hpp"
#include "openbus/detail/login_cache.hpp"
#include "openbus/detail/openssl/public_key.hpp"

#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
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

void Connection::renewLogin(
  boost::weak_ptr<Connection> conn,
  idl::access::AccessControl_ptr acs, 
  OpenBusContext &ctx,
  idl::access::ValidityTime t)
{
  log_scope l(log()->general_logger(), info_level, "Connection::renewLogin()");
  {
    boost::shared_ptr<Connection> conn_ = conn.lock();
    ctx.setCurrentConnection(conn_);
    if (conn_.unique())
      return;
  }
      
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

Connection::Connection(
  CORBA::Object_var ref,
  CORBA::ORB_ptr orb, 
  interceptors::ORBInitializer *orb_init,
  OpenBusContext &m, 
  EVP_PKEY *access_key,
  bool legacy_support)
  : _openbusContext(m)
  , _key(access_key)
  , _legacy_support(legacy_support)
  , _legacy_access_control(idl::legacy::access::AccessControl::_nil())
  , _legacy_converter(idl::legacy_support::LegacyConverter::_nil())
  , _component_ref(ref)
  , _iComponent(scs::core::IComponent::_nil())
  , _port(0)
  , _orb_init(orb_init)
  , _orb(orb)
  , _loginInfo(0)    
  , _invalid_login(0)
  , _onInvalidLogin(0)
  , _state(LOGGED_OUT)
  , _profile2login(LRUSize)
  , _login2session(LRUSize)
{
}

Connection::Connection(
  const std::string host,
  const unsigned short port, CORBA::ORB_ptr orb, 
  interceptors::ORBInitializer *orb_init,
  OpenBusContext &m, 
  EVP_PKEY *access_key,
  bool legacy_support)
  : _openbusContext(m)
  , _key(access_key)
  , _legacy_support(legacy_support)
  , _legacy_access_control(idl::legacy::access::AccessControl::_nil())
  , _legacy_converter(idl::legacy_support::LegacyConverter::_nil())
  , _iComponent(scs::core::IComponent::_nil())
  , _host(host)
  , _port(port)
  , _orb_init(orb_init)
  , _orb(orb)
  , _loginInfo(0)
  , _invalid_login(0)
  , _onInvalidLogin(0)
  , _state(LOGGED_OUT)
  , _profile2login(LRUSize)
  , _login2session(LRUSize)
{
  log_scope l(log()->general_logger(), info_level, "Connection::Connection");
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
  log_scope l(log()->general_logger(), info_level, "Connection::~Connection");
  try
  {
    _logout();
    join_renew_threads();
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

void Connection::login_init()
{
  scs::core::IComponent_var iComponent;
  idl::access::AccessControl_var access_control;
  idl::legacy::access::AccessControl_var legacy_access_control;
  idl::legacy_support::LegacyConverter_var legacy_converter;
  idl::offers::OfferRegistry_var offer_registry;
  idl::access::LoginRegistry_var login_registry;
  log_scope l(log()->general_logger(), info_level, "Connection::init");
  {
    interceptors::ignore_interceptor _i(_orb_init);
    iComponent = scs::core::IComponent::_narrow(_component_ref);
    
    CORBA::Object_var obj(
      iComponent->getFacet(idl::access::_tc_AccessControl->id()));
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
  _renew_threads[loginInfo.id.in()] = boost::thread(
    boost::bind(renewLogin,
                boost::weak_ptr<Connection>(shared_from_this()),
                _access_control, 
                boost::ref(_openbusContext), validityTime));
}

void Connection::loginByPassword(const std::string &entity, 
                                 const std::string &password,
                                 const std::string &domain) 
{
  log_scope l(log()->general_logger(), info_level, 
              "Connection::loginByPassword");
  boost::unique_lock<boost::mutex> lock(_mutex);
  if (LOGGED == _state)
  {
    throw AlreadyLoggedIn();
  }
  lock.unlock();
  login_init();  
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
  loginInfo = _access_control->loginByPassword(
    entity.c_str(),
    domain.c_str(),
    _key.pubKey(),
    encryptedBlock,
    validityTime);
  login(*loginInfo, validityTime);
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

void Connection::loginByCertificate(const std::string &entity, 
                                    EVP_PKEY *key) 
{
  log_scope l(log()->general_logger(), info_level, 
              "Connection::loginByCertificate");
  boost::unique_lock<boost::mutex> lock(_mutex);
  if (LOGGED == _state)
  {
    throw AlreadyLoggedIn();
  }
  lock.unlock();
  login_init();
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
  loginInfo = loginProcess->login(_key.pubKey(), encryptedBlock, 
                                  validityTime);
  login(*loginInfo, validityTime);
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

SharedAuthSecret
Connection::startSharedAuth() 
{
  log_scope l(log()->general_logger(), info_level, 
              "Connection::startSharedAuth");
  idl::core::EncryptedBlock challenge;
  boost::shared_ptr<Connection> conn;
  idl::access::LoginProcess_ptr login_process;
  idl::legacy::access::LoginProcess_ptr legacy_login_process(
    idl::legacy::access::LoginProcess::_nil());
  try 
  {
    conn = _openbusContext.getCurrentConnection();
    _openbusContext.setCurrentConnection(shared_from_this());
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
  log_scope l(log()->general_logger(), info_level, 
              "Connection::loginBySharedAuth");
  boost::unique_lock<boost::mutex> lock(_mutex);
  if (LOGGED == _state)
  {
    throw AlreadyLoggedIn();
  }
  lock.unlock();
  login_init();
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
  catch (const CORBA::OBJECT_NOT_EXIST &)
  {
    throw InvalidLoginProcess();
  }
  login(*loginInfo, validityTime);
  l.vlog("conn.login.id: %s", _loginInfo->id.in());
}

bool Connection::_logout(bool local) 
{
  log_scope l(log()->general_logger(), info_level, "Connection::_logout");
  boost::unique_lock<boost::mutex> lock(_mutex);
  if (LOGGED_OUT == _state)
  {
    return false;
  }
  assert(_loginInfo.get());
  idl::access::LoginInfo login = *(_loginInfo.get());
  _renew_threads[login.id.in()].interrupt();
  _loginInfo.reset();
  _invalid_login.reset();
  _iComponent = scs::core::IComponent::_nil();
  idl::access::AccessControl_var access_control(_access_control);
  _access_control = idl::access::AccessControl::_nil();
  _offer_registry = idl::offers::OfferRegistry::_nil();
  _login_registry = idl::access::LoginRegistry::_nil();
  _legacy_access_control = idl::legacy::access::AccessControl::_nil();
  _legacy_converter = idl::legacy_support::LegacyConverter::_nil();
  std::string busid(_busid);
  _busid = "";
  _loginCache.reset();
  _buskey.reset();
  interceptors::ClientInterceptor *cln_int(
    dynamic_cast<interceptors::ClientInterceptor *>(
      _orb_init->cln_interceptor.in()));
  cln_int->clear_caches();
  interceptors::ServerInterceptor *srv_int(
    dynamic_cast<interceptors::ServerInterceptor *>(
      _orb_init->srv_interceptor.in()));
  srv_int->clear_caches();
  _profile2login.clear();
  _login2session.clear();
  _state = LOGGED_OUT;
  lock.unlock();
  if (local)
  {
    return false;
  }
  bool success(false);
  struct save_state
  {
    save_state(OpenBusContext &context,
               boost::shared_ptr<Connection> self)
      : context(context)
      , previous_conn(context.getCurrentConnection())
      , previous_chain(context.getJoinedChain())
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
    boost::shared_ptr<Connection> previous_conn;
    CallerChain previous_chain;
  } save_state_(_openbusContext, shared_from_this());

  static_cast<void>(save_state_); // avoid warnings
  try
  {
    interceptors::ignore_invalid_login i(_orb_init);
    interceptors::login l(_orb_init, login);
    interceptors::busid b(_orb_init, busid);
    access_control->logout();
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
      l.level_vlog(warning_level,
                   "Falha durante chamada remota de logout: "   \
                   "CORBA::NO_PERMISSION with rep_id '%s' and minor '%d'",
                   e._rep_id(), e.minor());
      success = false;
    }
  }
  catch (const CORBA::Exception &e)
  {
    l.level_vlog(warning_level, "Falha durante chamada remota de logout: " \
                 "CORBA::Exception with rep_id '%s'",
                 e._rep_id());
    success = false;
  }
  catch (const CORBA::SystemException &e)
  {
    l.level_vlog(warning_level, "Falha durante chamada remota de logout: " \
                 "CORBA::SystemException with rep_id '%s' and minor '%d'",
                 e._rep_id(), e.minor());
    success = false;
  }
  catch (...)
  {
    l.level_vlog(warning_level,
                 "Falha desconhecida durante chamada remota de logout.");
    success = false;
  }
  return success;    
}

bool Connection::logout() 
{ 
  log_scope l(log()->general_logger(), info_level, "Connection::logout");
  return _logout(false); 
}

void Connection::onInvalidLogin(Connection::InvalidLoginCallback p) 
{ 
  boost::lock_guard<boost::mutex> lock(_mutex);;
  _onInvalidLogin = p; 
}

Connection::InvalidLoginCallback Connection::onInvalidLogin() const
{ 
  boost::lock_guard<boost::mutex> lock(_mutex);;
  return _onInvalidLogin; 
}

const idl::access::LoginInfo * Connection::login() const 
{
  boost::lock_guard<boost::mutex> lock(_mutex);;
  return _loginInfo.get();
}

const std::string Connection::busid() const
{ 
  boost::lock_guard<boost::mutex> lock(_mutex);;
  return _busid;
}

idl::access::LoginInfo Connection::get_login()
{
  log_scope l(log()->general_logger(), info_level, "Connection::get_login");
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
      boost::this_thread::disable_interruption di;
      _onInvalidLogin(shared_from_this(), invalid_login);
    }
    catch (...)
    {
      l.level_log(warning_level, 
                  "Falha na execução da callback OnInvalidLogin. " \
                  "Silenciamento de exceção lançada.");
    }
    boost::lock_guard<boost::mutex> lock(_mutex);;
    login = _loginInfo.get() ? *(_loginInfo.get()) : idl::access::LoginInfo();
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
  return login;
}

void Connection::join_renew_threads()
{
  BOOST_FOREACH(_login_thread_t::value_type &e, _renew_threads)
  {
    if (e.second.get_id() != boost::this_thread::get_id())
    {
      e.second.join();
    }
  }
}
  
}
