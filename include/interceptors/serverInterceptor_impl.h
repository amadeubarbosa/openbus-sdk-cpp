#ifndef TECGRAF_SERVERINTERCEPTOR_IMPL_H_
#define TECGRAF_SERVERINTERCEPTOR_IMPL_H_

#include <CORBA.h>
#include "connection.h"
#include "manager.h"
extern "C" {
  #include <util/tickets.h>
}
#ifndef TECGRAF_LRUCACHE_H_
#define TECGRAF_LRUCACHE_H_
#include "util/lru_cache.h"
#endif

/* forward declarations */
namespace openbus {
  class Connection;
  class ConnectionManager;
  struct Login;
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
    char* name() throw (CORBA::SystemException) { return CORBA::string_dup("ServerInterceptor"); }
    void destroy() { }
    void setConnectionManager(ConnectionManager* m) { _manager = m; }
    void resetCaches();
  private:
    #ifdef OPENBUS_SDK_MULTITHREAD
    MICOMT::Mutex _mutex;
    #endif
    PortableInterceptor::Current* _piCurrent;
    PortableInterceptor::SlotId _slotId_connectionAddr;
    PortableInterceptor::SlotId _slotId_joinedCallChain;
    PortableInterceptor::SlotId _slotId_signedCallChain;
    PortableInterceptor::SlotId _slotId_legacyCallChain;
    PortableInterceptor::SlotId _slotId_busid;
    IOP::Codec* _cdrCodec;
    ConnectionManager* _manager;
    void sendCredentialReset(Connection *, Login*, PortableInterceptor::ServerRequestInfo*);

    struct Session {
      CORBA::ULong id;
      tickets_History ticketsHistory;
      unsigned char secret[SECRET_SIZE];
      Session(CORBA::ULong id) : id(id) {
        tickets_init(&ticketsHistory);
        for (short i=0;i<SECRET_SIZE;++i) secret[i] = rand() % 255;
      }
    };

    typedef LRUCache<CORBA::ULong, Session*> SessionLRUCache;
    std::auto_ptr<SessionLRUCache> _sessionLRUCache;
  };
}
}

#endif
