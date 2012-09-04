// -*- coding: iso-latin-1 -*-

#include <openbus/assistant.h>
#include <openbus/assistant/AssociativePropertySeq.h>

#include <boost/bind.hpp>
#include <boost/utility/result_of.hpp>
#include <log/output/streambuf_output.h>

#include <boost/chrono/include.hpp>

#include <iterator>

namespace openbus { namespace assistant {

namespace {

const char* exception_message(CORBA::NO_PERMISSION const&)
{
  return "CORBA::NO_PERMISSION";
}

const char* exception_message(CORBA::TRANSIENT const&)
{
  return "CORBA::TRANSIENT";
}

const char* exception_message(CORBA::COMM_FAILURE const&)
{
  return "CORBA::COMM_FAILURE";
}

const char* exception_message(CORBA::OBJECT_NOT_EXIST const&)
{
  return "CORBA::OBJECT_NOT_EXIST";
}

const char* exception_message(idl::services::ServiceFailure const&)
{
  return "tecgraf::openbus::core::v2_0::services::ServiceFailure";
}

const char* exception_message(idl::services::UnauthorizedOperation const&)
{
  return "tecgraf::openbus::core::v2_0::services::UnauthorizedOperation";
}

struct wait_predicate_signalled
{
  const char* what() const throw() { return "wait_predicate_signalled"; }
};

struct wait_until_cancelled
{
  template <typename Lock, typename VarCond, typename Pred, typename TimePoint>
  void operator()(Lock& lock, VarCond& var_cond, Pred p, TimePoint time_point) const
  {
    while(!p() && var_cond.wait_until(lock, time_point) != boost::cv_status::timeout)
      ;
    if(p())
      throw wait_predicate_signalled();
  }
};

template <typename F, typename E, typename WaitF>
typename boost::result_of<F()>::type execute_with_retry(F f, E error, WaitF wait_f)
{
  do
  {
    try
    {
      return f();
    }
    catch(CORBA::NO_PERMISSION const& e)
    {
      error(e);
    }
    catch(CORBA::TRANSIENT const& e)
    {
      error(e);
    }
    catch(CORBA::COMM_FAILURE const& e)
    {
      error(e);
    }
    catch(CORBA::OBJECT_NOT_EXIST const& e)
    {
      error(e);
    }
    catch(idl::services::ServiceFailure const& e)
    {
      error(e);
    }
    catch(idl::services::UnauthorizedOperation const& e)
    {
      error(e);
    }

    wait_f();
  }
  while(true);
}

template <typename F, typename E>
typename boost::result_of<F()>::type execute_with_retry
  (F f, E error, boost::chrono::steady_clock::time_point timeout)
{
  do
  {
    try
    {
      return f();
    }
    catch(CORBA::TRANSIENT const& e)
    {
      error(e);
    }
    catch(CORBA::COMM_FAILURE const& e)
    {
      error(e);
    }
    catch(CORBA::OBJECT_NOT_EXIST const& e)
    {
      error(e);
    }
    catch(idl::services::ServiceFailure const& e)
    {
      error(e);
    }
    catch(idl::services::UnauthorizedOperation const& e)
    {
      error(e);
    }

    if(timeout <= boost::chrono::steady_clock::now())
      throw timeout_error();

    boost::chrono::steady_clock::duration 
      sleep_time = timeout - boost::chrono::steady_clock::now();
    boost::chrono::seconds sleep_time_in_secs
      = boost::chrono::duration_cast<boost::chrono::seconds>(sleep_time);

    if(sleep_time_in_secs.count() < 30)
    {
      unsigned int t = 30;
      do { t = sleep(t); } while(t);
    }
    else
      throw timeout_error();
  }
  while(true);
}

struct exception_logging
{
  logger::log_scope& l;
  std::string error_message;
  exception_logging(logger::log_scope& l, std::string error_message = std::string())
    : l(l), error_message(error_message) {}
  ~exception_logging()
  {
    try
    {
      if(std::uncaught_exception())
      {
        if(error_message.empty())
          l.level_log(logger::error_level, "A exception was thrown");
        else
          l.level_vlog(logger::error_level, "A exception was thrown: %s", error_message.c_str());
      }
    }
    catch(std::exception const&) {}
  }
};

struct wait_until_timeout_and_signal_exit
{
  wait_until_timeout_and_signal_exit(boost::shared_ptr<assistant_detail::shared_state> state)
    : state(state)
  {}

  struct predicate
  {
    typedef bool result_type;
    result_type operator()(boost::shared_ptr<assistant_detail::shared_state> state) const
    {
      return state->work_exit;
    }
  };

