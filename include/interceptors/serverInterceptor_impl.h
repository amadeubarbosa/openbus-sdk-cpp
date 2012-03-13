#ifndef TECGRAF_SERVERINTERCEPTOR_IMPL_H_
#define TECGRAF_SERVERINTERCEPTOR_IMPL_H_

#include <CORBA.h>
#include <connection.h>

namespace openbus {
  class Connection;
  namespace interceptors {
    class ServerInterceptor : public PortableInterceptor::ServerRequestInterceptor {
      public:
        ServerInterceptor(
          PortableInterceptor::Current* piCurrent, 
          PortableInterceptor::SlotId slotId, 
          IOP::Codec* cdr_codec);
        ~ServerInterceptor();
        void receive_request_service_contexts(PortableInterceptor::ServerRequestInfo*) 
          throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);
        void receive_request(PortableInterceptor::ServerRequestInfo* ri)
          throw (CORBA::SystemException, PortableInterceptor::ForwardRequest) { }
        void send_reply(PortableInterceptor::ServerRequestInfo*)
          throw (CORBA::SystemException) { }
        void send_exception(PortableInterceptor::ServerRequestInfo*)
          throw (CORBA::SystemException, PortableInterceptor::ForwardRequest) { };
        void send_other(PortableInterceptor::ServerRequestInfo*)
          throw (CORBA::SystemException, PortableInterceptor::ForwardRequest) { };
        char* name() throw (CORBA::SystemException) 
          { return CORBA::string_dup("ServerInterceptor"); }
        void destroy() { }
        void addConnection(Connection* connection);
        void removeConnection(Connection* connection);
      private:
        PortableInterceptor::Current* piCurrent;
        PortableInterceptor::SlotId slotId;
        IOP::Codec* cdr_codec;
        Connection* connection;
        struct Session {
          //[todo] tickets
          CORBA::ULong id;
          unsigned char secret[16];
          Session() {
            for (short i=0;i<16;++i)
              secret[i] = rand() % 255;
            id = 0;
          }
        };
        std::map<std::string, Session> loginSession;
    };
  }
}

#endif
