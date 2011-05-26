#ifndef TEC_OPENBUS_UTIL_SEQUENCE_ITERATOR_HPP
#define TEC_OPENBUS_UTIL_SEQUENCE_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>

namespace tec { namespace openbus { namespace util {

template <typename Sequence>
struct sequence_iterator : boost::iterator_facade
  <sequence_iterator<Sequence>, typename Sequence::ElementType
   , std::random_access_iterator_tag>
{
  sequence_iterator(Sequence& seq, std::size_t pos = 0u)
    : seq(&seq), pos(pos)
  {}

  typedef sequence_iterator<Sequence> self_type;
  typedef typename Sequence::ElementType reference_type;

  void increment()
  {
    ++pos;
  }
  reference_type dereference() const
  {
    return (*seq)[pos];
  }
  void decrement()
  {
    --pos;
  }
  void advance(int n)
  {
    int pos_ = pos;
    pos_ += n;
    pos = pos_;
  }
  bool equal(self_type const& other) const
  {
    assert(seq == other.seq);
    return pos == other.pos;
  }

  Sequence* seq;
  std::size_t pos;
};

} } }

#endif
