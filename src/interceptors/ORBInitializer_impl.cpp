// -*- coding: iso-8859-1-unix -*-

#include "openbus/interceptors/ORBInitializer_impl.hpp"
#include "openbus/interceptors/ClientInterceptor_impl.hpp"
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/log.hpp"

#include "openbus/crypto/OpenSSL.hpp"

namespace openbus 
{
hash_value hash(std::string operation, CORBA::ULong ticket, 
                boost::array<unsigned char, secret_size> secret)
{
  size_t size(sizeof(idl::MajorVersion) + sizeof(idl::MinorVersion) 
    + secret_size + sizeof(CORBA::ULong) /* ticket */ 
              + operation.size());
  boost::scoped_array<unsigned char> buf (new unsigned char[size]());
  size_t pos(0);
  buf.get()[pos] = idl::MajorVersion;
  pos += sizeof(idl::MajorVersion);
  buf.get()[pos] = idl::MinorVersion;
  pos += sizeof(idl::MinorVersion);
  std::memcpy(buf.get() + pos, secret.data(), secret_size);
  pos += secret_size;
  std::memcpy(buf.get() + pos, &ticket, sizeof(CORBA::ULong));
  pos += sizeof(CORBA::ULong);
  std::memcpy(buf.get() + pos, operation.c_str(), operation.size());
  hash_value hash;
  SHA256(buf.get(), size, hash.c_array());
  return hash;
}

namespace interceptors 
{
ignore_interceptor::ignore_interceptor(boost::shared_ptr<orb_info> p)
  : _orb_info(p)
{
  CORBA::Any ignoreInterceptorAny;
  ignoreInterceptorAny <<= CORBA::Any::from_boolean(true);
  _orb_info->pi_current->set_slot(_orb_info->slot.ignore_interceptor, 
                                 ignoreInterceptorAny);
}

ignore_interceptor::~ignore_interceptor()
{
  try
  {
    CORBA::Any ignoreInterceptorAny;
    ignoreInterceptorAny <<= CORBA::Any::from_boolean(false);
    _orb_info->pi_current->set_slot(_orb_info->slot.ignore_interceptor, 
                                   ignoreInterceptorAny); 
  } 
  catch (...)
  {
  }
}

ignore_invalid_login::ignore_invalid_login(boost::shared_ptr<orb_info> p)
  : _orb_info(p)
{
  CORBA::Any any;
  any <<= CORBA::Any::from_boolean(true);
  _orb_info->pi_current->set_slot(_orb_info->slot.ignore_invalid_login, any); 
}

ignore_invalid_login::~ignore_invalid_login()
{
  try
  {
    CORBA::Any any;
    any <<= CORBA::Any::from_boolean(false);
    _orb_info->pi_current->set_slot(_orb_info->slot.ignore_invalid_login, any); 
  } 
  catch (...)
  {
  }
}

orb_info::orb_info(PI::ORBInitInfo_ptr i)
  : info(i), slot(info)
{
}

ORBInitializer::ORBInitializer() 
{
  log_scope l(log().general_logger(), debug_level, 
              "openbus::interceptors::ORBInitializer::ORBInitializer");
}

ORBInitializer::~ORBInitializer() 
{
}

void ORBInitializer::pre_init(PortableInterceptor::ORBInitInfo_ptr info) 
{
}

void ORBInitializer::post_init(PortableInterceptor::ORBInitInfo_ptr info)
{
  _orb_info = boost::shared_ptr<orb_info>(new orb_info(info));

  IOP::CodecFactory_var codec_factory(info->codec_factory());
  IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
  _orb_info->codec = codec_factory->create_codec(cdr_encoding);

  CORBA::Object_var init_ref(info->resolve_initial_references("PICurrent"));
  _orb_info->pi_current = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_orb_info->pi_current));

  cln_interceptor = new ClientInterceptor(_orb_info,
                                          _orb_info->pi_current.in(),
                                          _orb_info->codec);
  info->add_client_request_interceptor(cln_interceptor);
  srv_interceptor = new ServerInterceptor(_orb_info, _orb_info->codec);
  info->add_server_request_interceptor(srv_interceptor);
}

}}
