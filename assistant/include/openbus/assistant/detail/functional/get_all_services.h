// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_FUNCTIONAL_GET_ALL_SERVICES_H
#define OPENBUS_ASSISTANT_DETAIL_FUNCTIONAL_GET_ALL_SERVICES_H

namespace openbus {  namespace assistant { namespace assistant_detail { namespace functional {

struct get_all_services
{
  get_all_services(boost::shared_ptr<assistant_detail::shared_state> state)
    : state(state) {}

  static const char* name() { return "getAllServices"; }

  typedef idl_or::ServiceOfferDescSeq result_type;
  result_type operator()() const
  {
    return *state->connection->offers()->getAllServices();
  }

  boost::shared_ptr<assistant_detail::shared_state> state;
};

} } } }

#endif