  typedef void result_type;
  result_type operator()() const
  {
    boost::unique_lock<boost::mutex> l(state->mutex);
    boost::chrono::steady_clock::time_point time_point
      = boost::chrono::steady_clock::now() + state->retry_wait;
    f(l, state->work_cond_var, boost::bind(predicate(), state), time_point);
  }

  boost::shared_ptr<assistant_detail::shared_state> state;
  wait_until_cancelled f;
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
      callback(exception_message(e));
  }
};

std::auto_ptr<Connection> create_connection_simple(CORBA::ORB_var orb, std::string const& host
                                                   , unsigned short port, logger::logger& logging)
{
  logger::log_scope l(logging, logger::info_level, "Criando conexão");
  exception_logging ex_l(l, "Failed creating connection");
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
    (orb->resolve_initial_references("OpenbusConnectionManager"));
  assert(manager != 0);

  std::auto_ptr<openbus::Connection> c = manager->createConnection(host.c_str(), port);
  l.log("Connection created");
  return c;
}

std::auto_ptr<Connection> create_connection(CORBA::ORB_var orb, std::string const& host, unsigned short port
                                            , logger::logger& logging
                                            , boost::shared_ptr<assistant_detail::shared_state> state
                                            , boost::function<void(std::string)> callback)
{
  return execute_with_retry(boost::bind(&create_connection_simple, orb, host, port, boost::ref(logging))
                            , error_creating_connection(callback)
                            , wait_until_timeout_and_signal_exit(state));
}

void login_simple(Connection& c, assistant_detail::authentication_info const& info
                  , logger::logger& logging)
{
  logger::log_scope l (logging, logger::info_level, "Tentando logar");
  exception_logging ex_l(l);
  assert(boost::get<assistant_detail::password_authentication_info const>(&info)
         || boost::get<assistant_detail::certificate_authentication_info const>(&info));
  if(assistant_detail::password_authentication_info const* p
     = boost::get<assistant_detail::password_authentication_info const>(&info))
  {
    l.log("Fazendo login por password");
    l.level_vlog(logger::debug_level
                 , "Fazendo login por password com usuário '%s' e senha '%s'"
                 , p->username.c_str(), p->password.c_str());
    c.loginByPassword(p->username.c_str(), p->password.c_str());
  }
  else if(assistant_detail::certificate_authentication_info const* p
          = boost::get<assistant_detail::certificate_authentication_info const>(&info))
  {
    l.vlog("Fazendo login por chave privada para entidade %s", p->entity.c_str());
    c.loginByCertificate(p->entity.c_str(), p->private_key);
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
        callback(exception_message(e));
    }
    catch(...)
    {}
  }
};

std::auto_ptr<Connection> create_connection_and_login
  (CORBA::ORB_var orb, std::string const& host, unsigned short port
   , assistant_detail::authentication_info const& info
   , logger::logger& logging
   , boost::shared_ptr<assistant_detail::shared_state> state
   , boost::function<void(std::string)> error)
{
  std::auto_ptr<Connection> c = create_connection(orb, host, port, boost::ref(logging), state, error);
  execute_with_retry(boost::bind(&login_simple, boost::ref(*c), boost::ref(info)
                                 , boost::ref(logging))
                     , login_error(error)
                     , wait_until_timeout_and_signal_exit(state));
  return c;
}

struct register_information
{
  scs::core::IComponent_var component;
  assistant_detail::idl_or::ServicePropertySeq properties;
  bool registered;
};

typedef std::vector<register_information> register_container;
typedef register_container::iterator register_iterator;

void register_component(assistant_detail::idl_or::OfferRegistry_var offer_registry
                        , register_iterator& reg_current, register_iterator reg_last
                        , logger::logger& logging)
{
  while(reg_current != reg_last)
  {
    register_iterator current = reg_current++;
    if(!current->registered)
    {
      logger::log_scope l(logging, logger::info_level, "Registering one component");
      exception_logging ex_l(l);
      offer_registry->registerService(current->component, current->properties);
      l.level_log(logger::debug_level, "Component registered");
      current->registered = true;
    }
  }
}

struct register_fail
{
  boost::function<void(scs::core::IComponent_var
                       , idl_or::ServicePropertySeq, std::string /*error*/)> register_error_callback;
  register_iterator* next_current;

  register_fail(boost::function<void(scs::core::IComponent_var
                                     , idl_or::ServicePropertySeq
                                     , std::string /*error*/)> register_error_callback
                , register_iterator& next_current)
    : register_error_callback(register_error_callback), next_current(&next_current) {}

