#ifndef TECGRAF_SERVERINTERCEPTOR_IMPL_H_
#define TECGRAF_SERVERINTERCEPTOR_IMPL_H_

#include <CORBA.h>
#include <connection.h>
#include <manager.h>
extern "C" {
  #include <util/tickets.h>
}

/* forward declarations */
namespace openbus {
  class Connection;
  class ConnectionManager;
}

#define SECRET_SIZE 16

namespace openbus {
  namespace interceptors {
    class ServerInterceptor : public PortableInterceptor::ServerRequestInterceptor {
    public:
      ServerInterceptor(
        PortableInterceptor::Current* piCurrent, 
        PortableInterceptor::SlotId slotId_connectionAddr,
        PortableInterceptor::SlotId slotId_joinedCallChain,
        PortableInterceptor::SlotId slotId_signedCallChain, 
        PortableInterceptor::SlotId slotId_legacyCallChain,
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
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest) { }
      void send_other(PortableInterceptor::ServerRequestInfo*)
        throw (CORBA::SystemException, PortableInterceptor::ForwardRequest) { }
      char* name() throw (CORBA::SystemException)
        { return CORBA::string_dup("ServerInterceptor"); }
      void destroy() { }
      void setConnectionManager(ConnectionManager* m) { _manager = m; }
      void resetCaches() { _idSecretSession.clear(); }
    private:
      PortableInterceptor::Current* _piCurrent;
      PortableInterceptor::SlotId _slotId_connectionAddr;
      PortableInterceptor::SlotId _slotId_joinedCallChain;
      PortableInterceptor::SlotId _slotId_signedCallChain;
      PortableInterceptor::SlotId _slotId_legacyCallChain;
      PortableInterceptor::SlotId _slotId_busid;
      IOP::Codec* _cdrCodec;
      ConnectionManager* _manager;
      struct SecretSession {
        CORBA::ULong id;
        tickets_History ticketsHistory;
        unsigned char secret[SECRET_SIZE];
        SecretSession(CORBA::ULong id) : id(id) {
          tickets_init(&ticketsHistory);
          for (short i=0;i<SECRET_SIZE;++i)
            secret[i] = rand() % 255;
        }
      };
      std::map<CORBA::ULong, SecretSession*> _idSecretSession;
    };
  }
}

#endif
