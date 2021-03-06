// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_INTERCEPTORS_INITIALIZER_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_INTERCEPTORS_INITIALIZER_HPP

#include "openbus/log.hpp"
#include "openbus/idl.hpp"
#include "openbus/detail/decl.hpp"
#include "openbus/detail/openssl/openssl.hpp"

#include <tao/ORB.h>
#include <tao/PI/PI.h>
#include <tao/LocalObject.h>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/scoped_array.hpp>
#include <boost/cstdint.hpp>
#include <boost/type_traits/is_same.hpp>
#include <memory>

namespace openbus 
{
const size_t secret_size = 16;
typedef boost::array<CORBA::Octet, idl::core::HashValueSize> hash_value;

template <typename C>
hash_value hash(std::string operation,
                CORBA::ULong ticket, 
                boost::array<unsigned char, secret_size> secret)
{
  size_t size(sizeof(idl::core::MajorVersion) + sizeof(idl::core::MinorVersion) 
    + secret_size + sizeof(CORBA::ULong) /* ticket */ 
              + operation.size());
  boost::scoped_array<unsigned char> buf (new unsigned char[size]());
  size_t pos(0);

  CORBA::Octet major_version(
    boost::is_same<C, idl::creden::CredentialData>::value ?
    idl::core::MajorVersion : idl::legacy::core::MajorVersion);

  CORBA::Octet minor_version(
    boost::is_same<C, idl::creden::CredentialData>::value ?
    idl::core::MinorVersion : idl::legacy::core::MinorVersion);

  buf.get()[pos] = major_version;
  pos += sizeof(major_version);
  buf.get()[pos] = minor_version;
  pos += sizeof(minor_version);
  std::memcpy(buf.get() + pos, secret.data(), secret_size);
  pos += secret_size;
  // ticket must be a little endian 32 bits number (by openbus protocol)
  boost::uint8_t* ticket_bytes = (boost::uint8_t*) &ticket;
  CORBA::ULong little_endian_ticket = (ticket_bytes[0] << 0 |
                                       ticket_bytes[1] << 8 |
                                       ticket_bytes[2] << 16 |
                                       ticket_bytes[3] << 24);
  std::memcpy(buf.get() + pos, &little_endian_ticket, sizeof(CORBA::ULong));
  pos += sizeof(CORBA::ULong);
  std::memcpy(buf.get() + pos, operation.c_str(), operation.size());
  hash_value hash;
  SHA256(buf.get(), size, hash.c_array());
  return hash;
}

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

struct OPENBUS_SDK_DECL login
{
  login(ORBInitializer *, idl::access::LoginInfo);
  ~login();
  ORBInitializer *orb_init;
};

struct OPENBUS_SDK_DECL busid
{
  busid(ORBInitializer *, std::string);
  ~busid();
  ORBInitializer *orb_init;
};

struct OPENBUS_SDK_DECL ORBInitializer : 
  public PortableInterceptor::ORBInitializer,
  public CORBA::LocalObject
{
  ORBInitializer(log_type *);
  ~ORBInitializer();
  void pre_init(PortableInterceptor::ORBInitInfo_ptr);
  void post_init(PortableInterceptor::ORBInitInfo_ptr);

  log_type *log;
  PortableInterceptor::ClientRequestInterceptor_var cln_interceptor; 
  PortableInterceptor::ServerRequestInterceptor_var srv_interceptor;
  IOP::Codec_var codec;
  PortableInterceptor::Current_var pi_current;
  PortableInterceptor::SlotId current_connection, joined_call_chain,
    signed_call_chain, ignore_interceptor, ignore_invalid_login, request_id,
    login, busid;
};

}}

#endif