  typedef void result_type;
  template <typename Exception>
  result_type operator()(Exception const& e) const
  {
    try
    {
      if(register_error_callback)
      {
        register_iterator current = boost::prior(*next_current);
        register_error_callback(current->component, current->properties
                                , exception_message(e));
      }
    }
    catch(...) {}
  }
};

register_information construct_register_item(std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> const& item)
{
  register_information r = {item.first, item.second, false};
  return r;
}

bool not_registered_predicate(register_information const& info)
{
  return !info.registered;
}

void work_thread_function(boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope work_thread_log(state->logging, logger::debug_level, "work_thread_function");
  exception_logging ex_l(work_thread_log);
  try
  {
    {
      boost::function<void(std::string /*error*/)> login_error_callback;
      {
        boost::unique_lock<boost::mutex> lock(state->mutex);
        login_error_callback = state->login_error;
      }

      work_thread_log.level_log(logger::debug_level, "Creating connection and logging");
      std::auto_ptr<Connection> connection = create_connection_and_login
        (state->orb, state->host, state->port, state->auth_info
         , state->logging, state
         , login_error_callback);
      {
        work_thread_log.level_log(logger::debug_level, "Registering connection as default");
        openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
          (state->orb->resolve_initial_references("OpenbusConnectionManager"));
        assert(manager != 0);
        manager->setDefaultConnection(connection.get());
      }
      assert(!!connection.get());
      boost::unique_lock<boost::mutex> lock(state->mutex);
      assert(!state->connection.get());
      state->connection = connection;
      state->connection_ready = true;
      state->connection_ready_var.notify_one();
      lock.unlock();
      work_thread_log.level_log(logger::debug_level, "Saved connection");
    }

    boost::unique_lock<boost::mutex> lock(state->mutex);
    do
    {
      register_container components;
      std::transform(state->queued_components.begin()
                     , state->queued_components.end()
                     , std::back_inserter<register_container>(components)
                     , &construct_register_item);
      std::copy(state->queued_components.begin(), state->queued_components.end()
                , std::back_inserter<std::vector<std::pair
                <scs::core::IComponent_var, idl_or::ServicePropertySeq> > >(state->components));
      state->queued_components.clear();

      boost::function<void(scs::core::IComponent_var, idl_or::ServicePropertySeq
                           , std::string /*error*/)> register_error_callback
        = state->register_error;
      lock.unlock();

      do
      {
        work_thread_log.level_log(logger::debug_level, "Registering some components");
        register_iterator current = components.begin();
        execute_with_retry(boost::bind(&register_component, state->connection->offers()
                                       , boost::ref(current), components.end()
                                       , boost::ref(state->logging))
                           , register_fail(register_error_callback, current)
                           , wait_until_timeout_and_signal_exit(state));
      }
      while(std::find_if(components.begin(), components.end()
                         , &not_registered_predicate) != components.end());

      work_thread_log.level_log(logger::debug_level, "All components registered");
      
      lock.lock();
      while(!state->new_queued_components && !state->work_exit)
      {
        work_thread_log.level_log(logger::debug_level, "Waiting for newer components to be registered");
          state->work_cond_var.wait(lock);
      }
      state->new_queued_components = false;
      if(state->work_exit)
        return;
    }
    while(true);
  }
  catch(std::bad_alloc const& e)
  {
    logger::log_scope l(state->logging, logger::error_level, "Worker thread std::bad_alloc catch");
    exception_logging ex_l(l);
  }
  catch(wait_predicate_signalled const& e)
  {
    logger::log_scope l(state->logging, logger::info_level
                        , "Worker thread was cancelled because shutdown was called");
  }
  catch(std::exception const& e)
  {
    logger::log_scope l(state->logging, logger::error_level, "Worker thread std::exception catch");
    exception_logging ex_l(l);
    try
    {
      boost::unique_lock<boost::mutex> lock(state->mutex);
      boost::function<void(const char*)> fatal_error = state->fatal_error;
      lock.unlock();
      if(fatal_error)
        fatal_error(e.what());
    }
    catch(...)
    {}
  }
  catch(...)
  {
    logger::log_scope l(state->logging, logger::error_level, "Worker thread all catch");
    exception_logging ex_l(l);
    try
    {
      boost::unique_lock<boost::mutex> lock(state->mutex);
      boost::function<void(const char*)> fatal_error = state->fatal_error;
      lock.unlock();
      if(fatal_error)
        fatal_error("Unknown exception was thrown");
    }
    catch(...)
    {}
  }
}

void orb_thread_function(CORBA::ORB_var orb
                         , boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope l(state->logging, logger::debug_level, "ORB event thread");
  exception_logging ex_l(l);
  try
  {
    orb->run();
    l.level_log(logger::info_level, "ORB returned from run");
    {
      boost::unique_lock<boost::mutex> lock(state->mutex);
      state->work_exit = true;
      state->work_cond_var.notify_one();
    }
  }
  catch(...)
  {
    l.level_log(logger::error_level, "Exception thrown in ORB thread");
    boost::unique_lock<boost::mutex> lock(state->mutex);
    state->work_exit = true;
    state->work_cond_var.notify_one();
  }
}

#ifdef ASSISTANT_SDK_MULTITHREAD
void create_threads(boost::shared_ptr<assistant_detail::shared_state> state)
{
  state->orb_thread = boost::thread(boost::bind(&assistant::orb_thread_function, state->orb, state));
  state->work_thread = boost::thread(boost::bind(&assistant::work_thread_function, state));
}
#else
#error SINGLE THREAD NOT IMPLEMENTED YET
void create_threads(boost::shared_ptr<assistant_detail::shared_state>) {}
#endif

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

}

