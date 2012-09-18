// -*- coding: iso-latin-1 -*-

#ifndef OPENBUS_ASSISTANT_DETAIL_REGISTER_INFORMATION_H
#define OPENBUS_ASSISTANT_DETAIL_REGISTER_INFORMATION_H

#include <openbus/assistant/detail/exception_message.h>

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
  boost::function<void(scs::core::IComponent_var
                       , idl_or::ServicePropertySeq, std::string /*error*/)> register_error_callback;
  register_iterator* next_current;

  register_fail(boost::function<void(scs::core::IComponent_var
                                     , idl_or::ServicePropertySeq
                                     , std::string /*error*/)> register_error_callback
                , register_iterator& next_current)
    : register_error_callback(register_error_callback), next_current(&next_current) {}

  typedef void result_type;
  template <typename Exception>
  result_type operator()(Exception const& e) const
  {
    try
    {
      if(register_error_callback)
      {
        register_iterator current = boost::prior(*next_current);
        register_error_callback(current->component, current->properties
                                , exception_message(e));
      }
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
