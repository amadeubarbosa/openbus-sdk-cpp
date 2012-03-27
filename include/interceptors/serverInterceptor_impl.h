#ifndef TECGRAF_SERVERINTERCEPTOR_IMPL_H_
#define TECGRAF_SERVERINTERCEPTOR_IMPL_H_

#include <CORBA.h>
#include <connection.h>
#include <multiplexer.h>
extern "C" {
  #include <util/tickets.h>
}

namespace openbus {
  class Connection;
  namespace multiplexed {
    class ConnectionMultiplexer;
  }
}

namespace openbus {
  namespace interceptors {
    class ServerInterceptor : public PortableInterceptor::ServerRequestInterceptor {
    public:
      ServerInterceptor(
        PortableInterceptor::Current* piCurrent, 
        PortableInterceptor::SlotId slotId_joinedCallChain,
        PortableInterceptor::SlotId slotId_signedCallChain, 
        PortableInterceptor::SlotId slotId_busid, 
        IOP::Codec* cdr_codec);
      ~ServerInterceptor();
      void receive_request_service_contexts(PortableInterceptor::ServerRequestInfo*) 
      throw (CORBA::SystemException, PortableInterceptor::ForwardRequest);
      void receive_request(PortableInterceptor::ServerRequestInfo*)
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
      void setConnection(Connection* c) { _conn = c; }
      void setConnectionMultiplexer(multiplexed::ConnectionMultiplexer* m) { _multiplexer = m; }
    private:
      PortableInterceptor::Current* _piCurrent;
      PortableInterceptor::SlotId _slotId_joinedCallChain;
      PortableInterceptor::SlotId _slotId_signedCallChain;
      PortableInterceptor::SlotId _slotId_busid;
      IOP::Codec* _cdrCodec;
      Connection* _conn;
      multiplexed::ConnectionMultiplexer* _multiplexer;
      struct CredentialSession {
        CORBA::ULong id;
        tickets_History ticketsHistory;
        unsigned char secret[16];
        CredentialSession(CORBA::ULong id) : id(id) {
          tickets_init(&ticketsHistory);
          for (short i=0;i<16;++i)
            secret[i] = rand() % 255;
        }
      };
      std::map<CORBA::ULong, CredentialSession*> _sessionIdCredentialSession;
    };
  }
}

#endif
