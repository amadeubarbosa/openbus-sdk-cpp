#ifndef TEC_OPENBUS_DETAIL_SERVER_INTERCEPTOR_HPP
#define TEC_OPENBUS_DETAIL_SERVER_INTERCEPTOR_HPP

#include <tec/openbus/openbus_security_manager_impl.hpp>

#include <CORBA.h>

namespace tec { namespace openbus { namespace detail {

struct security_server_interceptor : ::PortableInterceptor::ServerRequestInterceptor
{
  security_server_interceptor(openbus_security_manager_impl* security_manager);

  char* name();
  void destroy();
  void receive_request_service_contexts( ::PortableInterceptor::ServerRequestInfo_ptr);
  void receive_request( ::PortableInterceptor::ServerRequestInfo_ptr );
  void send_reply( ::PortableInterceptor::ServerRequestInfo_ptr );
  void send_exception( ::PortableInterceptor::ServerRequestInfo_ptr );
  void send_other( ::PortableInterceptor::ServerRequestInfo_ptr );

  openbus_security_manager_impl* security_manager;
};

} } }

#endif

