// -*- coding: iso-latin-1 -*-

#include <openbus/assistant.h>
#include <openbus/assistant/AssociativePropertySeq.h>
#include <openbus/assistant/detail/exception_logging.h>
#include <openbus/assistant/detail/register_information.h>
#include <openbus/assistant/detail/execute_with_retry.h>
#include <openbus/assistant/detail/exception_message.h>
#include <openbus/assistant/detail/create_connection_and_login.h>
#include <openbus/assistant/detail/wait_login.h>

#include <boost/bind.hpp>
#include <boost/utility/result_of.hpp>

#include <boost/chrono/include.hpp>
#include <boost/weak_ptr.hpp>

#include <iterator>

namespace openbus { namespace assistant {

namespace idl = tecgraf::openbus::core::v2_0;
namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace idl_cr = tecgraf::openbus::core::v2_0::credential;

typedef assistant_detail::register_information register_information;
typedef assistant_detail::register_container register_container;
typedef assistant_detail::register_iterator register_iterator;

struct invalid_login_callback
{
  typedef void result_type;
  result_type operator()(Connection &c, idl_ac::LoginInfo old_login
                         , boost::weak_ptr<assistant_detail::shared_state> state_weak) const;
};

struct error_creating_connection
{
  boost::function<void(std::string /*error*/)> callback;

  error_creating_connection(boost::function<void(std::string /*error*/)> callback)
    : callback(callback) {}

  typedef void result_type;
  template <typename Exception>
  result_type operator()(Exception const& e) const
  {
    if(callback)
      callback(assistant_detail::exception_message(e));
  }
};

std::auto_ptr<Connection> create_connection_simple(CORBA::ORB_var orb, std::string const& host
                                                   , unsigned short port, logger::logger& logging
                                                   , boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope l(logging, logger::info_level, "Criando conex�o");
  assistant_detail::exception_logging ex_l(l, "Failed creating connection");
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
    (orb->resolve_initial_references("OpenbusConnectionManager"));
  assert(manager != 0);

  std::auto_ptr<openbus::Connection> c = manager->createConnection(host.c_str(), port);
  l.log("Connection created");
  boost::weak_ptr<assistant_detail::shared_state> weak_state = state;
  c->onInvalidLogin(boost::bind(invalid_login_callback(), _1, _2, weak_state));
  return c;
}

std::auto_ptr<Connection> create_connection(CORBA::ORB_var orb, std::string const& host, unsigned short port
                                            , logger::logger& logging
                                            , boost::shared_ptr<assistant_detail::shared_state> state
                                            , boost::function<void(std::string)> callback
                                            , boost::optional<boost::chrono::steady_clock::time_point> timeout)
{
  if(timeout)
    return assistant_detail::execute_with_retry
      (boost::bind(&create_connection_simple, orb, host, port, boost::ref(logging), state)
       , error_creating_connection(callback)
       , *timeout
       , state->logging);
  else
    return assistant_detail::execute_with_retry
      (boost::bind(&create_connection_simple, orb, host, port, boost::ref(logging), state)
       , error_creating_connection(callback)
       , assistant_detail::wait_until_timeout_and_signal_exit(state)
       , state->logging);
}

void login_simple(Connection& c, assistant_detail::authentication_info const& info
                  , logger::logger& logging)
{
  logger::log_scope l (logging, logger::info_level, "Tentando logar");
  assistant_detail::exception_logging ex_l(l);
  assert(boost::get<assistant_detail::password_authentication_info const>(&info)
         || boost::get<assistant_detail::certificate_authentication_info const>(&info));
  if(assistant_detail::password_authentication_info const* p
     = boost::get<assistant_detail::password_authentication_info const>(&info))
  {
    l.log("Fazendo login por password");
    l.level_vlog(logger::debug_level
                 , "Fazendo login por password com usu�rio '%s' e senha '%s'"
                 , p->username.c_str(), p->password.c_str());
    c.loginByPassword(p->username.c_str(), p->password.c_str());
  }
  else if(assistant_detail::certificate_authentication_info const* p
          = boost::get<assistant_detail::certificate_authentication_info const>(&info))
  {
    l.vlog("Fazendo login por chave privada para entidade %s", p->entity.c_str());
    c.loginByCertificate(p->entity.c_str(), p->private_key);
  }
  else if(assistant_detail::shared_auth_authentication_info const* p
          = boost::get<assistant_detail::shared_auth_authentication_info const>(&info))
  {
    std::pair<idl_ac::LoginProcess_ptr, idl::OctetSeq> r = p->callback();
    c.loginBySharedAuth(r.first, r.second);
  }
}

struct login_error
{
  boost::function<void(std::string /*error*/)> callback;

