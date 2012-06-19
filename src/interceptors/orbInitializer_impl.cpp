#include <interceptors/orbInitializer_impl.h>
#include <openbus.h>
#include <log.h>


namespace openbus {
namespace interceptors {
  ORBInitializer::ORBInitializer() {
    log_scope l(log.general_logger(), debug_level, 
      "openbus::interceptors::ORBInitializer::ORBInitializer");
  }

  ORBInitializer::~ORBInitializer() {}
  
  void ORBInitializer::pre_init(PortableInterceptor::ORBInitInfo* info) {
    IOP::CodecFactory_var codec_factory = info->codec_factory();
    IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
    _codec = codec_factory->create_codec(cdr_encoding);
    _slotId_connectionAddr = info->allocate_slot_id();
    _slotId_joinedCallChain = info->allocate_slot_id();
    _slotId_ignoreInterceptor = info->allocate_slot_id();
    CORBA::Object_var init_ref = info->resolve_initial_references("PICurrent");
    PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
    assert(!CORBA::is_nil(piCurrent));
    _clientInterceptor = std::auto_ptr<ClientInterceptor> 
      (new ClientInterceptor(_slotId_connectionAddr, _slotId_joinedCallChain, 
      _slotId_ignoreInterceptor, _codec.in()));
    info->add_client_request_interceptor(_clientInterceptor.get());
    _slotId_signedCallChain = info->allocate_slot_id();
    _slotId_busid = info->allocate_slot_id();
    _slotId_legacyCallChain = info->allocate_slot_id();
    _serverInterceptor = std::auto_ptr<ServerInterceptor> (
      new ServerInterceptor(
        piCurrent.in(),
        _slotId_connectionAddr,
        _slotId_joinedCallChain,
        _slotId_signedCallChain,
        _slotId_legacyCallChain,
        _slotId_busid,
        _codec.in()));
    info->add_server_request_interceptor(_serverInterceptor.get());
  }
}
}
