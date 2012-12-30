// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_IMPL_H_
#define TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_IMPL_H_

extern "C" {
  #include "openbus/Ticket_impl.h"
}
#ifndef TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#define TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#include "openbus/LRUCache_impl.hpp"
#endif
#include "openbus/LoginCache.hpp"

#include <CORBA.h>
#include <string>

namespace openbus 
{
class OpenBusContext;
class Connection;

namespace interceptors 
{

const unsigned int secretSize = 16;

struct Session 
{
  Session(CORBA::ULong, const std::string);
  CORBA::ULong id;
  tickets_History tickets;
  unsigned char secret[secretSize];
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
  void 
  receive_request_service_contexts(PortableInterceptor::ServerRequestInfo *);
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
  Connection &getDispatcher(OpenBusContext &context, 
                            const std::string &busId,
                            const std::string &loginId, 
                            const std::string &operation);

  void sendCredentialReset(Connection &, Login &, 
                           PortableInterceptor::ServerRequestInfo *);

  typedef LRUCache<CORBA::ULong, Session> SessionLRUCache;
  SessionLRUCache _sessionLRUCache;
};
}
}

#endif
