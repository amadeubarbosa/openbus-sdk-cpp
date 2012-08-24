#ifndef OPENBUS_EXTENSION_ASSOCIATIVEPROPERTYSEQ_H
#define OPENBUS_EXTENSION_ASSOCIATIVEPROPERTYSEQ_H

#include <openbus/extension/corba/SequenceRange.h>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/identity.hpp>

namespace openbus { namespace extension {

namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;
#ifndef OPENBUS_EXTENSION_DOXYGEN
namespace AssociativePropertySeq_detail {

struct service_property_name_less
{
  typedef bool result_type;
  result_type operator()(idl_or::ServiceProperty const& lhs, idl_or::ServiceProperty const& rhs) const
  {
    return std::strcmp(lhs.name, rhs.name) < 0;
  }
};
  
typedef boost::multi_index_container
 <idl_or::ServiceProperty
  , boost::multi_index::indexed_by
  <
      boost::multi_index::ordered_non_unique
      <boost::multi_index::identity<idl_or::ServiceProperty>
       , service_property_name_less>
      , boost::multi_index::sequenced<>
  > > container_type;

}
#endif

struct AssociativePropertySeq
#ifndef OPENBUS_EXTENSION_DOXYGEN
 : AssociativePropertySeq_detail::container_type
#endif
{
#ifndef OPENBUS_EXTENSION_DOXYGEN
  typedef AssociativePropertySeq_detail::container_type base_type;
#endif
  AssociativePropertySeq(idl_or::ServicePropertySeq const& seq)
  {
    corba::SequenceRangeIterator<idl_or::ServicePropertySeq const
      , idl_or::ServiceProperty const> first (seq, 0)
      , last (seq, seq.length());
    base_type::nth_index<1>::type& index = base_type::get<1>();
    index.insert(index.end(), first, last);
  }

  typedef base_type::nth_index<0u>::type iterator;
  typedef base_type::nth_index<0u>::type const_iterator;

  const_iterator begin() const { return index().begin(); }
  iterator begin() { return index().begin(); }
  const_iterator end() const { return index().end(); }
  iterator end() { return index().end(); }

  const_iterator find(const char* name) const { return index().find(name); }
  iterator find(const char* name) { return index().find(name); }

  std::pair<const_iterator, const_iterator> equal_range (const char* name) const
  { return index().equal_range(name); }
  std::pair<iterator, iterator> equal_range (const char* name)
  { return index().equal_range(name); }

  iterator upper_bound (const char* name)
  { return index().upper_bound(name); }
  const_iterator upper_bound (const char* name) const
  { return index().upper_bound(name); }

  iterator lower_bound (const char* name)
  { return index().lower_bound(name); }
  const_iterator lower_bound (const char* name) const
  { return index().lower_bound(name); }

private:
  typedef base_type::nth_index<0u>::type index_type;
  index_type const& index() const { return base_type::get<0u>(); }
  index_type& index() { return base_type::get<0u>(); }
};

} }

#endif
