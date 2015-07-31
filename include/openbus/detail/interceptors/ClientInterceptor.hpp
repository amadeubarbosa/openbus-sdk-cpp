// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_CLIENT_INTERCEPTOR_HPP
#define TECGRAF_SDK_OPENBUS_CLIENT_INTERCEPTOR_HPP

#include "openbus_creden-2.1C.h"
#include "openbus_access-2.1C.h"
#include "openbus/detail/interceptors/ORBInitializer.hpp"
#include "openbus/detail/decl.hpp"
#ifndef TECGRAF_SDK_OPENBUS_LRUCACHE_HPP
#define TECGRAF_SDK_OPENBUS_LRUCACHE_HPP
#include "openbus/detail/LRUCache.hpp"
#endif

#include <tao/PI/PI.h>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <map>

namespace openbus 
{
namespace idl_cr = tecgraf::openbus::core::v2_1::credential;

class OpenBusContext;
class Connection;
struct CallerChain;

namespace interceptors 
{
namespace PI = PortableInterceptor;
struct OPENBUS_SDK_DECL 
ClientInterceptor : public PI::ClientRequestInterceptor 
{
  ClientInterceptor(ORBInitializer *);

  void send_request(PI::ClientRequestInfo_ptr);
  void receive_exception(PI::ClientRequestInfo_ptr);
  void send_poll(PI::ClientRequestInfo_ptr);
  void receive_reply(PI::ClientRequestInfo_ptr);
  void receive_other(PI::ClientRequestInfo_ptr);
  char *name();
  void destroy();
  Connection &get_current_connection(PI::ClientRequestInfo &);
  bool ignore_request(PI::ClientRequestInfo &);
  bool ignore_invalid_login(PI::ClientRequestInfo &);
  
  idl_cr::SignedData get_signed_chain(Connection &, hash_value &hash, 
                                           const std::string &remote_id);
  void build_credential(PI::ClientRequestInfo &, Connection &conn,
                        const tecgraf::openbus::core::v2_1::services::access_control::LoginInfo &);
  openbus::CallerChain get_joined_chain(Connection &, PI::ClientRequestInfo &);
  boost::uuids::uuid get_request_id(PI::ClientRequestInfo_ptr);
  ORBInitializer *_orb_init;
  LRUCache<hash_value, idl_cr::SignedData> _callChainLRUCache;
  OpenBusContext *_bus_ctx;
  std::map<boost::uuids::uuid, Connection *> _request_id2conn;
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::mutex _mutex;
#endif
};

}}

#endif