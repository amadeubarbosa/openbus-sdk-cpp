// -*- coding: iso-latin-1 -*-

#include <openbus/assistant/detail/shared_state.h>

namespace openbus { namespace assistant {

namespace assistant_detail {

struct find_services_error_retry
{
  find_services_error_retry(int& retry, boost::shared_ptr<assistant_detail::shared_state> state)
    : retry(&retry), state(state) {}
  typedef void result_type;
  template <typename E>
  result_type operator()(E const& e) const
  {
    --*retry;
    logger::log_scope log(state->logging, logger::info_level, "Failed to find service");
    log.vlog("More %d retries to go", *retry);
    if(!*retry)
      throw e;
  }
  int* retry;
  boost::shared_ptr<assistant_detail::shared_state> state;
};

} } }
