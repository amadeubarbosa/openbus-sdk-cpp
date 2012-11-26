// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_NORMAL_ERROR_HANDLING_UNTIL_RETRY_H
#define OPENBUS_ASSISTANT_DETAIL_NORMAL_ERROR_HANDLING_UNTIL_RETRY_H

#include <openbus/assistant/detail/shared_state.h>
#include <openbus/assistant/detail/exception_message.h>

#include <boost/function.hpp>

namespace openbus { namespace assistant {

namespace assistant_detail {

template <typename F>
struct normal_error_handling_until_retry
{
  normal_error_handling_until_retry(int& retry, boost::shared_ptr<assistant_detail::shared_state> state
                                    , F f)
    : retry(&retry), state(state), f(f) {}
  typedef void result_type;
  template <typename E>
  result_type operator()(E const& e) const
  {
    --*retry;
    logger::log_scope log(state->logging, logger::info_level, "Failed to find service");
    log.vlog("More %d retries to go", *retry);
    if(!*retry)
      throw e;
    else
      f(e);
  }
  int* retry;
  boost::shared_ptr<assistant_detail::shared_state> state;
  F f;
};

} } }

#endif
