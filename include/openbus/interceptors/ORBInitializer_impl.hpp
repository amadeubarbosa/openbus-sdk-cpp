// -*- coding: iso-8859-1-unix -*-
#ifndef TECGRAF_SDK_OPENBUS_ORB_INITIALIZER_IMPL_H_
#define TECGRAF_SDK_OPENBUS_ORB_INITIALIZER_IMPL_H_

#include "openbus/decl.hpp"
#include "coreC.h"

#include <tao/ORB.h>
#include <tao/PI/PI.h>
#include <tao/LocalObject.h>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <memory>

namespace openbus 
{
namespace idl = tecgraf::openbus::core::v2_0;
const size_t secret_size = 16;
typedef boost::array<CORBA::Octet, idl::HashValueSize> hash_value;
hash_value hash(std::string, CORBA::ULong ticket, 
                boost::array<unsigned char, secret_size> secret);

namespace interceptors 
{
struct ClientInterceptor;
struct ServerInterceptor;
struct ORBInitializer;
  
struct OPENBUS_SDK_DECL ignore_interceptor
{
  ignore_interceptor(ORBInitializer *);
  ~ignore_interceptor();
  ORBInitializer *orb_init;
};

struct OPENBUS_SDK_DECL ignore_invalid_login
{
  ignore_invalid_login(ORBInitializer *);
  ~ignore_invalid_login();
  ORBInitializer *orb_init;
};

struct OPENBUS_SDK_DECL ORBInitializer : 
  public PortableInterceptor::ORBInitializer,
  public CORBA::LocalObject
{
  ORBInitializer();
  ~ORBInitializer();
  void pre_init(PortableInterceptor::ORBInitInfo_ptr);
  void post_init(PortableInterceptor::ORBInitInfo_ptr);

  PortableInterceptor::ClientRequestInterceptor_var cln_interceptor; 
  PortableInterceptor::ServerRequestInterceptor_var srv_interceptor;
  IOP::Codec_var codec;
  PortableInterceptor::Current_var pi_current;
  PortableInterceptor::SlotId current_connection, joined_call_chain,
    signed_call_chain, ignore_interceptor, ignore_invalid_login, request_id;
};

}}

#endif
