#include <interceptors/orbInitializer_impl.h>

#include <openbus.h>

namespace openbus {
  namespace interceptors {
    ORBInitializer::ORBInitializer() {}

    ORBInitializer::~ORBInitializer() {}
    
    void ORBInitializer::pre_init(PortableInterceptor::ORBInitInfo* info) {
      IOP::CodecFactory_var codec_factory = info->codec_factory();
      IOP::Encoding cdr_encoding = {
        IOP::ENCODING_CDR_ENCAPS, 
        1, 
        2};
      _codec = codec_factory->create_codec(cdr_encoding);
      clientInterceptor = std::auto_ptr<ClientInterceptor> (new ClientInterceptor(_codec.in()));
      info->add_client_request_interceptor(clientInterceptor.get());
      _slotId_callChain = info->allocate_slot_id();
      _slotId_busId = info->allocate_slot_id();
      CORBA::Object_var init_ref = info->resolve_initial_references("PICurrent");
      assert(!CORBA::is_nil(init_ref));
      PortableInterceptor::Current_var piCurrent = PortableInterceptor::Current::_narrow(init_ref);
      serverInterceptor = std::auto_ptr<ServerInterceptor> (
        new ServerInterceptor(
          piCurrent.in(), 
          _slotId_callChain,
          _slotId_busId,
          _codec.in()));
      info->add_server_request_interceptor(serverInterceptor.get());
    }
  }
}
