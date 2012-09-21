// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_CREATE_CONNECTION_AND_LOGIN_H
#define OPENBUS_ASSISTANT_DETAIL_CREATE_CONNECTION_AND_LOGIN_H

#include <openbus/assistant/detail/shared_state.h>

#include <boost/chrono/include.hpp>
#include <boost/optional.hpp>
#include <boost/none.hpp>

namespace openbus { namespace assistant { namespace assistant_detail {

std::auto_ptr<Connection> create_connection_and_login
  (CORBA::ORB_var orb, std::string const& host, unsigned short port
   , assistant_detail::authentication_info const& info
   , logger::logger& logging
   , boost::shared_ptr<assistant_detail::shared_state> state
   , boost::function<void(std::string)> error
   , boost::optional<boost::chrono::steady_clock::time_point> timeout);

} } }

#endif
