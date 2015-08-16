// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_HPP
#define TECGRAF_SDK_OPENBUS_SERVER_INTERCEPTOR_HPP

#include "openbus_creden-2.1C.h"
#include "openbus_access-2.1C.h"
#include "access_controlC.h"
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
namespace legacy_idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
namespace idl_ac = tecgraf::openbus::core::v2_1::services::access_control;
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
    PI::ServerRequestInfo_ptr,
    OpenBusContext *);

  Connection &get_dispatcher_connection(
    OpenBusContext *, 
    const std::string &busid,
    const std::string &login,
    PI::ServerRequestInfo_ptr);

  
  template <typename C = idl_cr::CredentialData>
  C get_credential(PI::ServerRequestInfo_ptr) const;

  bool check_legacy_chain(
    legacy_idl_ac::CallChain,
    boost::shared_ptr<Login> caller,
    Connection &,
    PI::ServerRequestInfo_ptr);

  bool check_chain(
    idl_ac::CallChain,
    boost::shared_ptr<Login> caller,  
    Connection &);

  template <typename C = idl_cr::CredentialData>
    bool validate_chain(
      C &,
      boost::shared_ptr<Login> caller,  
      Connection &,
      PI::ServerRequestInfo_ptr r);
  
  CORBA::Any attach_legacy_credential_rst(
    boost::shared_ptr<Session>,
    Connection &,
    idl::OctetSeq &secret);
  
  CORBA::Any attach_credential_rst(
    boost::shared_ptr<Session>,
    Connection &,
    idl::OctetSeq &secret);
    
  template <typename C = idl_cr::CredentialData>
    void send_credential_reset(
      Connection &, 
      boost::shared_ptr<Login>, 
      PI::ServerRequestInfo_ptr);

  template <typename C = idl_cr::CredentialData>
    void handle_credential(
      C &,
      PI::ServerRequestInfo_ptr);
};

}}

#endif
