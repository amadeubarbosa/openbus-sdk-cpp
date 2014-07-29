// -*- coding: iso-8859-1-unix -*-

#if 0
//#ifndef OPENBUS_ASSISTANT_REFERENCE_H
#define OPENBUS_ASSISTANT_REFERENCE_H

#include <scs/IComponent.h>
#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <CORBA.h>

#include <boost/shared_ptr.hpp>

namespace openbus { namespace assistant {

namespace idl = tecgraf::openbus::core::v2_0;
namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace idl_cr = tecgraf::openbus::core::v2_0::credential;

template <typename T>
struct searchable_ref
{
  typedef bool(T::*safe_bool_type)() const;
  typedef searchable_ref<T> self_type;

  searchable_ref(T ref)
    : ref(new T(ref)) {}

  T operator->() const
  {
    return ref_;
  }

  operator safe_bool_type() const
  {
    safe_bool_type null_ = 0;
    return is_empty()?null_:&self_type::is_empty;
  }
private:
  bool is_empty() const { return CORBA::is_nil(ref_); }

  boost::shared_ptr<T> ref;
};

template <typename T, typename U>
searchable_ref<T> narrow(searchable_ref<U> r)
{
  
}

} }

#endif
