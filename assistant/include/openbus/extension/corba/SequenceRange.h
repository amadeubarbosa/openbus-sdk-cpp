#ifndef OPENBUS_EXTENSION_CORBA_SEQUENCE_RANGE_H
#define OPENBUS_EXTENSION_CORBA_SEQUENCE_RANGE_H

#include <boost/iterator/iterator_facade.hpp>

#include <CORBA.h>

namespace openbus { namespace extension { namespace corba {

template <typename Seq, typename E>
struct SequenceRangeIterator : boost::iterator_facade
<SequenceRangeIterator<Seq, E>, E, std::random_access_iterator_tag>
{
  SequenceRangeIterator(Seq& seq, CORBA::ULong i)
    : seq(&seq), index(i)
  {}
private:
#ifndef OPENBUS_EXTENSION_DOXYGEN
  friend class boost::iterator_core_access;
#endif

  void increment()
  {
    ++index;
  }
  void advance(std::ptrdiff_t i)
  {
    index += i;
  }
  void decrement()
  {
    --index;
  }
  std::ptrdiff_t distance_to(SequenceRangeIterator const& other) const
  {
    return other.index - index;
  }
  bool equal(SequenceRangeIterator const& other) const
  {
    assert(seq == other.seq);
    return other.index == index;
  }
  E& dereference() const
  {
    return (*seq)[index];
  }

  Seq* seq;
  CORBA::ULong index;
};

} } }

#endif