void AssistantImpl::InitWithPassword(std::string const& hostname, unsigned short port
                                     , std::string const& username, std::string const& password
                                     , int& argc, char** argv
                                     , login_error_callback_type login_error
                                     , register_error_callback_type register_error
                                     , fatal_error_callback_type fatal_error)
{
  CORBA::ORB_var orb = ORBInitializer(argc, argv);
  activate_RootPOA(orb);
#ifndef NDEBUG
  try
  {
    assert(!CORBA::is_nil(orb->resolve_initial_references("OpenbusConnectionManager")));
  }
  catch(...)
  {
    ::std::abort();
  }
#endif  
  assistant_detail::password_authentication_info info = {username, password};
  state.reset(new assistant_detail::shared_state
              (orb, info, hostname, port, login_error, register_error, fatal_error));
  state->logging.set_level(logger::debug_level);
  state->logging.add_output(logger::output::make_streambuf_output(*std::cerr.rdbuf()));
  create_threads(state);
}
void AssistantImpl::InitWithPassword(std::string const& hostname, unsigned short port
                                     , std::string const& username, std::string const& password
                                     , login_error_callback_type login_error
                                     , register_error_callback_type register_error
                                     , fatal_error_callback_type fatal_error)
{
  int argc = 1;
  char* argv[] = {const_cast<char*>("")};
  InitWithPassword(hostname, port, username, password
                   , argc, argv, login_error
                   , register_error, fatal_error);
}

void AssistantImpl::InitWithPrivateKey(std::string const& hostname, unsigned short port
                                       , std::string const& entity, CORBA::OctetSeq const& private_key
                                       , int& argc, char** argv
                                       , login_error_callback_type login_error
                                       , register_error_callback_type register_error
                                       , fatal_error_callback_type fatal_error)
{
  CORBA::ORB_var orb = ORBInitializer(argc, argv);
  activate_RootPOA(orb);
#ifndef NDEBUG
  try
  {
    assert(!CORBA::is_nil(orb->resolve_initial_references("OpenbusConnectionManager")));
  }
  catch(...)
  {
    ::std::abort();
  }
#endif  
  assistant_detail::certificate_authentication_info info = {entity, private_key};
  state.reset(new assistant_detail::shared_state
              (orb, info, hostname, port, login_error, register_error, fatal_error));
  state->logging.set_level(logger::debug_level);
  state->logging.add_output(logger::output::make_streambuf_output(*std::cerr.rdbuf()));
  create_threads(state);
}

void AssistantImpl::InitWithPrivateKey(std::string const& hostname, unsigned short port
                                       , std::string const& entity, CORBA::OctetSeq const& private_key
                                       , login_error_callback_type login_error
                                       , register_error_callback_type register_error
                                       , fatal_error_callback_type fatal_error)
{
  int argc = 1;
  char* argv[] = {const_cast<char*>("")};
  InitWithPrivateKey(hostname, port, entity, private_key
                     , argc, argv, login_error
                     , register_error, fatal_error);
}

Assistant Assistant::createWithPassword(const char* username, const char* password
                                        , const char* host, unsigned short port
                                        , int& argc, char** argv
                                        , login_error_callback_type login_error
                                        , register_error_callback_type register_error
                                        , fatal_error_callback_type fatal_error)
{
  Assistant assistant;
  assistant.InitWithPassword(host, port, username, password, argc, argv
                             , login_error, register_error, fatal_error);
  return assistant;
}
 
