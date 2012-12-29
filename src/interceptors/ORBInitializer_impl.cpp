// -*- coding: iso-8859-1 -*-
#include "openbus/Connection.hpp"
#include "openbus/interceptors/ORBInitializer_impl.hpp"
#include "openbus/interceptors/ClientInterceptor_impl.hpp"
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/log.hpp"

namespace openbus 
{
namespace interceptors 
{
ORBInitializer::ORBInitializer() 
{
  log_scope l(log.general_logger(), debug_level, 
              "openbus::interceptors::ORBInitializer::ORBInitializer");
}

ORBInitializer::~ORBInitializer() 
{ 
}

void ORBInitializer::pre_init(PortableInterceptor::ORBInitInfo *info) 
{
  IOP::CodecFactory_var codec_factory = info->codec_factory();
  IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
  _codec = codec_factory->create_codec(cdr_encoding);
  _slotId_requesterConnection = info->allocate_slot_id();
  _slotId_receiveConnection = info->allocate_slot_id();
  _slotId_joinedCallChain = info->allocate_slot_id();
  _slotId_ignoreInterceptor = info->allocate_slot_id();
  CORBA::Object_var init_ref = info->resolve_initial_references("PICurrent");
  PortableInterceptor::Current_var piCurrent = 
    PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(piCurrent.in()));
  _clientInterceptor = std::auto_ptr<ClientInterceptor> 
    (new ClientInterceptor(_slotId_requesterConnection, 
                           _slotId_joinedCallChain, 
                           _slotId_ignoreInterceptor, _codec.in()));
  info->add_client_request_interceptor(_clientInterceptor.get());
  _slotId_signedCallChain = info->allocate_slot_id();
  _slotId_legacyCallChain = info->allocate_slot_id();
  _serverInterceptor = std::auto_ptr<ServerInterceptor> 
    (new ServerInterceptor(piCurrent.in(), _slotId_requesterConnection, 
                           _slotId_receiveConnection, 
                           _slotId_joinedCallChain, _slotId_signedCallChain, 
                           _slotId_legacyCallChain, _codec.in()));
  info->add_server_request_interceptor(_serverInterceptor.get());
}
}
}
