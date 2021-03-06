// -*- coding: iso-8859-1-unix -*-

#include "openbus/log.hpp"
#include "openbus/detail/interceptors/orb_initializer.hpp"
#include "openbus/detail/interceptors/client.hpp"
#include "openbus/detail/interceptors/server.hpp"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include "openbus/log.hpp"
#pragma clang diagnostic pop

namespace openbus { namespace interceptors 
{
ignore_interceptor::ignore_interceptor(ORBInitializer * p)
  : orb_init(p)
{
  CORBA::Any ignoreInterceptorAny;
  ignoreInterceptorAny <<= CORBA::Any::from_boolean(true);
  orb_init->pi_current->set_slot(orb_init->ignore_interceptor, 
                                 ignoreInterceptorAny);
}

ignore_interceptor::~ignore_interceptor()
{
  try
  {
    CORBA::Any ignoreInterceptorAny;
    ignoreInterceptorAny <<= CORBA::Any::from_boolean(false);
    orb_init->pi_current->set_slot(orb_init->ignore_interceptor, 
                                   ignoreInterceptorAny); 
  } 
  catch (...)
  {
  }
}

ignore_invalid_login::ignore_invalid_login(ORBInitializer * p)
  : orb_init(p)
{
  CORBA::Any any;
  any <<= CORBA::Any::from_boolean(true);
  orb_init->pi_current->set_slot(orb_init->ignore_invalid_login, any); 
}

ignore_invalid_login::~ignore_invalid_login()
{
  try
  {
    CORBA::Any any;
    any <<= CORBA::Any::from_boolean(false);
    orb_init->pi_current->set_slot(orb_init->ignore_invalid_login, any); 
  } 
  catch (...)
  {
  }
}

login::login(ORBInitializer * p,
                     idl::access::LoginInfo login)
  : orb_init(p)
{
  CORBA::Any any;
  any <<= login;
  orb_init->pi_current->set_slot(orb_init->login, any); 
}

login::~login()
{
  try
  {
    CORBA::Any any;
    any <<= idl::access::LoginInfo();
    orb_init->pi_current->set_slot(orb_init->login, any); 
  } 
  catch (...)
  {
  }
}

busid::busid(ORBInitializer * p,
             std::string id)
  : orb_init(p)
{
  CORBA::Any any;
  any <<= id.c_str();
  orb_init->pi_current->set_slot(orb_init->busid, any); 
}

busid::~busid()
{
  try
  {
    CORBA::Any any;
    any <<= "";
    orb_init->pi_current->set_slot(orb_init->busid, any); 
  } 
  catch (...)
  {
  }
}

ORBInitializer::ORBInitializer(log_type *log)
  : log(log)
{
  log_scope l(log->general_logger(), debug_level, 
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
  IOP::CodecFactory_var codec_factory(info->codec_factory());
  IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
  codec = codec_factory->create_codec(cdr_encoding);

  CORBA::Object_var init_ref(info->resolve_initial_references("PICurrent"));
  pi_current = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(pi_current));

  cln_interceptor = new ClientInterceptor(this);
  info->add_client_request_interceptor(cln_interceptor);
  srv_interceptor = new ServerInterceptor(this);
  info->add_server_request_interceptor(srv_interceptor);

  current_connection = info->allocate_slot_id();
  joined_call_chain = info->allocate_slot_id();
  signed_call_chain = info->allocate_slot_id();
  ignore_interceptor = info->allocate_slot_id();
  ignore_invalid_login = info->allocate_slot_id();
  request_id = info->allocate_slot_id();
  login = info->allocate_slot_id();
  busid = info->allocate_slot_id();
}

}}
