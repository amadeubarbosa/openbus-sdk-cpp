// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_SERVERINTERCEPTOR_IMPL_H_
#define TECGRAF_SDK_SERVERINTERCEPTOR_IMPL_H_

#include "openbus/Connection_impl.h"
extern "C" {
  #include "openbus/util/Ticket_impl.h"
}
#ifndef TECGRAF_SDK_LRUCACHE_H_
#define TECGRAF_SDK_LRUCACHE_H_
#include "openbus/util/LRUCache_impl.h"
#endif

#include <CORBA.h>
#include <string>

namespace openbus 
{
namespace interceptors 
{
struct Session 
{
  Session(CORBA::ULong, const std::string);
  CORBA::ULong id;
  tickets_History tickets;
  unsigned char secret[SECRET_SIZE];
  std::string remoteId;
};

class ServerInterceptor : public PortableInterceptor::ServerRequestInterceptor 
{
public:
  ServerInterceptor(PortableInterceptor::Current *piCurrent, 
                    PortableInterceptor::SlotId slotId_requesterConnection,
                    PortableInterceptor::SlotId slotId_receiveConnection,
                    PortableInterceptor::SlotId slotId_joinedCallChain,
                    PortableInterceptor::SlotId slotId_signedCallChain, 
                    PortableInterceptor::SlotId slotId_legacyCallChain,
                    IOP::Codec *cdr_codec);
  ~ServerInterceptor();
  void receive_request_service_contexts(PortableInterceptor::ServerRequestInfo *);
  void receive_request(PortableInterceptor::ServerRequestInfo *) 
  { 
  }

  void send_reply(PortableInterceptor::ServerRequestInfo *) 
  { 
  }

  void send_exception(PortableInterceptor::ServerRequestInfo *) 
  { 
  }

  void send_other(PortableInterceptor::ServerRequestInfo *) 
  { 
  }

  char *name() 
  { 
    return CORBA::string_dup("ServerInterceptor"); 
  }

  void destroy() 
  { 
  }

  void openbusContext(OpenBusContext *m) 
  { 
    _openbusContext = m; 
  }
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
  Connection *getDispatcher(OpenBusContext &context, const std::string busId, 
                            const std::string loginId, const std::string operation);
  void sendCredentialReset(Connection *, Login *, PortableInterceptor::ServerRequestInfo *);

  typedef LRUCache<CORBA::ULong, Session> SessionLRUCache;
  SessionLRUCache _sessionLRUCache;
};
}
}

#endif
