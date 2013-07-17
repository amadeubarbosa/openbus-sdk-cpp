// -*- coding: iso-8859-1-unix -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_EXECUTE_WITH_RETRY_H
#define OPENBUS_ASSISTANT_DETAIL_EXECUTE_WITH_RETRY_H

#include <log/logger.h>

#ifdef _WIN32
#include <windows.h>
#endif

#define OPENBUS_ASSISTANT_CATCH_EXCEPTIONS(x)                           \
  catch(CORBA::NO_PERMISSION const& e)                                  \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(CORBA::TRANSIENT const& e)                                      \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(CORBA::COMM_FAILURE const& e)                                   \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(CORBA::OBJECT_NOT_EXIST const& e)                               \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(idl::services::ServiceFailure const& e)                         \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(idl::services::UnauthorizedOperation const& e)                  \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(idl::services::access_control::InvalidCertificate const& e)     \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(idl::services::access_control::MissingCertificate const& e)     \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(idl::services::access_control::InvalidPublicKey const& e)       \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(idl::services::access_control::AccessDenied const& e)           \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(idl::services::offer_registry::UnauthorizedFacets const& e)     \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(idl::services::offer_registry::InvalidService const& e)         \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(idl::services::offer_registry::InvalidProperties const& e)      \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(CORBA::SystemException const& e)                                \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(CORBA::UserException const& e)                                  \
    {                                                                   \
      x(e);                                                             \
    }                                                                   \
  catch(std::exception& e)                                              \
    {                                                                   \
      x(e);                                                             \
    }

namespace openbus { namespace assistant { namespace assistant_detail {

struct wait_predicate_signalled
{
  const char* what() const throw() { return "wait_predicate_signalled"; }
};

struct timeout_error : std::exception
{
  const char* what() const throw()
  {
    return "timeout_error";
  }
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
      log.log("Trying to execute");
      return f();
    }
    OPENBUS_ASSISTANT_CATCH_EXCEPTIONS(error)

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
      log.log("Trying to execute");
      return f();
    }
    OPENBUS_ASSISTANT_CATCH_EXCEPTIONS(error)

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
#ifdef _WIN32
      ::Sleep(3000);
#else
      unsigned int t = 30;
      do { t = sleep(t); } while(t);
#endif
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
