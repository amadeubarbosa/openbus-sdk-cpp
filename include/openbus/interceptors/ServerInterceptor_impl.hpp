// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_IMPL_H_
#define TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_IMPL_H_

#include "openbus/interceptors/ORBInitializer_impl.hpp"
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
#include <boost/shared_ptr.hpp>
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

struct OPENBUS_SDK_DECL ServerInterceptor : public PI::ServerRequestInterceptor 
{
  ServerInterceptor(boost::shared_ptr<orb_info>);
  void receive_request_service_contexts(PI::ServerRequestInfo_ptr);
  void receive_request(PI::ServerRequestInfo_ptr);
  void send_reply(PI::ServerRequestInfo_ptr);
  void send_exception(PI::ServerRequestInfo_ptr);
  void send_other(PI::ServerRequestInfo_ptr);
  char *name();
  void destroy();
  boost::shared_ptr<OpenBusContext> _openbus_ctx;
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::mutex _mutex;
#endif
  boost::shared_ptr<orb_info> _orb_info;
  Connection &getDispatcher(boost::shared_ptr<OpenBusContext> context, 
                            const std::string &busId,const std::string &loginId,
                            const std::string &operation);
  void sendCredentialReset(Connection &, Login &, PI::ServerRequestInfo &);
  typedef LRUCache<CORBA::ULong, Session> SessionLRUCache;
  SessionLRUCache _sessionLRUCache;
};

}}

#endif
