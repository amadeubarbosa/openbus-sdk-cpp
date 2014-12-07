// -*- coding: iso-8859-1-unix -*-
#ifndef TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_IMPL_H_
#define TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_IMPL_H_

#include "openbus/interceptors/ORBInitializer_impl.hpp"
#include "openbus/decl.hpp"
#include "stubs/credential.h"
#include "stubs/credential_v1_5.h"

extern "C" 
{
  #include "openbus/Ticket_impl.h"
}
#ifndef TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#define TECGRAF_SDK_OPENBUS_LRUCACHE_H_
  #include "openbus/LRUCache_impl.hpp"
#endif

#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <CORBA.h>
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
  tecgraf::openbus::core::v2_0::credential::CredentialData data;
  openbus::legacy::v1_5::Credential legacy;
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
  ::IOP::Codec_var _codec;
  Connection &get_dispatcher(
    boost::shared_ptr<OpenBusContext>, 
    const std::string &bus,
    const std::string &login,
    const std::string &operation);
  credential get_credential(const PI::ServerRequestInfo_ptr);
  void build_legacy_chain(
    PI::ServerRequestInfo &,
    std::string target,
    const openbus::legacy::v1_5::Credential &);
  void send_credential_reset(
    Connection &, 
    boost::shared_ptr<Login>, 
    PI::ServerRequestInfo &);
  LRUCache<CORBA::ULong, boost::shared_ptr<Session> > _sessionLRUCache;
};

}}

#endif
