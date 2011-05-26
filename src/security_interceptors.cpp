
#include <tec/openbus/security_interceptors.hpp>

namespace tec { namespace openbus {

security_interceptors::security_interceptors()
  : initializer(new detail::orb_initializer)
{
  PortableInterceptor::register_orb_initializer(&*initializer);
}

} }
