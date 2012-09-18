// -*- coding: iso-latin-1 -*-

#include <openbus/assistant.h>
#include <openbus/assistant/detail/exception_logging.h>
#include <openbus/assistant/detail/register_information.h>
#include <openbus/assistant/detail/execute_with_retry.h>
#include <openbus/assistant/detail/exception_message.h>

#include <boost/bind.hpp>

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

std::auto_ptr<Connection> create_connection_and_login
  (CORBA::ORB_var orb, std::string const& host, unsigned short port
   , assistant_detail::authentication_info const& info
   , logger::logger& logging
   , boost::shared_ptr<assistant_detail::shared_state> state
   , boost::function<void(std::string)> error);

register_information construct_register_item(std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> const& item);

void register_component(idl_or::OfferRegistry_var offer_registry
                        , register_iterator& reg_current, register_iterator reg_last
                        , logger::logger& logging);

void create_threads(boost::shared_ptr<assistant_detail::shared_state> state)
{
}

void AssistantImpl::shutdown()
{
  state->work_exit = true;
  state->orb->shutdown(true);
}

void wait_login(boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope log(state->logging, logger::debug_level, "wait_login function");
  assistant_detail::exception_logging ex_l(log);

  log.level_log(logger::debug_level, "Creating connection and logging");
  std::auto_ptr<Connection> connection = create_connection_and_login
    (state->orb, state->host, state->port, state->auth_info
     , state->logging, state, state->login_error);
  {
    log.level_log(logger::debug_level, "Registering connection as default");
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (state->orb->resolve_initial_references("OpenbusConnectionManager"));
    assert(manager != 0);
    manager->setDefaultConnection(connection.get());
  }
  state->connection = connection;
  state->connection_ready = true;
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
  
}

idl_or::ServiceOfferDescSeq findOffers(idl_or::ServicePropertySeq properties, int timeout_secs
                                       , boost::shared_ptr<assistant_detail::shared_state> state)
{
  // TODO
}

idl_or::ServiceOfferDescSeq findOffers(idl_or::ServicePropertySeq properties
                                       , boost::shared_ptr<assistant_detail::shared_state> state)
{
  if(!state->connection_ready)
    wait_login(state);

  assert(!CORBA::is_nil(state->connection->offers()));
  idl_or::ServiceOfferDescSeq_var r
    = assistant_detail::execute_with_retry
    (boost::bind(find_services(), state, properties)
     , find_services_error(), assistant_detail::wait_until_timeout_and_signal_exit(state));
  return *r;
}

idl_or::ServiceOfferDescSeq findOffers_immediate
  (idl_or::ServicePropertySeq properties, boost::shared_ptr<assistant_detail::shared_state> state)
{
  if(!state->connection_ready)
    throw timeout_error();

  assert(!CORBA::is_nil(state->connection->offers()));
  idl_or::ServiceOfferDescSeq_var r = state->connection->offers()->findServices(properties);
  return *r;
}

} }
