// -*- coding: iso-8859-1 -*-
#ifndef TECGRAF_SDK_OPENBUS_CLIENT_INTERCEPTOR_IMPL_H_
#define TECGRAF_SDK_OPENBUS_CLIENT_INTERCEPTOR_IMPL_H_

#include "openbus/decl.hpp"
#ifndef TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#define TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#include "openbus/LRUCache_impl.hpp"
#endif
#include "stubs/credential.h"

#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif

#include <CORBA.h>
#include <string>

namespace openbus 
{
namespace idl_cr = tecgraf::openbus::core::v2_0::credential;
class OpenBusContext;
class Connection;
struct CallerChain;

namespace interceptors 
{
namespace PI = PortableInterceptor;

class OPENBUS_SDK_DECL 
ClientInterceptor : public PI::ClientRequestInterceptor 
{
public:
  ClientInterceptor(
    PI::SlotId slotId_requesterConnection, PI::SlotId slotId_joinedCallChain,
    PI::SlotId slotId_ignoreInterceptor, IOP::Codec *cdr_codec);

  void send_request(PI::ClientRequestInfo *);
  void receive_exception(PI::ClientRequestInfo *);
  void send_poll(PI::ClientRequestInfo *) 
  { 
  }

  void receive_reply(PI::ClientRequestInfo *) 
  { 
  }

  void receive_other(PI::ClientRequestInfo *) 
  { 
  }

  char *name() 
  { 
    return CORBA::string_dup("ClientInterceptor"); 
  }

  void destroy() 
  { 
  }

  void openbusContext(openbus::OpenBusContext &m) 
  { 
    _openbusContext = &m; 
  }

  Connection &getCurrentConnection(PI::ClientRequestInfo &);
  openbus::CallerChain getJoinedChain(Connection &, PI::ClientRequestInfo &);
  static PI::SlotId _slotId_ignoreInterceptor;
private:

  /* Variáveis que são modificadas somente no construtor. */
  IOP::Codec *_cdrCodec;
  PI::SlotId _slotId_requesterConnection;
  PI::SlotId _slotId_joinedCallChain;
  /**/
    
  struct SecretSession 
  {
    CORBA::ULong id;
    CORBA::String_var remoteId;
    CORBA::OctetSeq_var secret;
    CORBA::ULong ticket;
  };

  typedef LRUCache<std::string, SecretSession> SessionLRUCache;
  typedef LRUCache<std::string, const idl_cr::SignedCallChain> 
    CallChainLRUCache;

  /* dado uma hash de um profile de uma requisição eu consigo obter uma sessão
   * que me permite uma comunicação com o objeto CORBA que está sendo
   * requisitado.
   */
  SessionLRUCache _sessionLRUCache;    
    
  CallChainLRUCache _callChainLRUCache;
  OpenBusContext *_openbusContext;
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::mutex _mutex;
#endif
};

class IgnoreInterceptor 
{
public:
  IgnoreInterceptor(PI::Current &c) : _piCurrent(c) 
  {
    CORBA::Any ignoreInterceptorAny;
    ignoreInterceptorAny <<= CORBA::Any::from_boolean(true);
    _piCurrent.set_slot(ClientInterceptor::_slotId_ignoreInterceptor, 
                        ignoreInterceptorAny);
  }

  ~IgnoreInterceptor() 
  {
    try
    {
      CORBA::Any ignoreInterceptorAny;
      ignoreInterceptorAny <<= CORBA::Any::from_boolean(false);
      _piCurrent.set_slot(ClientInterceptor::_slotId_ignoreInterceptor, 
                          ignoreInterceptorAny); 
    } catch (...)
    {
    }
  }

  static bool status(PI::ClientRequestInfo &r) 
  {
    CORBA::Any_var any = 
      r.get_slot(ClientInterceptor::_slotId_ignoreInterceptor);
    CORBA::Boolean b = 0;
    return ( (*any >>= CORBA::Any::to_boolean(b)) ? b : false );
  }
private:
  PI::Current &_piCurrent;
};
}
}

#endif
