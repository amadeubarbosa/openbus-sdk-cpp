#ifndef TEC_OPENBUS_DETAIL_CLIENT_INTERCEPTOR_HPP
#define TEC_OPENBUS_DETAIL_CLIENT_INTERCEPTOR_HPP

#include <CORBA.h>

#include <tec/openbus/openbus_security_manager_impl.hpp>

namespace tec { namespace openbus { namespace detail {

struct security_client_interceptor : PortableInterceptor::ClientRequestInterceptor
{
  security_client_interceptor(openbus_security_manager_impl* security_manager);

  char* name();
  void destroy();
  void send_request(PortableInterceptor::ClientRequestInfo_ptr);
  void send_poll(PortableInterceptor::ClientRequestInfo_ptr);
  void receive_reply(PortableInterceptor::ClientRequestInfo_ptr);
  void receive_exception(PortableInterceptor::ClientRequestInfo_ptr);
  void receive_other(PortableInterceptor::ClientRequestInfo_ptr);

  openbus_security_manager_impl* security_manager;
};

} } }

#endif
