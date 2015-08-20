
// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_ANY_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_ANY_HPP

namespace openbus {
  
template<typename T>
T extract(CORBA::Any_var any)
{
  const T *tmp;
  if (*any >>= tmp)
  {
    return (*tmp);
  }
  return T();
}

}
#endif
