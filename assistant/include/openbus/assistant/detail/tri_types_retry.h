// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_TRI_TYPES_RETRY_H
#define OPENBUS_ASSISTANT_DETAIL_TRI_TYPES_RETRY_H

#include <openbus/assistant/detail/normal_error_handling_until_retry.h>

#include <boost/utility/result_of.hpp>

namespace openbus { namespace assistant { namespace assistant_detail {

struct normal_error_handling
{
  typedef void result_type;
  result_type operator()(CORBA::TRANSIENT const& e) const {}
  result_type operator()(CORBA::COMM_FAILURE const& e) const {}
  result_type operator()(CORBA::OBJECT_NOT_EXIST const& e) const {}
  result_type operator()(CORBA::NO_PERMISSION const& e) {}
  result_type operator()(idl::services::ServiceFailure const& e) {}
  result_type operator()(idl::services::UnauthorizedOperation const& e) {}
  result_type operator()(idl::services::offer_registry::UnauthorizedFacets const& e) {}
  template <typename E>
  result_type operator()(E const& e) const
  {
    throw e;
  }
};

#ifdef ASSISTANT_SDK_MULTITHREAD
template <typename Op>
typename boost::result_of<Op()>::type tri_types_retry_immediate
 (Op op, boost::shared_ptr<assistant_detail::shared_state> state)
{
  {
    boost::unique_lock<boost::mutex> l(state->mutex);
    if(!state->connection_ready)
      throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
  }

  // From here connection_ready is true, which means that state->connection is valid
  // and imutable, and a happens before has already been established between
  // state->connection and connection_ready (because of the acquire semantics of the lock
  // above and release semantics of the unlock after assignment of connection_ready)
  return op();
}

template <typename Op>
typename boost::result_of<Op()>::type tri_types_retry_determinate_retries
 (Op op, boost::shared_ptr<assistant_detail::shared_state> state, int retries)
{
  boost::unique_lock<boost::mutex> l(state->mutex);
  while(!state->connection_ready && retries > 0)
  {
    l.unlock();
    boost::this_thread::sleep_for(state->retry_wait);
    l.lock();
  }
  l.unlock();
  
  if(retries == 0)
    throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);

  // From here connection_ready is true, which means that state->connection is valid
  // and imutable, and a happens before has already been established between
  // state->connection and connection_ready (because of the acquire semantics of the lock
  // above and release semantics of the unlock after assignment of connection_ready)
  return assistant_detail::execute_with_retry
    (op
     , assistant_detail::normal_error_handling_until_retry(retries, state)
     , assistant_detail::wait_until_timeout_and_signal_exit(state)
     , state->logging);
}

template <typename Op>
typename boost::result_of<Op()>::type tri_types_retry_infinitely
 (Op op, boost::shared_ptr<assistant_detail::shared_state> state)
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
  return assistant_detail::execute_with_retry
    (op
     , normal_error_handling()
     , assistant_detail::wait_until_timeout_and_signal_exit(state)
     , state->logging);
}

#else
template <typename Op>
typename boost::result_of<Op()>::type tri_types_retry_immediate
  (Op op, boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope log(state->logging, logger::debug_level, "Executing task without retries");
  log.vlog("Task %s", Op::name());
  if(!state->connection_ready)
    throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);

  return op();
}

template <typename Op>
typename boost::result_of<Op()>::type tri_types_retry_infinitely
  (Op op, boost::shared_ptr<assistant_detail::shared_state> state)
{
  logger::log_scope log(state->logging, logger::debug_level, "Executing task with infinity retries");
  log.vlog("Task %s", Op::name());
  if(!state->connection_ready)
    wait_login(state);

  return assistant_detail::execute_with_retry
    (op
     , normal_error_handling()
     , assistant_detail::wait_until_timeout_and_signal_exit(state)
     , state->logging);
}

template <typename Op>
typename boost::result_of<Op()>::type tri_types_retry_determinate_retries
 (Op op, boost::shared_ptr<assistant_detail::shared_state> state, int retries)
{
  logger::log_scope log(state->logging, logger::debug_level, "Executing task with retries");
  log.vlog("Task %s", Op::name());
  assert(retries > 0);

  while(!state->connection_ready && retries)
  {
    log.log("Not logged in yet. Trying to login");
    boost::chrono::steady_clock::time_point timeout
      = boost::chrono::steady_clock::now()
      + state->retry_wait;

    try
    {
      wait_login(state, timeout);
    }
    catch(assistant_detail::timeout_error&)
    {
      log.log("Failed logging by timeout");
    }
  }

  if(!state->connection_ready)
    throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);

  assert(!CORBA::is_nil(state->connection->offers()));
  return assistant_detail::execute_with_retry
    (op
     , assistant_detail::normal_error_handling_until_retry(retries, state)
     , assistant_detail::wait_until_timeout_and_signal_exit(state)
     , state->logging);
}
#endif

template <typename Op>
typename boost::result_of<Op()>::type tri_types_retry
  (Op op, boost::shared_ptr<assistant_detail::shared_state> state, int retries)
{
  if(retries < 0)
    return assistant_detail::tri_types_retry_infinitely(op, state);
  else if(retries == 0)
    return assistant_detail::tri_types_retry_immediate(op, state);
  else
    return assistant_detail::tri_types_retry_determinate_retries(op, state, retries);
}

} } }

#endif
