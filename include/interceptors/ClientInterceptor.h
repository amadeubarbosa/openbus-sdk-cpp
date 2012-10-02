/*
** interceptors/ClientInterceptor.h
*/

#ifndef CLIENTINTERCEPTOR_H_
#define CLIENTINTERCEPTOR_H_

#include <map>
#include <string.h>
#ifdef OPENBUS_ORBIX
  #include <orbix/corba.hh>
  #include <omg/PortableInterceptor.hh>
  #include "stubs/orbix/access_control_service.hh"
#else
  #include <CORBA.h>
  #include "stubs/mico/access_control_service.h"
#endif
#include "FaultToleranceManager.h"

namespace openbus {
  namespace interceptors {
    class ClientInterceptor : public PortableInterceptor::ClientRequestInterceptor 
    #ifdef OPENBUS_ORBIX
                              ,public IT_CORBA::RefCountedLocalObject 
    #endif
    {
      private:
        IOP::Codec_ptr cdr_codec;
        FaultToleranceManager* faultToleranceManager;
        void loadOperationObjectKey();
      public:
        ClientInterceptor(IOP::Codec_ptr pcdr_codec);
        ~ClientInterceptor();
        void send_request(PortableInterceptor::ClientRequestInfo_ptr ri);
        void send_poll(PortableInterceptor::ClientRequestInfo_ptr ri);
        void receive_reply(PortableInterceptor::ClientRequestInfo_ptr ri);
        void receive_exception(PortableInterceptor::ClientRequestInfo_ptr ri);
        void receive_other(PortableInterceptor::ClientRequestInfo_ptr ri);
        char* name();
        void destroy();
    };
  }
}

#endif
