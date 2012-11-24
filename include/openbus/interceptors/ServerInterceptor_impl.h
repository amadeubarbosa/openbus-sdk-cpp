#ifndef TECGRAF_SERVERINTERCEPTOR_IMPL_H_
#define TECGRAF_SERVERINTERCEPTOR_IMPL_H_

#include <CORBA.h>

#include "openbus/Connection.h"
#include "openbus/Connection_impl.h"
#include "openbus/OpenBusContext.h"
extern "C" {
  #include "openbus/util/Ticket_impl.h"
}
#ifndef TECGRAF_LRUCACHE_H_
#define TECGRAF_LRUCACHE_H_
#include "openbus/util/LRUCache_impl.h"
#endif

namespace openbus {
namespace interceptors {

  struct Session {
    Session(CORBA::ULong, const char *);
    CORBA::ULong id;
    tickets_History tickets;
    unsigned char secret[SECRET_SIZE];
    char* remoteId;
  };

  class ServerInterceptor : public PortableInterceptor::ServerRequestInterceptor {
  public:
    ServerInterceptor(
      PortableInterceptor::Current *piCurrent, 
      PortableInterceptor::SlotId slotId_requesterConnection,
      PortableInterceptor::SlotId slotId_receiveConnection,
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
    void openbusContext(OpenBusContext *m) { _openbusContext = m; }
  private:
    Mutex _mutex;
    PortableInterceptor::Current *_piCurrent;
    PortableInterceptor::SlotId _slotId_requesterConnection;
    PortableInterceptor::SlotId _slotId_receiveConnection;
    PortableInterceptor::SlotId _slotId_joinedCallChain;
    PortableInterceptor::SlotId _slotId_signedCallChain;
    PortableInterceptor::SlotId _slotId_legacyCallChain;
    IOP::Codec *_cdrCodec;
    OpenBusContext *_openbusContext;
    Connection *getDispatcher(OpenBusContext &context, const char *busId, const char *loginId, 
                               const char *operation);
    void sendCredentialReset(Connection *, Login*, PortableInterceptor::ServerRequestInfo*);

    typedef LRUCache<CORBA::ULong, Session> SessionLRUCache;
    SessionLRUCache _sessionLRUCache;
  };
}
}

#endif
