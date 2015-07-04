// -*- condig: iso-8859-1-unix

#ifndef TECGRAF_SDK_OPENBUS_TESTS_CHECK_HPP
#define TECGRAF_SDK_OPENBUS_TESTS_CHECK_HPP

#include <string>
#include <iostream>

namespace openbus { namespace tests {
    
template <typename T>
void is_equal(T l, T r, const std::string &l_exp, const std::string &r_exp)
{
  if (l != r)
  {
    std::cerr << l_exp << " != " << r_exp << std::endl;
    std::cerr << l_exp << " == " << l << std::endl;
    std::cerr << r_exp << " == " << r << std::endl;
    std::abort();
  }
}

}}

#endif
