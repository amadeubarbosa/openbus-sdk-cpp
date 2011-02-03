/*
** interceptors/ORBInitializerImpl.cpp
*/

#ifdef OPENBUS_ORBIX
  #include <omg/IOP.hh>
#endif

#include <openbus/interceptors/orb_initializer_impl.h>
#include <openbus/openbus.h>
#include <openbus/orb_state.h>

namespace openbus { namespace interceptors {

bool orb_initializer_impl::singleInstance = false;

orb_initializer_impl::orb_initializer_impl(openbus::orb_state& orb_state)
  : orb_state(&orb_state)
{}

orb_initializer_impl::~orb_initializer_impl()
{
  //Openbus::logger->log(INFO, "orb_initializer_impl::~orb_initializer_impl() BEGIN");
  //Openbus::logger->indent();
  //Openbus::logger->dedent(INFO, "orb_initializer_impl::~orb_initializer_impl() END");
}

void orb_initializer_impl::pre_init(PortableInterceptor::ORBInitInfo_ptr info)
{
  //Openbus::logger->log(INFO, "orb_initializer_impl::pre_init() BEGIN");
  //Openbus::logger->indent();
  _info = info;
  IOP::CodecFactory_var codec_factory = _info->codec_factory();
  IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
  codec = codec_factory->create_codec(cdr_encoding);

  PortableInterceptor::ClientRequestInterceptor_var clientInterceptor = 
    new client_interceptor(codec, *orb_state);
  _info->add_client_request_interceptor(clientInterceptor);

  slotid = _info->allocate_slot_id();

  CORBA::Object_var init_ref = _info->resolve_initial_references("PICurrent");
  PortableInterceptor::Current_var pi_current = PortableInterceptor::Current::_narrow(init_ref);

  serverInterceptor = new server_interceptor(pi_current, slotid, codec, *orb_state);

  PortableInterceptor::ServerRequestInterceptor_var 
    serverRequestInterceptor = serverInterceptor;
  _info->add_server_request_interceptor(serverRequestInterceptor) ;
      
  //Openbus::logger->dedent(INFO, "orb_initializer_impl::pre_init() END");
}

void orb_initializer_impl::post_init(PortableInterceptor::ORBInitInfo_ptr info) {}

server_interceptor* orb_initializer_impl::getServerInterceptor()
{
  return serverInterceptor;
}

} }

