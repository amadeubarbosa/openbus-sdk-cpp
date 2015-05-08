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

struct OPENBUS_SDK_DECL Slot
{
  Slot(PortableInterceptor::ORBInitInfo_ptr info)
    : current_connection(info->allocate_slot_id()),
    joined_call_chain(info->allocate_slot_id()),
    signed_call_chain(info->allocate_slot_id()),
    ignore_interceptor(info->allocate_slot_id()),
    ignore_invalid_login(info->allocate_slot_id()),
    request_id(info->allocate_slot_id())
  {}
  const PortableInterceptor::SlotId current_connection, joined_call_chain,
    signed_call_chain, ignore_interceptor, ignore_invalid_login, request_id;
};

struct orb_info
{
  orb_info(PortableInterceptor::ORBInitInfo_ptr);
  PortableInterceptor::ORBInitInfo_ptr info;
  Slot slot;
  PortableInterceptor::Current_var pi_current;
};

struct OPENBUS_SDK_DECL ignore_interceptor
{
  ignore_interceptor(boost::shared_ptr<orb_info>);
  ~ignore_interceptor();
  boost::shared_ptr<orb_info> _orb_info;
};

struct OPENBUS_SDK_DECL ignore_invalid_login
{
  ignore_invalid_login(boost::shared_ptr<orb_info>);
  ~ignore_invalid_login();
  boost::shared_ptr<orb_info> _orb_info;
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
  boost::shared_ptr<orb_info> _orb_info;
  IOP::Codec_var codec;
};

}}

#endif
