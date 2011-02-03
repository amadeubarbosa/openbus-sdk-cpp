/*
** interceptors/ClientInterceptor.h
*/

#ifndef OPENBUS_SDK_INTERCEPTORS_CLIENT_INTERCEPTOR_H
#define OPENBUS_SDK_INTERCEPTORS_CLIENT_INTERCEPTOR_H

#include <openbus/version.h>

#include <map>
#include <string.h>
#ifdef OPENBUS_ORBIX
  #include <orbix/corba.hh>
  #include <omg/PortableInterceptor.hh>
  #include "../../stubs/orbix/access_control_service.hh"
#else
  #include <CORBA.h>
  #include "access_control_service.h"
#endif
#include <openbus/fault_tolerance_manager.h>

#include <memory>

namespace openbus {

struct orb_state;

namespace interceptors {

namespace idl_namespace = tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE;

class client_interceptor : public PortableInterceptor::ClientRequestInterceptor 
#ifdef OPENBUS_ORBIX
                         , public IT_CORBA::RefCountedLocalObject 
#endif
{
  openbus::orb_state* orb_state;
  IOP::Codec_ptr cdr_codec;
  std::auto_ptr<fault_tolerance_manager> faultToleranceManager;
  void loadOperationObjectKey();
public:
  static idl_namespace::access_control_service::Credential* credential;

  client_interceptor(IOP::Codec_ptr pcdr_codec, openbus::orb_state& orb_state);
  ~client_interceptor();
  void send_request(PortableInterceptor::ClientRequestInfo_ptr ri) 
    throw(
    CORBA::SystemException,
      PortableInterceptor::ForwardRequest);
  void send_poll(PortableInterceptor::ClientRequestInfo_ptr ri) 
    throw(CORBA::SystemException);
  void receive_reply(PortableInterceptor::ClientRequestInfo_ptr ri) 
    throw(CORBA::SystemException);
  void receive_exception(PortableInterceptor::ClientRequestInfo_ptr ri) 
    throw(
    CORBA::SystemException,
      PortableInterceptor::ForwardRequest);
  void receive_other(PortableInterceptor::ClientRequestInfo_ptr ri) 
    throw(
    CORBA::SystemException,
      PortableInterceptor::ForwardRequest);
  char* name() 
    throw(CORBA::SystemException);
  void destroy();
};

} }

#endif
