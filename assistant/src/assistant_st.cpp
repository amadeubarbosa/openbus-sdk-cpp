// -*- coding: iso-latin-1 -*-

#include <openbus/assistant.h>
#include <openbus/assistant/waitlogin.h>
#include <openbus/assistant/detail/exception_logging.h>
#include <openbus/assistant/detail/register_information.h>
#include <openbus/assistant/detail/execute_with_retry.h>
#include <openbus/assistant/detail/exception_message.h>
#include <openbus/assistant/detail/create_connection_and_login.h>

#include <boost/bind.hpp>
#include <boost/weak_ptr.hpp>

namespace openbus { namespace assistant {

typedef assistant_detail::register_information register_information;
typedef assistant_detail::register_container register_container;
typedef assistant_detail::register_iterator register_iterator;
typedef assistant_detail::register_fail register_fail;

namespace assistant_detail {

struct wait_until_cancelled
{
  template <typename Pred, typename TimeDuration>
  void operator()(Pred p, TimeDuration time_duration) const
  {
    int seconds = boost::chrono::duration_cast<boost::chrono::seconds>
      (time_duration).count();
    while(!p() && seconds)
      seconds = ::sleep(seconds);
    if(p())
      throw wait_predicate_signalled();
  }
};

void wait_until_timeout_and_signal_exit::operator()() const
{
  wait_until_cancelled f;
  f(boost::bind(predicate(), state), state->retry_wait);
}

}

void AssistantImpl::shutdown()
{
  state->work_exit = true;
  state->orb->shutdown(true);
}

namespace assistant_detail {

struct register_error_handler
{
  register_error_handler(boost::shared_ptr<shared_state> state
                         , std::size_t& i)
    : state(state), i(&i) {}

  template <typename E>
  void operator()(E const& e)
  {
    if(!state->relogin) // i is still valid
    {
      state->register_error()(e, state->queued_components[*i].first
                              , state->queued_components[*i].second);
      ++*i;
    }
  }

