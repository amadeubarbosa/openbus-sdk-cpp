#ifndef TEC_OPENBUS_DETAIL_ORB_INITIALIZER_HPP
#define TEC_OPENBUS_DETAIL_ORB_INITIALIZER_HPP

#include <tec/openbus/detail/security_client_interceptor.hpp>
#include <tec/openbus/detail/security_server_interceptor.hpp>

#include <CORBA.h>

namespace tec { namespace openbus { namespace detail {

struct orb_initializer : PortableInterceptor::ORBInitializer
{
  orb_initializer();

  void pre_init(PortableInterceptor::ORBInitInfo_ptr);
  void post_init(PortableInterceptor::ORBInitInfo_ptr);
};

} } }

#endif
