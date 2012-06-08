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
    void pre_init(PortableInterceptor::ORBInitInfo*);
    void post_init(PortableInterceptor::ORBInitInfo*) { }
    ClientInterceptor* clientInterceptor() const { return _clientInterceptor.get(); }
    ServerInterceptor* serverInterceptor() const { return _serverInterceptor.get(); }
    IOP::Codec* codec() const { return _codec; }
    PortableInterceptor::SlotId slotId_connectionAddr() const { return _slotId_connectionAddr; }      
    PortableInterceptor::SlotId slotId_joinedCallChain() const { return _slotId_joinedCallChain; }
    PortableInterceptor::SlotId slotId_signedCallChain() const { return _slotId_signedCallChain; }
    PortableInterceptor::SlotId slotId_legacyCallChain() const { return _slotId_legacyCallChain; }
    PortableInterceptor::SlotId slotId_busid() const { return _slotId_busid; }
  private:
    std::auto_ptr<ClientInterceptor> _clientInterceptor; 
    std::auto_ptr<ServerInterceptor> _serverInterceptor;
    IOP::Codec_var _codec;
    PortableInterceptor::SlotId _slotId_connectionAddr;
    PortableInterceptor::SlotId _slotId_joinedCallChain;
    PortableInterceptor::SlotId _slotId_signedCallChain;
    PortableInterceptor::SlotId _slotId_legacyCallChain;
    PortableInterceptor::SlotId _slotId_busid;
    PortableInterceptor::SlotId _slotId_ignoreInterceptor;
  };
}
}

#endif
