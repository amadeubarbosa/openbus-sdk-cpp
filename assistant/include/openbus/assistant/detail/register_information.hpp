// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_REGISTER_INFORMATION_H
#define OPENBUS_ASSISTANT_DETAIL_REGISTER_INFORMATION_H

#include <openbus/assistant/detail/exception_message.hpp>

namespace openbus { namespace assistant { namespace assistant_detail {

struct register_information
{
  scs::core::IComponent_var component;
  idl_or::ServicePropertySeq properties;
  bool registered;
};

typedef std::vector<register_information> register_container;
typedef register_container::iterator register_iterator;

struct register_fail
{
  register_error_callback_type register_error_callback;
  register_iterator* next_current;

  register_fail(register_error_callback_type register_error_callback
                , register_iterator& next_current)
    : register_error_callback(register_error_callback), next_current(&next_current) {}

  typedef void result_type;
  template <typename Exception>
  result_type operator()(Exception const& e) const
  {
    try
    {
      register_iterator current = boost::prior(*next_current);
      register_error_callback(e, current->component, current->properties);
    }
    catch(...) {}
  }
};

inline bool not_registered_predicate(register_information const& info)
{
  return !info.registered;
}

} } }

#endif
