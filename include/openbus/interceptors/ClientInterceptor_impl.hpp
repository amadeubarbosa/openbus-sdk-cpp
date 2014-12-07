// -*- coding: iso-8859-1-unix -*-
#ifndef TECGRAF_SDK_OPENBUS_CLIENT_INTERCEPTOR_IMPL_H_
#define TECGRAF_SDK_OPENBUS_CLIENT_INTERCEPTOR_IMPL_H_

#include "openbus/interceptors/ORBInitializer_impl.hpp"
#include "openbus/decl.hpp"
#ifndef TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#define TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#include "openbus/LRUCache_impl.hpp"
#endif
#include "stubs/credential.h"

#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/shared_ptr.hpp>
#include <CORBA.h>

namespace openbus 
{
namespace idl_cr = tecgraf::openbus::core::v2_0::credential;

class OpenBusContext;
class Connection;
struct CallerChain;

namespace interceptors 
{
namespace PI = PortableInterceptor;
struct OPENBUS_SDK_DECL 
ClientInterceptor : public PI::ClientRequestInterceptor 
{
  ClientInterceptor(boost::shared_ptr<orb_info>);

  void send_request(PI::ClientRequestInfo_ptr);
  void receive_exception(PI::ClientRequestInfo_ptr);
  void send_poll(PI::ClientRequestInfo_ptr);
  void receive_reply(PI::ClientRequestInfo_ptr);
  void receive_other(PI::ClientRequestInfo_ptr);
  char *name();
  void destroy();
  Connection &get_current_connection(PI::ClientRequestInfo &);
  bool ignore_request(PI::ClientRequestInfo &);
  idl_cr::SignedCallChain get_signed_chain(Connection &, hash_value &hash, 
                                           const std::string &remote_id);
  void build_credential(PI::ClientRequestInfo &, Connection &conn);
  void build_legacy_credential(PI::ClientRequestInfo &, Connection &conn);
  openbus::CallerChain get_joined_chain(Connection &, PI::ClientRequestInfo &);
  boost::shared_ptr<orb_info> _orb_info;
  ::IOP::Codec_var _codec;
  boost::shared_ptr<OpenBusContext> _openbus_ctx;
  LRUCache<hash_value, idl_cr::SignedCallChain> _callChainLRUCache;
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::mutex _mutex;
#endif
};

}}

#endif
