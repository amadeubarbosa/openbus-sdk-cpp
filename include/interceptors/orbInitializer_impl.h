#ifndef TECGRAF_ORBINITIALIZER_IMPL_H_
#define TECGRAF_ORBINITIALIZER_IMPL_H_

#include <memory>
#include <CORBA.h>
#include "clientInterceptor_impl.h"
#include "serverInterceptor_impl.h"

namespace openbus {
  namespace interceptors {
    class ClientInterceptor;
    class ServerInterceptor;
    
    class ORBInitializer : public PortableInterceptor::ORBInitializer {
      public:
        ORBInitializer();
        ~ORBInitializer();
        void pre_init(PortableInterceptor::ORBInitInfo* info);
        void post_init(PortableInterceptor::ORBInitInfo* info) { }
        ClientInterceptor* getClientInterceptor() const { return clientInterceptor.get(); }
        ServerInterceptor* getServerInterceptor() const { return serverInterceptor.get(); }
        IOP::Codec* codec() const { return _codec; }
        PortableInterceptor::SlotId slotId_joinedCallChain() const 
          { return _slotId_joinedCallChain; }
        PortableInterceptor::SlotId slotId_callChain() const 
          { return _slotId_callChain; }
        PortableInterceptor::SlotId slotId_busId() const 
          { return _slotId_busId; }
      private:
        std::auto_ptr<ClientInterceptor> clientInterceptor; 
        std::auto_ptr<ServerInterceptor> serverInterceptor;
        IOP::Codec_var _codec;
        PortableInterceptor::SlotId _slotId_joinedCallChain;
        PortableInterceptor::SlotId _slotId_callChain;
        PortableInterceptor::SlotId _slotId_busId;
    };
  }
}

#endif
