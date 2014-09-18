// -*- coding: iso-8859-1-unix -*-
#ifndef TECGRAF_SDK_OPENBUS_CLIENT_INTERCEPTOR_IMPL_H_
#define TECGRAF_SDK_OPENBUS_CLIENT_INTERCEPTOR_IMPL_H_

#include "openbus/interceptors/ORBInitializer_impl.hpp"
#include "openbus/decl.hpp"
#ifndef TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#define TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#include "openbus/LRUCache_impl.hpp"
#endif
#include "credentialC.h"

#include <tao/PI/PI.h>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/shared_ptr.hpp>

namespace openbus 
{
namespace idl_cr = tecgraf::openbus::core::v2_0::credential;

class OpenBusContext;
class Connection;
struct CallerChain;

namespace interceptors 
{
struct OPENBUS_SDK_DECL 
ClientInterceptor : public PortableInterceptor::ClientRequestInterceptor 
{
  ClientInterceptor(boost::shared_ptr<orb_info>);

  void send_request(PortableInterceptor::ClientRequestInfo_ptr);
  void receive_exception(PortableInterceptor::ClientRequestInfo_ptr);
  void send_poll(PortableInterceptor::ClientRequestInfo_ptr);
  void receive_reply(PortableInterceptor::ClientRequestInfo_ptr);
  void receive_other(PortableInterceptor::ClientRequestInfo_ptr);
  char *name();
  void destroy();
  Connection &get_current_connection(PortableInterceptor::ClientRequestInfo &);
  bool ignore_request(PortableInterceptor::ClientRequestInfo &);
  idl_cr::SignedCallChain get_signed_chain(Connection &, hash_value &hash, 
                                           const std::string &remote_id);
  void build_credential(PortableInterceptor::ClientRequestInfo &,
                        Connection &conn);
  void build_legacy_credential(PortableInterceptor::ClientRequestInfo &,
                               Connection &conn);
  openbus::CallerChain get_joined_chain(
    Connection &, PortableInterceptor::ClientRequestInfo &);
  boost::shared_ptr<orb_info> _orb_info;
  boost::shared_ptr<OpenBusContext> _openbus_ctx;
  LRUCache<hash_value, idl_cr::SignedCallChain> _callChainLRUCache;
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::mutex _mutex;
#endif
};

}}

#endif