  boost::shared_ptr<shared_state> state;
  std::size_t* i;
};

void register_queued_components(boost::shared_ptr<shared_state> state)
{
  typedef std::vector<std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> >
    component_container_type;
  component_container_type& queued_components = state->queued_components;

  // We use indexes instead of iterators because the call to registerService
  // can invalidate iterators and pointers to component_container_type
  // if it calls register_relogin. But, register_relogin guarantees that
  // The queued_components elements before it being called are kept there, but with
  // pointers to them invalidated.
  // registerService is the only function that can invalidate have queued_components
  // modified in this function.
  std::size_t i = 0;
  while(i != queued_components.size())
  {
    register_error_handler error_handler(state, i);
    try
    {
      logger::log_scope l(state->logging, logger::info_level, "Registering one component");
      assistant_detail::exception_logging ex_l(l);

      // This function can invalidate queued_components pointers and iterators

      openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
        (state->orb->resolve_initial_references("OpenBusContext"));
      openbusContext->getOfferRegistry()->registerService(queued_components[i].first, queued_components[i].second);

      if(state->relogin) // queued_components were modified
      {
        state->relogin = false;
        continue; // retry this registration
      }
      else
      {
        // Succesful, remove from queued_components
        state->components.push_back(queued_components[i]);
        queued_components.erase(boost::next(queued_components.begin(), i));
      }
    }
    OPENBUS_ASSISTANT_CATCH_EXCEPTIONS(error_handler)
  }
}

class add_offers_dispatcher : public CORBA::DispatcherCallback
{
public:
  add_offers_dispatcher(CORBA::ORB_var orb, boost::weak_ptr<shared_state> state)
    : orb(orb), state_(state) {}
  void callback(CORBA::Dispatcher*, Event)
  {
    if(boost::shared_ptr<shared_state> state = state_.lock())
    {
      register_queued_components(state);
      if(!state->queued_components.empty())
      {
        boost::chrono::milliseconds s = boost::chrono::duration_cast
          <boost::chrono::milliseconds>(state->retry_wait);
        state->orb->dispatcher()->tm_event(this, s.count());
      }
      else
      {
        state->orb->dispatcher()->remove(this, CORBA::Dispatcher::Timer);
      }
    }
  }
private:
  CORBA::ORB_var orb;
  boost::weak_ptr<shared_state> state_;
};

void create_add_offers_dispatcher(boost::shared_ptr<shared_state> state
                                  , boost::optional<boost::chrono::milliseconds> s = boost::none)
{
  if(!state->asynchronous_offers_dispatcher)
  {
    int wait = s? s->count() : 0u;
    std::auto_ptr<add_offers_dispatcher> dispatcher(new add_offers_dispatcher(state->orb, state));
    state->orb->dispatcher()->tm_event(dispatcher.get(), wait);
    state->asynchronous_offers_dispatcher = dispatcher.release();
  }
}

void simple_add_offer_error(scs::core::IComponent_var component, idl_or::ServicePropertySeq properties
                            , boost::shared_ptr<shared_state> state)
{
  state->queued_components.push_back(std::make_pair(component, properties));
  create_add_offers_dispatcher(state, boost::chrono::duration_cast<boost::chrono::milliseconds>
                               (state->retry_wait));
}

void wait_login(boost::shared_ptr<assistant_detail::shared_state> state
                , boost::optional<boost::chrono::steady_clock::time_point> timeout = boost::none)
{
  logger::log_scope log(state->logging, logger::debug_level, "wait_login function");
  assistant_detail::exception_logging ex_l(log);

  log.vlog("Has timeout? %d", (int)!!timeout);

  log.log("Creating connection and logging");
  std::auto_ptr<Connection> connection = assistant_detail::create_connection_and_login
    (state->orb, state->host, state->port, state->auth_info
     , state->logging, state, state->login_error(), timeout);
  {
    log.log("Registering connection as default");
    openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (state->orb->resolve_initial_references("OpenBusContext"));
    assert(openbusContext != 0);
    openbusContext->setDefaultConnection(connection.get());
  }
  state->connection = connection;
  state->connection_ready = true;
  assert(!state->asynchronous_offers_dispatcher);
  register_queued_components(state);
  if(!state->queued_components.empty())
    create_add_offers_dispatcher(state);
}

}

std::auto_ptr<Connection> create_connection_simple(CORBA::ORB_var orb, std::string const& host
                                                   , unsigned short port, logger::logger& logging
                                                   , boost::shared_ptr<assistant_detail::shared_state> state);
void login_simple(Connection& c, assistant_detail::authentication_info const& info
                  , logger::logger& logging);

namespace assistant_detail {

void try_wait_login(boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope log(state->logging, logger::debug_level, "try_wait_login function");
  assistant_detail::exception_logging ex_l(log);

  log.log("Creating connection and logging");
  std::auto_ptr<Connection> connection = create_connection_simple
    (state->orb, state->host, state->port, state->logging, state);
  login_simple(*connection, state->auth_info, state->logging);
  {
    log.log("Registering connection as default");
    openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (state->orb->resolve_initial_references("OpenBusContext"));
    assert(openbusContext != 0);
    openbusContext->setDefaultConnection(connection.get());
  }
  state->connection = connection;
  state->connection_ready = true;
  assert(!state->asynchronous_offers_dispatcher);
  register_queued_components(state);
  if(!state->queued_components.empty())
    create_add_offers_dispatcher(state);
}

class login_dispatcher : public CORBA::DispatcherCallback
{
public:
  login_dispatcher(CORBA::ORB_var orb, boost::weak_ptr<shared_state> state)
    : orb(orb), state_(state) {}

  template <typename E>
  void error(E const& e)
  {
    boost::shared_ptr<shared_state> state = state_.lock();
    assert(!!state);
    state->login_error()(e);
  }

