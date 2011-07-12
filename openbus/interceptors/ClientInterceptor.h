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
#include "../../FaultToleranceManager.h"

using namespace PortableInterceptor;
using namespace tecgraf::openbus::core::v1_05;

namespace openbus {
  namespace interceptors {
    class ClientInterceptor : public ClientRequestInterceptor 
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
        void send_request(ClientRequestInfo_ptr ri) 
          throw(
            CORBA::SystemException,
            PortableInterceptor::ForwardRequest);
        void send_poll(ClientRequestInfo_ptr ri) 
          throw(CORBA::SystemException);
        void receive_reply(ClientRequestInfo_ptr ri) 
          throw(CORBA::SystemException);
        void receive_exception(ClientRequestInfo_ptr ri) 
          throw(
            CORBA::SystemException,
            PortableInterceptor::ForwardRequest);
        void receive_other(ClientRequestInfo_ptr ri) 
          throw(
            CORBA::SystemException,
            PortableInterceptor::ForwardRequest);
        char* name() 
          throw(CORBA::SystemException);
        void destroy();
    };
  }
}

#endif
