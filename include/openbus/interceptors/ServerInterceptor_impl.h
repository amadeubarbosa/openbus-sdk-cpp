#ifndef TECGRAF_SERVERINTERCEPTOR_IMPL_H_
#define TECGRAF_SERVERINTERCEPTOR_IMPL_H_

#include <CORBA.h>

#include "openbus/Connection.h"
#include "openbus/Connection_impl.h"
#include "openbus/ConnectionManager.h"
extern "C" {
  #include "openbus/util/Ticket_impl.h"
}
#ifndef TECGRAF_LRUCACHE_H_
#define TECGRAF_LRUCACHE_H_
#include "openbus/util/LRUCache_impl.h"
#endif

#define SECRET_SIZE 16

namespace openbus {
namespace interceptors {
  class ServerInterceptor : public PortableInterceptor::ServerRequestInterceptor {
  public:
    ServerInterceptor(
      PortableInterceptor::Current *piCurrent, 
      PortableInterceptor::SlotId slotId_requesterConnection,
      PortableInterceptor::SlotId slotId_joinedCallChain,
      PortableInterceptor::SlotId slotId_signedCallChain, 
      PortableInterceptor::SlotId slotId_legacyCallChain,
      IOP::Codec *cdr_codec);
    ~ServerInterceptor();
    void receive_request_service_contexts(PortableInterceptor::ServerRequestInfo*);
    void receive_request(PortableInterceptor::ServerRequestInfo*) { }
    void send_reply(PortableInterceptor::ServerRequestInfo*) { }
    void send_exception(PortableInterceptor::ServerRequestInfo*) { }
    void send_other(PortableInterceptor::ServerRequestInfo*) { }
    char *name() { return CORBA::string_dup("ServerInterceptor"); }
    void destroy() { }
    void connectionManager(ConnectionManager *m) { _manager = m; }
  private:
    MICOMT::Mutex _mutex;
    PortableInterceptor::Current *_piCurrent;
    PortableInterceptor::SlotId _slotId_requesterConnection;
    PortableInterceptor::SlotId _slotId_joinedCallChain;
    PortableInterceptor::SlotId _slotId_signedCallChain;
    PortableInterceptor::SlotId _slotId_legacyCallChain;
    IOP::Codec *_cdrCodec;
    ConnectionManager *_manager;
    void sendCredentialReset(Connection *, Login*, PortableInterceptor::ServerRequestInfo*);

    struct Session {
      Session() {
        tickets_init(&ticketsHistory);
        for (short i = 0; i < SECRET_SIZE; ++i) secret[i] = rand() % 255;
      }
      CORBA::ULong id;
      tickets_History ticketsHistory;
      unsigned char secret[SECRET_SIZE];
    };

    typedef LRUCache<CORBA::ULong, Session> SessionLRUCache;
    SessionLRUCache _sessionLRUCache;
  };
}
}

#endif