  login_error(boost::function<void(std::string /*error*/)> callback)
    : callback(callback) {}

  typedef void result_type;
  template <typename Exception>
  result_type operator()(Exception const& e) const
  {
    try
    {
      if(callback)
        callback(assistant_detail::exception_message(e));
    }
    catch(...)
    {}
  }
};

void register_relogin(boost::shared_ptr<assistant_detail::shared_state>);

void invalid_login_callback::operator()(Connection &c, idl_ac::LoginInfo old_login
                                        , boost::weak_ptr<assistant_detail::shared_state> state_weak) const
{
  boost::shared_ptr<assistant_detail::shared_state> state = state_weak.lock();
  if(state)
  {
    assistant_detail::execute_with_retry
      (boost::bind(&login_simple, boost::ref(c), boost::ref(state->auth_info)
                   , boost::ref(state->logging))
       , login_error(state->login_error)
       , assistant_detail::wait_until_timeout_and_signal_exit(state)
       , state->logging);

    register_relogin(state);
  }
}

namespace assistant_detail {

std::auto_ptr<Connection> create_connection_and_login
  (CORBA::ORB_var orb, std::string const& host, unsigned short port
   , assistant_detail::authentication_info const& info
   , logger::logger& logging
   , boost::shared_ptr<assistant_detail::shared_state> state
   , boost::function<void(std::string)> error
   , boost::optional<boost::chrono::steady_clock::time_point> timeout)
{
  logger::log_scope log(state->logging, logger::debug_level, "create_connection_and_login function");
  log.vlog("Has timeout? %d", (int)!!timeout);
  std::auto_ptr<Connection> c = create_connection(orb, host, port, logging, state, error
                                                  , timeout);
  if(timeout)
  {
    log.log("Calling execute_with_retry with timeout");
    assistant_detail::execute_with_retry
      (boost::bind(&login_simple, boost::ref(*c), boost::ref(info)
                   , boost::ref(logging))
       , login_error(error)
       , *timeout, logging);
  }
  else
  {
    log.log("Calling execute_with_retry with waiting function (infinite timeout)");
    assistant_detail::execute_with_retry
      (boost::bind(&login_simple, boost::ref(*c), boost::ref(info)
                   , boost::ref(logging))
       , login_error(error)
       , assistant_detail::wait_until_timeout_and_signal_exit(state)
       , logging);
  }
  return c;
}

}

void register_component(idl_or::OfferRegistry_var offer_registry
                        , register_iterator& reg_current, register_iterator reg_last
                        , logger::logger& logging)
{
  while(reg_current != reg_last)
  {
    register_iterator current = reg_current++;
    if(!current->registered)
    {
      logger::log_scope l(logging, logger::info_level, "Registering one component");
      assistant_detail::exception_logging ex_l(l);
      offer_registry->registerService(current->component, current->properties);
      l.level_log(logger::debug_level, "Component registered");
      current->registered = true;
    }
  }
}

register_information construct_register_item(std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> const& item)
{
  register_information r = {item.first, item.second, false};
  return r;
}

void activate_RootPOA(CORBA::ORB_var orb)
{
  CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
  assert(!CORBA::is_nil(poa_obj));
  PortableServer::POA_var poa = PortableServer::POA::_narrow(poa_obj);
  assert(!CORBA::is_nil(poa));
  PortableServer::POAManager_var manager = poa->the_POAManager();
  assert(!CORBA::is_nil(manager));
  manager->activate();
}

void create_threads(boost::shared_ptr<assistant_detail::shared_state> state);

void AssistantImpl::InitWithPassword(std::string const& hostname, unsigned short port
                                     , std::string const& username, std::string const& password
                                     , int& argc, char** argv
                                     , login_error_callback_type login_error
                                     , register_error_callback_type register_error
                                     , fatal_error_callback_type fatal_error
                                     , logger::level l)
{
  CORBA::ORB_var orb = ORBInitializer(argc, argv);
  activate_RootPOA(orb);
  assistant_detail::password_authentication_info info = {username, password};
  state.reset(new assistant_detail::shared_state
              (orb, info, hostname, port, login_error, register_error, fatal_error, l));
  logger::log_scope log(state->logging, logger::info_level, "InitWithPassword");
  log.log("Constructed assistant");
  create_threads(state);
}
void AssistantImpl::InitWithPassword(std::string const& hostname, unsigned short port
                                     , std::string const& username, std::string const& password
                                     , login_error_callback_type login_error
                                     , register_error_callback_type register_error
                                     , fatal_error_callback_type fatal_error
                                     , logger::level l)
{
  int argc = 1;
  char* argv[] = {const_cast<char*>("")};
  InitWithPassword(hostname, port, username, password
                   , argc, argv, login_error
                   , register_error, fatal_error, l);
}

void AssistantImpl::InitWithPrivateKey(std::string const& hostname, unsigned short port
                                       , std::string const& entity, CORBA::OctetSeq const& private_key
                                       , int& argc, char** argv
                                       , login_error_callback_type login_error
                                       , register_error_callback_type register_error
                                       , fatal_error_callback_type fatal_error
                                       , logger::level l)
{
  CORBA::ORB_var orb = ORBInitializer(argc, argv);
  activate_RootPOA(orb);
  assistant_detail::certificate_authentication_info info = {entity, private_key};
  state.reset(new assistant_detail::shared_state
              (orb, info, hostname, port, login_error, register_error, fatal_error, l));
  logger::log_scope log(state->logging, logger::info_level, "InitWithPrivateKey");
  log.log("Constructed assistant");
  create_threads(state);
}

void AssistantImpl::InitWithPrivateKey(std::string const& hostname, unsigned short port
                                       , std::string const& entity, CORBA::OctetSeq const& private_key
                                       , login_error_callback_type login_error
                                       , register_error_callback_type register_error
                                       , fatal_error_callback_type fatal_error
                                       , logger::level l)
{
  int argc = 1;
  char* argv[] = {const_cast<char*>("")};
  InitWithPrivateKey(hostname, port, entity, private_key
                     , argc, argv, login_error
                     , register_error, fatal_error, l);
}

void AssistantImpl::InitWithSharedAuth(std::string const& hostname, unsigned short port
                                       , shared_auth_callback_type shared_auth_callback
                                       , int& argc, char** argv
                                       , login_error_callback_type login_error
                                       , register_error_callback_type register_error
                                       , fatal_error_callback_type fatal_error
                                       , logger::level l)
{
  CORBA::ORB_var orb = ORBInitializer(argc, argv);
  activate_RootPOA(orb);
  assistant_detail::shared_auth_authentication_info info = {shared_auth_callback};
  state.reset(new assistant_detail::shared_state
              (orb, info, hostname, port, login_error, register_error, fatal_error, l));
  logger::log_scope log(state->logging, logger::info_level, "InitWithSharedAuth");
  log.log("Constructed assistant");
  create_threads(state);
}

void AssistantImpl::InitWithSharedAuth(std::string const& hostname, unsigned short port
                                       , shared_auth_callback_type shared_auth_callback
                                       , login_error_callback_type login_error
                                       , register_error_callback_type register_error
                                       , fatal_error_callback_type fatal_error
                                       , logger::level l)
{
  int argc = 1;
  char* argv[] = {const_cast<char*>("")};
  InitWithSharedAuth(hostname, port, shared_auth_callback, argc, argv, login_error
                     , register_error, fatal_error, l);
}

Assistant Assistant::createWithPassword(const char* username, const char* password
                                        , const char* host, unsigned short port
                                        , int& argc, char** argv
                                        , login_error_callback_type login_error
                                        , register_error_callback_type register_error
                                        , fatal_error_callback_type fatal_error
                                        , logger::level l)
{
  Assistant assistant;
  assistant.InitWithPassword(host, port, username, password, argc, argv
                             , login_error, register_error, fatal_error, l);
  return assistant;
}
 
Assistant Assistant::createWithPrivateKey(const char* entity, const idl::OctetSeq privKey
                                          , const char* host, unsigned short port
                                          , int& argc, char** argv
                                          , login_error_callback_type login_error
                                          , register_error_callback_type register_error
                                          , fatal_error_callback_type fatal_error
                                          , logger::level l)
{
  Assistant assistant;
  assistant.InitWithPrivateKey(host, port, entity, privKey, argc, argv
                               , login_error, register_error, fatal_error, l);
  return assistant;
}

idl_or::ServicePropertySeq AssistantImpl::createFacetAndEntityProperty(const char* facet, const char* entity)
{
  idl_or::ServicePropertySeq properties;
  properties.length(2);
  properties[0].name = "openbus.component.facet";
  properties[0].value = facet;
  properties[1].name = "openbus.offer.entity";
  properties[1].value = entity;
  return properties;
}  

idl_or::ServiceOfferDescSeq findOffers_immediate
(idl_or::ServicePropertySeq properties, boost::shared_ptr<assistant_detail::shared_state> state);
idl_or::ServiceOfferDescSeq findOffers(idl_or::ServicePropertySeq properties
                                       , boost::shared_ptr<assistant_detail::shared_state> state);
idl_or::ServiceOfferDescSeq findOffers(idl_or::ServicePropertySeq properties, int retries
                                       , boost::shared_ptr<assistant_detail::shared_state> state);

idl_or::ServiceOfferDescSeq AssistantImpl::findServices
  (idl_or::ServicePropertySeq properties, int retries) const
{
  if(retries < 0)
    return assistant::findOffers(properties, state);
  else if(retries == 0)
    return assistant::findOffers_immediate(properties, state);
  else
    return assistant::findOffers(properties, retries, state);
}

idl_or::ServiceOfferDescSeq AssistantImpl::filterWorkingOffers(idl_or::ServiceOfferDescSeq offers)
{
  idl_or::ServiceOfferDescSeq result_offers;
  for(std::size_t i = 0; i != offers.length(); ++i)
  {
    try
    {
      if(!offers[i].service_ref->_non_existent())
      {
        result_offers.length(result_offers.length()+1);
        result_offers[result_offers.length()-1] = offers[i];
      }
    }
    catch(CORBA::TRANSIENT const& e)
    {
    }
    catch(CORBA::COMM_FAILURE const& e)
    {
    }
    catch(CORBA::OBJECT_NOT_EXIST const& e)
    {
      std::abort();
    }
    catch(idl::services::ServiceFailure const& e)
    {
    }
    catch(idl::services::UnauthorizedOperation const& e)
    {
    }
  }
  return result_offers;
}

CallerChain AssistantImpl::getCallerChain()
{
  assistant::assistant_detail::wait_login(state);
  return state->connection->getCallerChain();
}

void AssistantImpl::joinChain(CallerChain chain)
{
  assistant::assistant_detail::wait_login(state);
  return state->connection->joinChain(chain);
}

void AssistantImpl::exitChain()
{
  assistant::assistant_detail::wait_login(state);
  return state->connection->exitChain();
}

CallerChain AssistantImpl::getJoinedChain()
{
  assistant::assistant_detail::wait_login(state);
  return state->connection->getJoinedChain();
}

} }
