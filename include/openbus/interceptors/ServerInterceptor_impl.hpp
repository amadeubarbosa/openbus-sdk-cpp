// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_IMPL_H_
#define TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_IMPL_H_

#include "openbus/decl.hpp"

extern "C" 
{
  #include "openbus/Ticket_impl.h"
}
#ifndef TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#define TECGRAF_SDK_OPENBUS_LRUCACHE_H_
  #include "openbus/LRUCache_impl.hpp"
#endif
#include "openbus/LoginCache.hpp"

#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif

#include <CORBA.h>
#include <cstddef>
#include <string>

namespace openbus 
{
class OpenBusContext;
class Connection;

namespace interceptors 
{
namespace PI = PortableInterceptor;

const std::size_t secretSize = 16;

struct OPENBUS_SDK_DECL Session 
{
  Session(std::size_t, const std::string &);
  std::size_t id;
  tickets_History tickets;
  unsigned char secret[secretSize];
  std::string remoteId;
};

class OPENBUS_SDK_DECL ServerInterceptor : public PI::ServerRequestInterceptor 
{
public:
  ServerInterceptor(
    PI::Current *piCurrent, PI::SlotId slotId_requesterConnection, 
    PI::SlotId slotId_receiveConnection, PI::SlotId slotId_joinedCallChain,
    PI::SlotId slotId_signedCallChain, PI::SlotId slotId_legacyCallChain,
    IOP::Codec *cdr_codec);

  void 
  receive_request_service_contexts(PI::ServerRequestInfo *);

  void receive_request(PI::ServerRequestInfo *) 
  { 
  }

  void send_reply(PI::ServerRequestInfo *) 
  { 
  }

  void send_exception(PI::ServerRequestInfo *) 
  { 
  }

  void send_other(PI::ServerRequestInfo *) 
  { 
  }

  char *name() 
  { 
    return CORBA::string_dup("ServerInterceptor"); 
  }

  void destroy() 
  { 
  }

  void openbusContext(OpenBusContext &m) 
  { 
    _openbusContext = &m;
  }
private:
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::mutex _mutex;
#endif
  PI::Current *_piCurrent;
  PI::SlotId _slotId_requesterConnection;
  PI::SlotId _slotId_receiveConnection;
  PI::SlotId _slotId_joinedCallChain;
  PI::SlotId _slotId_signedCallChain;
  PI::SlotId _slotId_legacyCallChain;
  IOP::Codec *_cdrCodec;
  OpenBusContext *_openbusContext;
  Connection &getDispatcher(
    OpenBusContext &context, const std::string &busId, 
    const std::string &loginId, const std::string &operation);

  void sendCredentialReset(Connection &, Login &, PI::ServerRequestInfo &);

  typedef LRUCache<CORBA::ULong, Session> SessionLRUCache;
  SessionLRUCache _sessionLRUCache;
};
}
}

#endif
