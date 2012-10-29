// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_FUNCTIONAL_FIND_SERVICES_H
#define OPENBUS_ASSISTANT_DETAIL_FUNCTIONAL_FIND_SERVICES_H

namespace openbus {  namespace assistant { namespace assistant_detail { namespace functional {

struct find_services
{
  find_services(boost::shared_ptr<assistant_detail::shared_state> state
                , idl_or::ServicePropertySeq properties)
    : state(state), properties(properties) {}

  static const char* name() { return "findServices"; }

  typedef idl_or::ServiceOfferDescSeq result_type;
  result_type operator()() const
  {
    return *state->connection->offers()->findServices(properties);
  }

  boost::shared_ptr<assistant_detail::shared_state> state;
  idl_or::ServicePropertySeq properties;
};

} } } }

#endif
