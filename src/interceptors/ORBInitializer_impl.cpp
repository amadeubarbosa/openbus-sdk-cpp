// -*- coding: iso-8859-1-unix-*-
#include "openbus/interceptors/ORBInitializer_impl.hpp"
#include "openbus/interceptors/ClientInterceptor_impl.hpp"
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/log.hpp"

namespace openbus 
{
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

orb_info::orb_info(PI::ORBInitInfo_ptr i)
  : info(i), slot(info)
{
  IOP::CodecFactory_var codec_factory = info->codec_factory();
  IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
  codec = codec_factory->create_codec(cdr_encoding);
  CORBA::Object_var init_ref = info->resolve_initial_references("PICurrent");
  pi_current = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(pi_current));
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
  _orb_info = boost::shared_ptr<orb_info>(new orb_info(info));
  clientInterceptor = boost::shared_ptr<ClientInterceptor>
    (new ClientInterceptor(_orb_info));
  info->add_client_request_interceptor(clientInterceptor.get());
  serverInterceptor = boost::shared_ptr<ServerInterceptor>
    (new ServerInterceptor(_orb_info));
  info->add_server_request_interceptor(serverInterceptor.get());
}

void ORBInitializer::post_init(PortableInterceptor::ORBInitInfo_ptr)
{
}

}}
