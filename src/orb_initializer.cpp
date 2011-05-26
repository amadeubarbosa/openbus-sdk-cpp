
#include <tec/openbus/detail/orb_initializer.hpp>
#include <tec/openbus/detail/security_client_interceptor.hpp>
#include <tec/openbus/detail/security_server_interceptor.hpp>
#include <tec/openbus/openbus_security_manager_impl.hpp>

namespace tec { namespace openbus { namespace detail {

orb_initializer::orb_initializer()
{
  std::cout << "orb_initializer::orb_initializer " << this << std::endl;
}

void orb_initializer::pre_init(PortableInterceptor::ORBInitInfo_ptr info)
{
  std::cout << "orb_initializer::pre_init " << this << std::endl;

  // openbus_security_manager is a LocalObject
  openbus_security_manager_impl* security_manager = 
    new openbus_security_manager_impl;

  info->register_initial_reference("OpenbusSecurityManager", security_manager);

  PortableInterceptor::ClientRequestInterceptor_var client_interceptor
    = new detail::security_client_interceptor(security_manager);
  info->add_client_request_interceptor(client_interceptor);

  PortableInterceptor::ServerRequestInterceptor_var server_interceptor
    = new detail::security_server_interceptor(security_manager);
  info->add_server_request_interceptor(server_interceptor);
}

void orb_initializer::post_init(PortableInterceptor::ORBInitInfo_ptr)
{
}

} } }
