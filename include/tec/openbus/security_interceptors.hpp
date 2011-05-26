#ifndef TEC_OPENBUS_SECURITY_INTERCEPTORS_HPP
#define TEC_OPENBUS_SECURITY_INTERCEPTORS_HPP

#include <tec/openbus/detail/security_client_interceptor.hpp>
#include <tec/openbus/detail/security_server_interceptor.hpp>
#include <tec/openbus/detail/orb_initializer.hpp>

#include <boost/shared_ptr.hpp>

namespace tec { namespace openbus {

struct security_interceptors
{
  security_interceptors();

  boost::shared_ptr<detail::orb_initializer> initializer;
};

} }

#endif
