// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_FUNCTIONAL_START_SHARED_AUTH_H
#define OPENBUS_ASSISTANT_DETAIL_FUNCTIONAL_START_SHARED_AUTH_H

namespace openbus {  namespace assistant { namespace assistant_detail { namespace functional {

struct start_shared_auth
{
  start_shared_auth(boost::shared_ptr<assistant_detail::shared_state> state)
    : state(state) {}

  static const char* name() { return "startSharedAuth"; }

  typedef std::pair<idl_ac::LoginProcess_ptr, idl::OctetSeq> result_type;
  result_type operator()() const
  {
    return state->connection->startSharedAuth();
  }

  boost::shared_ptr<assistant_detail::shared_state> state;
};

} } } }

#endif