Assistant Assistant::createWithPrivateKey(const char* entity, const idl::OctetSeq privKey
                                          , const char* host, unsigned short port
                                          , int& argc, char** argv
                                          , login_error_callback_type login_error
                                          , register_error_callback_type register_error
                                          , fatal_error_callback_type fatal_error)
{
  Assistant assistant;
  assistant.InitWithPrivateKey(host, port, entity, privKey, argc, argv
                               , login_error, register_error, fatal_error);
  return assistant;
}

void AssistantImpl::addOffer(scs::core::IComponent_var component, idl_or::ServicePropertySeq properties)
{
  boost::unique_lock<boost::mutex> l(state->mutex);
  state->queued_components.push_back(std::make_pair(component, properties));
  state->new_queued_components = true;
  state->work_cond_var.notify_one();
}

void AssistantImpl::shutdown()
{
  state->orb->shutdown(true);
  state->work_thread.join();
}

void AssistantImpl::wait()
{
  state->work_thread.join();
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

namespace {

struct find_services
{
  typedef idl_or::ServiceOfferDescSeq_var result_type;
  result_type operator()(boost::shared_ptr<assistant_detail::shared_state> state
                         , idl_or::ServicePropertySeq properties) const
  {
    return state->connection->offers()->findServices(properties);
  }
};

struct find_services_error
{
  typedef void result_type;
  result_type operator()(CORBA::TRANSIENT const& e) const {}
  result_type operator()(CORBA::COMM_FAILURE const& e) const {}
  result_type operator()(CORBA::OBJECT_NOT_EXIST const& e) const {}
  template <typename E>
  result_type operator()(E const& e) const
  {
    throw e;
  }
};

idl_or::ServiceOfferDescSeq findOffers(idl_or::ServicePropertySeq properties, int timeout_secs
                                       , boost::shared_ptr<assistant_detail::shared_state> state)
{
  boost::chrono::steady_clock::time_point timeout
    = boost::chrono::steady_clock::now()
    + boost::chrono::seconds(timeout_secs);

  {
    boost::unique_lock<boost::mutex> l(state->mutex);
    while(!state->connection_ready && 
          state->connection_ready_var.wait_until(l, timeout) != boost::cv_status::timeout)
      ;
    if(!state->connection_ready)
      throw timeout_error();
  }

  // From here connection_ready is true, which means that state->connection is valid
  // and imutable, and a happens before has already been established between
  // state->connection and connection_ready (because of the acquire semantics of the lock
  // above and release semantics of the unlock after assignment of connection_ready)
  idl_or::ServiceOfferDescSeq_var r
    = execute_with_retry(boost::bind(find_services(), state, properties)
                         , find_services_error(), timeout);
  return *r;
}

idl_or::ServiceOfferDescSeq findOffers(idl_or::ServicePropertySeq properties
                                       , boost::shared_ptr<assistant_detail::shared_state> state)
{
  {
    boost::unique_lock<boost::mutex> l(state->mutex);
    while(!state->connection_ready)
      state->connection_ready_var.wait(l);
    assert(state->connection_ready);
  }

  // From here connection_ready is true, which means that state->connection is valid
  // and imutable, and a happens before has already been established between
  // state->connection and connection_ready (because of the acquire semantics of the lock
  // above and release semantics of the unlock after assignment of connection_ready)
  idl_or::ServiceOfferDescSeq_var r
    = execute_with_retry(boost::bind(find_services(), state, properties)
                         , find_services_error(), wait_until_timeout_and_signal_exit(state));
  return *r;
}

idl_or::ServiceOfferDescSeq findOffers_immediate
  (idl_or::ServicePropertySeq properties, boost::shared_ptr<assistant_detail::shared_state> state)
{
  {
    boost::unique_lock<boost::mutex> l(state->mutex);
    if(!state->connection_ready)
      throw timeout_error();
  }

  // From here connection_ready is true, which means that state->connection is valid
  // and imutable, and a happens before has already been established between
  // state->connection and connection_ready (because of the acquire semantics of the lock
  // above and release semantics of the unlock after assignment of connection_ready)
  idl_or::ServiceOfferDescSeq_var r = state->connection->offers()->findServices(properties);
  return *r;
}

}

idl_or::ServiceOfferDescSeq AssistantImpl::findOffers
  (idl_or::ServicePropertySeq properties, int timeout_secs) const
{
  if(timeout_secs < 0)
    return assistant::findOffers(properties, state);
  else if(timeout_secs == 0)
    return assistant::findOffers_immediate(properties, state);
  else
    return assistant::findOffers(properties, timeout_secs, state);
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

} }
