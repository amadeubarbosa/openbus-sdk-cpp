// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_EXECUTE_WITH_RETRY_H
#define OPENBUS_ASSISTANT_DETAIL_EXECUTE_WITH_RETRY_H

#include <log/logger.h>

namespace openbus { namespace assistant { namespace assistant_detail {

struct wait_predicate_signalled
{
  const char* what() const throw() { return "wait_predicate_signalled"; }
};

template <typename F, typename E, typename WaitF>
typename boost::result_of<F()>::type execute_with_retry(F f, E error, WaitF wait_f
                                                        , logger::logger& logging)
{
  logger::log_scope log(logging, logger::debug_level, "execute_with_retry with wait function");
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
    catch(idl::services::offer_registry::UnauthorizedFacets const& e)
    {
      error(e);
    }
    catch(CORBA::SystemException const& e)
    {
      error(e);
    }
    catch(CORBA::UserException const& e)
    {
      error(e);
    }

    wait_f();
  }
  while(true);
}

template <typename F, typename E>
typename boost::result_of<F()>::type execute_with_retry
  (F f, E error, boost::chrono::steady_clock::time_point timeout
  , logger::logger& logging)
{
  logger::log_scope log(logging, logger::debug_level, "execute_with_retry with timeout");
  
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
    catch(idl::services::offer_registry::UnauthorizedFacets const& e)
    {
      error(e);
    }
    catch(CORBA::SystemException const& e)
    {
      error(e);
    }
    catch(CORBA::UserException const& e)
    {
      error(e);
    }

    if(timeout <= boost::chrono::steady_clock::now())
    {
      log.log("Timeout exceeded");
      throw timeout_error();
    }

    boost::chrono::steady_clock::duration 
      sleep_time = timeout - boost::chrono::steady_clock::now();
    boost::chrono::seconds sleep_time_in_secs
      = boost::chrono::duration_cast<boost::chrono::seconds>(sleep_time);

    if(sleep_time_in_secs.count() >= 30)
    {
      unsigned int t = 30;
      do { t = sleep(t); } while(t);
    }
    else
    {
      log.log("If we wait, timeout will exceed anyway");
      throw timeout_error();
    }
  }
  while(true);
}

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
  result_type operator()() const;

  boost::shared_ptr<assistant_detail::shared_state> state;
};

} } }

#endif
