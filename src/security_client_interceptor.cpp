
#include <tec/openbus/detail/security_client_interceptor.hpp>
#include <tec/openbus/service_context_constants.hpp>

#include <cstring>

namespace tec { namespace openbus { namespace detail {

security_client_interceptor::security_client_interceptor(openbus_security_manager_impl* security_manager)
  : security_manager(security_manager)
{
}

char* security_client_interceptor::name()
{
  return CORBA::string_dup("tec::sdk::detail::security_client_interceptor::openbus");
}

void security_client_interceptor::destroy()
{
}

void security_client_interceptor::send_request(PortableInterceptor::ClientRequestInfo_ptr)
{
  std::cout << "send_request" << std::endl;
}

void security_client_interceptor::send_poll(PortableInterceptor::ClientRequestInfo_ptr)
{
  std::cout << "send_poll" << std::endl;
}

void security_client_interceptor::receive_reply(PortableInterceptor::ClientRequestInfo_ptr info)
{
  std::cout << "receive_reply" << std::endl;
}

void security_client_interceptor::receive_exception(PortableInterceptor::ClientRequestInfo_ptr info)
{
  CORBA::String_var exception_id = info->received_exception_id();
  std::cout << "receive_exception " << std::endl;
  std::cout << "exception id: " << exception_id.in() << std::endl;

  if(!std::strcmp(exception_id.in(), "IDL:tecgraf/openbus/core/no_permission:1.0"))
  {
    std::cout << "Is a no_permission exception" << std::endl;

    tecgraf::openbus::core::no_permission exception;
    CORBA::Any_var exception_any = info->received_exception();
    exception_any >>= exception;

    PortableInterceptor::ForwardRequest forward_request(info->target(), false);
    throw forward_request;
  }
  else
    std::cout << "Some other exception, let it through" << std::endl;
}

void security_client_interceptor::receive_other(PortableInterceptor::ClientRequestInfo_ptr)
{
  std::cout << "receive_other" << std::endl;
}

} } }
