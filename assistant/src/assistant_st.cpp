// -*- coding: iso-latin-1 -*-

#include <openbus/assistant.h>
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
  template <typename Pred, typename TimePoint>
  void operator()(Pred p, TimePoint time_point) const
  {
    int seconds = boost::chrono::duration_cast<boost::chrono::seconds>
      (time_point - boost::chrono::steady_clock::now()).count();
    while(!p() && seconds)
      seconds = ::sleep(seconds);
    if(p())
      throw wait_predicate_signalled();
  }
};

void wait_until_timeout_and_signal_exit::operator()() const
{
  wait_until_cancelled f;
  boost::chrono::steady_clock::time_point time_point
    = boost::chrono::steady_clock::now() + state->retry_wait;
  f(boost::bind(predicate(), state), time_point);
}

}

void create_threads(boost::shared_ptr<assistant_detail::shared_state> state)
{
}

void AssistantImpl::shutdown()
{
  state->work_exit = true;
  state->orb->shutdown(true);
}

void register_relogin(boost::shared_ptr<assistant_detail::shared_state> state)
{
  state->queued_components.insert
    (state->queued_components.end()
     , state->components.begin(), state->components.end());
  state->components.clear();
    boost::chrono::microseconds s = boost::chrono::duration_cast
      <boost::chrono::microseconds>(state->retry_wait);
    std::auto_ptr<add_offers_dispatcher> dispatcher(new add_offers_dispatcher(state->orb, state));
    state->orb->dispatcher()->tm_event(dispatcher.get(), s.count());
    state->asynchronous_dispatcher = dispatcher.release();
  }
}

namespace assistant_detail {

void register_queued_components(boost::shared_ptr<shared_state> state)
{
  typedef std::vector<std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> >
    component_container_type;
  component_container_type& queued_components = state->queued_components;
  std::size_t i = 0;
  while(i != queued_components.size())
  {
    try
    {
      logger::log_scope l(state->logging, logger::info_level, "Registering one component");
      assistant_detail::exception_logging ex_l(l);
      state->connection->offers()
        ->registerService(queued_components[i].first, queued_components[i].second);
      state->components.push_back(queued_components[i]);
      queued_components.erase(boost::next(queued_components.begin(), i));
    }
    catch(CORBA::TRANSIENT const& e)
    {
      ++i;
    }
    catch(CORBA::COMM_FAILURE const& e)
    {
      ++i;
    }
    catch(CORBA::OBJECT_NOT_EXIST const& e)
    {
      ++i;
    }
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
        boost::chrono::microseconds s = boost::chrono::duration_cast
          <boost::chrono::microseconds>(state->retry_wait);
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

void simple_add_offer_error(scs::core::IComponent_var component, idl_or::ServicePropertySeq properties
                            , boost::shared_ptr<shared_state> state)
{
  state->queued_components.push_back(std::make_pair(component, properties));
  if(!state->asynchronous_dispatcher)
  {
    boost::chrono::microseconds s = boost::chrono::duration_cast
      <boost::chrono::microseconds>(state->retry_wait);
    std::auto_ptr<add_offers_dispatcher> dispatcher(new add_offers_dispatcher(state->orb, state));
    state->orb->dispatcher()->tm_event(dispatcher.get(), s.count());
    state->asynchronous_dispatcher = dispatcher.release();
  }
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
     , state->logging, state, state->login_error, timeout);
  {
    log.log("Registering connection as default");
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (state->orb->resolve_initial_references("OpenbusConnectionManager"));
    assert(manager != 0);
    manager->setDefaultConnection(connection.get());
  }
  state->connection = connection;
  state->connection_ready = true;
  assert(!state->asynchronous_dispatcher);
  register_queued_components(state);
  if(!state->queued_components.empty())
  {
    boost::chrono::microseconds s = boost::chrono::duration_cast
      <boost::chrono::microseconds>(state->retry_wait);
    std::auto_ptr<add_offers_dispatcher> dispatcher(new add_offers_dispatcher(state->orb, state));
    state->orb->dispatcher()->tm_event(dispatcher.get(), s.count());
    state->asynchronous_dispatcher = dispatcher.release();
  }
}

}

void AssistantImpl::wait()
{
  wait_login(state);
  if(!state->work_exit)
    state->orb->run();
}

void AssistantImpl::waitLogin()
{
  wait_login(state);
}

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

void AssistantImpl::addOffer(scs::core::IComponent_var component, idl_or::ServicePropertySeq properties)
{
  try
  {
    if(state->connection_ready)
    {
      logger::log_scope l(state->logging, logger::info_level, "Synchronous try registering one component");
      assistant_detail::exception_logging ex_l(l);
      state->connection->offers()->registerService(component, properties);
      state->components.push_back(std::make_pair(component, properties));
    }
    else
      state->queued_components.push_back(std::make_pair(component, properties));
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

idl_or::ServiceOfferDescSeq findOffers(idl_or::ServicePropertySeq properties, int timeout_secs
                                       , boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope log(state->logging, logger::debug_level, "findOffers with timeout");
  boost::chrono::steady_clock::time_point timeout
    = boost::chrono::steady_clock::now()
    + boost::chrono::seconds(timeout_secs);

  if(!state->connection_ready)
    wait_login(state, timeout);

  assert(!CORBA::is_nil(state->connection->offers()));
  idl_or::ServiceOfferDescSeq_var r
    = assistant_detail::execute_with_retry
    (boost::bind(find_services(), state, properties)
     , find_services_error(), timeout, state->logging);
  return *r;
}

idl_or::ServiceOfferDescSeq findOffers(idl_or::ServicePropertySeq properties
                                       , boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope log(state->logging, logger::debug_level, "findOffers with infinity timeout");
  if(!state->connection_ready)
    wait_login(state);

  assert(!CORBA::is_nil(state->connection->offers()));
  idl_or::ServiceOfferDescSeq_var r
    = assistant_detail::execute_with_retry
    (boost::bind(find_services(), state, properties)
     , find_services_error(), assistant_detail::wait_until_timeout_and_signal_exit(state)
     , state->logging);
  return *r;
}

idl_or::ServiceOfferDescSeq findOffers_immediate
  (idl_or::ServicePropertySeq properties, boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope log(state->logging, logger::debug_level, "findOffers with immediate timeout");
  if(!state->connection_ready)
    throw timeout_error();

  assert(!CORBA::is_nil(state->connection->offers()));
  idl_or::ServiceOfferDescSeq_var r = state->connection->offers()->findServices(properties);
  return *r;
}

} }
