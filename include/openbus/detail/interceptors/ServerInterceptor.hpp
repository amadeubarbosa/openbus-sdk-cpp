// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_HPP
#define TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_HPP

#include "openbus_creden-2.1C.h"
#include "openbus/detail/interceptors/ORBInitializer.hpp"
#include "openbus/detail/decl.hpp"

extern "C" 
{
  #include "openbus/detail/Ticket.h"
}
#ifndef TECGRAF_SDK_OPENBUS_LRUCACHE_HPP
#define TECGRAF_SDK_OPENBUS_LRUCACHE_HPP
  #include "openbus/detail/LRUCache.hpp"
#endif

#include <tao/PI_Server/PI_Server.h>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <cstddef>
#include <string>

namespace openbus 
{
class OpenBusContext;
class Connection;
struct Login;

namespace interceptors 
{

namespace PI = PortableInterceptor;

struct OPENBUS_SDK_DECL Session 
{
  Session(const std::string &login);
  CORBA::ULong id;
  tickets_History tickets;
  boost::uuids::uuid secret;
  std::string remote_id;
};

struct credential
{
  tecgraf::openbus::core::v2_1::credential::CredentialData data;
};

struct OPENBUS_SDK_DECL ServerInterceptor : public PI::ServerRequestInterceptor 
{
  ServerInterceptor(ORBInitializer *);
  void receive_request_service_contexts(PI::ServerRequestInfo_ptr);
  void receive_request(PI::ServerRequestInfo_ptr);
  void send_reply(PI::ServerRequestInfo_ptr);
  void send_exception(PI::ServerRequestInfo_ptr);
  void send_other(PI::ServerRequestInfo_ptr);
  char *name();
  void destroy();

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::mutex _mutex;
#endif
  ORBInitializer *_orb_init;
  LRUCache<CORBA::ULong, boost::shared_ptr<Session> > _sessionLRUCache;
  OpenBusContext *_bus_ctx;
  void save_dispatcher_connection(
    Connection &,
    PI::ServerRequestInfo &,
    OpenBusContext *);

  Connection &get_dispatcher_connection(
    OpenBusContext *, 
    const std::string &busid,
    const std::string &login,
    PI::ServerRequestInfo &);

  credential get_credential(PI::ServerRequestInfo &) const;

  bool validate_chain(
    credential& cred,
    boost::shared_ptr<Login> caller,  
    Connection &conn);

  void send_credential_reset(
    Connection &, 
    boost::shared_ptr<Login>, 
    PI::ServerRequestInfo &);
};

}}

#endif