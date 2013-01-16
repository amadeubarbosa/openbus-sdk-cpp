// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_WAIT_LOGIN_H
#define OPENBUS_ASSISTANT_DETAIL_WAIT_LOGIN_H

#include <openbus/assistant/detail/shared_state.h>

#include <boost/chrono/include.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>

namespace openbus { namespace assistant { namespace assistant_detail {

#ifdef ASSISTANT_SDK_MULTITHREAD
void wait_login(boost::shared_ptr<assistant_detail::shared_state> state);
#else
void wait_login(boost::shared_ptr<assistant_detail::shared_state> state
                , boost::optional<boost::chrono::steady_clock::time_point> timeout = boost::none);
#endif

} } }

#endif