  void callback(CORBA::Dispatcher*, Event)
  {
    if(boost::shared_ptr<shared_state> state = state_.lock())
    {
      logger::log_scope log(state->logging, logger::debug_level, "try login callback function");
      if(!state->connection_ready)
      {
        try
        {
          try_wait_login(state);
        }
        OPENBUS_ASSISTANT_CATCH_EXCEPTIONS(error)
        if(!state->connection_ready)
        {
          boost::chrono::milliseconds s = boost::chrono::duration_cast
            <boost::chrono::milliseconds>(state->retry_wait);
          log.vlog("Logging failed, rescheduling login in %d milliseconds", (int)s.count());
          state->orb->dispatcher()->tm_event(this, s.count());
        }
      }
      else
        orb->dispatcher()->remove(this, CORBA::Dispatcher::Timer);
    }
    else
      orb->dispatcher()->remove(this, CORBA::Dispatcher::Timer);
  }
private:
  CORBA::ORB_var orb;
  boost::weak_ptr<shared_state> state_;
};

}

// PRE: Q = state->queued_components /\ C = state->components /\ AS = state->asynchronous_offers_dispatcher
// POS: \forall i. i < Q.size(): Q[i] == state->queued_components
//       /\ state->components.empty()
//       /\ \forall i. i < C.size(): state->queued_components[i + Q.size()] == C[i]
//       /\ (AS == 0 => state->asynchronous_offers_dispatcher = new add_offers_dispatcher)
void register_relogin(boost::shared_ptr<assistant_detail::shared_state> state)
{
  state->relogin = true;
  state->queued_components.insert
    (state->queued_components.end()
     , state->components.begin(), state->components.end());
  state->components.clear();
  if(!state->queued_components.empty() && !state->asynchronous_offers_dispatcher)
  {
    std::auto_ptr<assistant_detail::add_offers_dispatcher>
      dispatcher(new assistant_detail::add_offers_dispatcher(state->orb, state));
    state->orb->dispatcher()->tm_event(dispatcher.get(), 0);
    state->asynchronous_offers_dispatcher = dispatcher.release();
  }
}

void create_threads(boost::shared_ptr<assistant_detail::shared_state> state)
{
  assert(state->asynchronous_login_dispatcher == 0);
  std::auto_ptr<assistant_detail::login_dispatcher>
    dispatcher(new assistant_detail::login_dispatcher(state->orb, state));
  state->orb->dispatcher()->tm_event(dispatcher.get(), 0);
  state->asynchronous_login_dispatcher = dispatcher.release();
}

struct assistant_access
{
  static boost::shared_ptr<assistant_detail::shared_state> state(Assistant a)
  {
    return a.state;
  }
};

void waitLogin(Assistant a)
{
  assistant_detail::wait_login(assistant_access::state(a));
}

void AssistantImpl::registerService(scs::core::IComponent_var component, idl_or::ServicePropertySeq properties)
{
  try
  {
    if(state->connection_ready)
    {
      logger::log_scope l(state->logging, logger::info_level, "Synchronous try registering one component");
      assistant_detail::exception_logging ex_l(l);
      openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
        (state->orb->resolve_initial_references("OpenBusContext"));
      openbusContext->getOfferRegistry()->registerService(component, properties);
      state->components.push_back(std::make_pair(component, properties));
    }
    else
    {
      state->queued_components.push_back(std::make_pair(component, properties));
      if(!state->asynchronous_offers_dispatcher && !state->asynchronous_login_dispatcher)
      {
        std::auto_ptr<assistant_detail::add_offers_dispatcher>
          dispatcher(new assistant_detail::add_offers_dispatcher(state->orb, state));
        state->orb->dispatcher()->tm_event(dispatcher.get(), 0);
        state->asynchronous_offers_dispatcher = dispatcher.release();
      }
    }
  }
  catch(CORBA::TRANSIENT const& e)
  {
    simple_add_offer_error(component, properties, state);
  }
  catch(CORBA::COMM_FAILURE const& e)
  {
    simple_add_offer_error(component, properties, state);
  }
  catch(CORBA::OBJECT_NOT_EXIST const& e)
  {
    simple_add_offer_error(component, properties, state);
  }
}

} }
