// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_INTERCEPTORS_CLIENT_INTERCEPTOR_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_INTERCEPTORS_CLIENT_INTERCEPTOR_HPP

#include "openbus/idl.hpp"
#include "openbus/detail/interceptors/orb_initializer.hpp"
#include "openbus/detail/decl.hpp"
#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_LRUCACHE_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_LRUCACHE_HPP
#include "openbus/detail/lru_cache.hpp"
#endif
#include "openbus/connection.hpp"

#include <tao/PI/PI.h>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/mpl/if.hpp>
#include <map>

namespace openbus 
{
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
  boost::shared_ptr<Connection> get_current_connection(PI::ClientRequestInfo &);
  bool ignore_request(PI::ClientRequestInfo &);
  bool ignore_invalid_login(PI::ClientRequestInfo &);

  idl::creden::SignedData
    get_signed_chain(
      const boost::shared_ptr<Connection> &,
      hash_value &hash, 
      const std::string &target,
      idl::creden::CredentialData);
    
  idl::legacy::creden::SignedCallChain
    get_signed_chain(
      const boost::shared_ptr<Connection> &,
      hash_value &, 
      const std::string &target,
      idl::legacy::creden::CredentialData);

  openbus::CallerChain get_joined_chain(
    PI::ClientRequestInfo_ptr,
    const boost::shared_ptr<Connection> &);
  
  boost::uuids::uuid get_request_id(PI::ClientRequestInfo_ptr);
  
  Connection::SecretSession * get_session(
    PI::ClientRequestInfo_ptr,
    const boost::shared_ptr<Connection> &);

  template <typename C>
    void fill_credential(
      PI::ClientRequestInfo_ptr,
      const boost::shared_ptr<Connection> &,
      typename boost::mpl::if_<
      typename boost::is_same<C, idl::creden::CredentialData>::type,
      idl::access::LoginInfo,
      idl::legacy::access::LoginInfo>::type &,
      CallerChain &,
      Connection::SecretSession *);

  void attach_credential(
    PI::ClientRequestInfo_ptr,
    const boost::shared_ptr<Connection> &,
    idl::access::LoginInfo &);
  
  boost::shared_ptr<log_type> _log;
  ORBInitializer *_orb_init;
  LRUCache<hash_value, idl::creden::SignedData> _callChainLRUCache;
  LRUCache<hash_value, idl::legacy::creden::SignedCallChain>
    _legacy_callChainLRUCache;
  OpenBusContext *_bus_ctx;
  std::map<boost::uuids::uuid, boost::shared_ptr<Connection> > _request_id2conn;
  boost::mutex _mutex;
};

}}

#endif
