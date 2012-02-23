#include <interceptors/serverInterceptor_impl.h>
#include <iostream>

namespace openbus {
  namespace interceptors {
    ServerInterceptor::ServerInterceptor(
      PortableInterceptor::Current* piCurrent, 
      PortableInterceptor::SlotId slotId, 
      IOP::Codec* cdr_codec) 
      : piCurrent(piCurrent) , slotId(slotId), cdr_codec(cdr_codec) { }
    
    ServerInterceptor::~ServerInterceptor() { }
    
    void ServerInterceptor::receive_request(PortableInterceptor::ServerRequestInfo* ri)
      throw (CORBA::SystemException, PortableInterceptor::ForwardRequest) 
    {
    }

    void ServerInterceptor::send_reply(PortableInterceptor::ServerRequestInfo* ri)
      throw (CORBA::SystemException) 
    {
    }
  }
}
