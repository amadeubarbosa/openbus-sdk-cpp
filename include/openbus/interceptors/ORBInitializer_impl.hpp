// -*- coding: iso-8859-1-unix -*-
#ifndef TECGRAF_SDK_OPENBUS_ORB_INITIALIZER_IMPL_H_
#define TECGRAF_SDK_OPENBUS_ORB_INITIALIZER_IMPL_H_

#include "openbus/decl.hpp"
#include "stubs/core.h"

#include <CORBA.h>
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
namespace PI = PortableInterceptor;
struct ClientInterceptor;
struct ServerInterceptor;

struct OPENBUS_SDK_DECL Slot
{
  Slot(PI::ORBInitInfo_ptr info)
    : requester_conn(info->allocate_slot_id()),
    receive_conn(info->allocate_slot_id()),
    joined_call_chain(info->allocate_slot_id()),
    signed_call_chain(info->allocate_slot_id()),
    ignore_interceptor(info->allocate_slot_id())
  {}
  const PI::SlotId requester_conn, receive_conn, joined_call_chain, 
    signed_call_chain, ignore_interceptor;
};

struct orb_info
{
  orb_info(PI::ORBInitInfo_ptr);
  PI::ORBInitInfo_ptr info;
  Slot slot;
  IOP::Codec_var codec;
  PortableInterceptor::Current_var pi_current;
};

struct OPENBUS_SDK_DECL ignore_interceptor
{
  ignore_interceptor(boost::shared_ptr<orb_info>);
  ~ignore_interceptor();
  boost::shared_ptr<orb_info> _orb_info;
};

struct OPENBUS_SDK_DECL ORBInitializer : 
  public PortableInterceptor::ORBInitializer 
{
  ORBInitializer();
  ~ORBInitializer();
  void pre_init(PortableInterceptor::ORBInitInfo_ptr);
  void post_init(PortableInterceptor::ORBInitInfo_ptr);

  boost::shared_ptr<ClientInterceptor> clientInterceptor; 
  boost::shared_ptr<ServerInterceptor> serverInterceptor;
  boost::shared_ptr<orb_info> _orb_info;
};

}}

#endif
