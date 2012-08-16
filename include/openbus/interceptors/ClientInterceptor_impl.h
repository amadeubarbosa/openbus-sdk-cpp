#ifndef TECGRAF_CLIENTINTERCEPTOR_IMPL_H_
#define TECGRAF_CLIENTINTERCEPTOR_IMPL_H_

#include <CORBA.h>

#include "openbus/Connection_impl.h"
#include "openbus/ConnectionManager.h"
#include "openbus/Connection.h"
#ifndef TECGRAF_LRUCACHE_H_
#define TECGRAF_LRUCACHE_H_
#include "openbus/util/LRUCache_impl.h"
#endif

namespace openbus {
namespace interceptors {  
  class ClientInterceptor : public PortableInterceptor::ClientRequestInterceptor {
  public:
    ClientInterceptor(
      PortableInterceptor::SlotId slotId_requesterConnection,
      PortableInterceptor::SlotId slotId_joinedCallChain,
      PortableInterceptor::SlotId slotId_ignoreInterceptor,
      IOP::Codec *cdr_codec);
    ~ClientInterceptor();
    void send_request(PortableInterceptor::ClientRequestInfo*);
    void send_poll(PortableInterceptor::ClientRequestInfo*) { }
    void receive_reply(PortableInterceptor::ClientRequestInfo*) { }
    void receive_exception(PortableInterceptor::ClientRequestInfo*);
    void receive_other(PortableInterceptor::ClientRequestInfo *ri) { }
    char *name() { return CORBA::string_dup("ClientInterceptor"); }
    void destroy() { }
    void connectionManager(ConnectionManager *m) { _manager = m; }
    Connection &getCurrentConnection(PortableInterceptor::ClientRequestInfo*);
    openbus::CallerChain *getJoinedChain(PortableInterceptor::ClientRequestInfo *r);
    static PortableInterceptor::SlotId _slotId_ignoreInterceptor;
  private:

    /* Variáveis que são modificadas somente no construtor. */
    IOP::Codec *_cdrCodec;
    PortableInterceptor::SlotId _slotId_requesterConnection;
    PortableInterceptor::SlotId _slotId_joinedCallChain;
    /**/
    
    struct SecretSession {
      CORBA::ULong id;
      CORBA::String_var remoteId;
      CORBA::OctetSeq_var secret;
      CORBA::ULong ticket;
    };

    typedef LRUCache<std::string, SecretSession> SessionLRUCache;
    typedef LRUCache<std::string, const idl_cr::SignedCallChain> CallChainLRUCache;

    /* dado uma hash de um profile de uma requisição eu consigo obter uma sessão que me permite 
    ** uma comunicação com o objeto CORBA que está sendo requisitado. */
    SessionLRUCache _sessionLRUCache;    
    
    CallChainLRUCache _callChainLRUCache;
    ConnectionManager *_manager;
    Mutex _mutex;
  };

  class IgnoreInterceptor {
  public:
    IgnoreInterceptor(PortableInterceptor::Current *c) : _piCurrent(c) {
      CORBA::Any ignoreInterceptorAny;
      ignoreInterceptorAny <<= CORBA::Any::from_boolean(true);
      _piCurrent->set_slot(ClientInterceptor::_slotId_ignoreInterceptor, ignoreInterceptorAny);
    }
    ~IgnoreInterceptor() {
      CORBA::Any ignoreInterceptorAny;
      ignoreInterceptorAny <<= CORBA::Any::from_boolean(false);
      _piCurrent->set_slot(ClientInterceptor::_slotId_ignoreInterceptor, ignoreInterceptorAny);      
    }
    static bool status(PortableInterceptor::ClientRequestInfo *r) {
      CORBA::Any_var any = r->get_slot(ClientInterceptor::_slotId_ignoreInterceptor);
      CORBA::Boolean b = 0;
      if (*any >>= CORBA::Any::to_boolean(b)) return b;
      else return 0;
    }
  private:
    PortableInterceptor::Current *_piCurrent;
  };
}
}

#endif
