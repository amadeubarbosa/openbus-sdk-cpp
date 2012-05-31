#ifndef TECGRAF_CLIENTINTERCEPTOR_IMPL_H_
#define TECGRAF_CLIENTINTERCEPTOR_IMPL_H_

#include <CORBA.h>
#include "connection.h"
#include "../manager.h"

/* forward declarations */
namespace openbus {
  class Connection;
  class ConnectionManager;
}

namespace openbus {
namespace interceptors {
  class ClientInterceptor : public PortableInterceptor::ClientRequestInterceptor {
  public:
    ClientInterceptor(
      PortableInterceptor::SlotId slotId_connectionAddr,
      PortableInterceptor::SlotId slotId_joinedCallChain,
      IOP::Codec* cdr_codec);
    ~ClientInterceptor();
    void send_request(PortableInterceptor::ClientRequestInfo*) throw (CORBA::Exception);
    void send_poll(PortableInterceptor::ClientRequestInfo*) throw (CORBA::Exception) { }
    void receive_reply(PortableInterceptor::ClientRequestInfo*) throw (CORBA::Exception) { }
    void receive_exception(PortableInterceptor::ClientRequestInfo*) 
      throw (CORBA::Exception, PortableInterceptor::ForwardRequest);
    void receive_other(PortableInterceptor::ClientRequestInfo* ri) 
      throw (CORBA::Exception, PortableInterceptor::ForwardRequest) { }
    char* name() throw (CORBA::Exception) { return CORBA::string_dup("ClientInterceptor"); }
    void destroy() { }
    void setConnectionManager(ConnectionManager* m) { _manager = m; }
    void resetCaches() { _session.clear(); }
    Connection& getCurrentConnection(PortableInterceptor::ClientRequestInfo*);
    bool allowRequestWithoutCredential;
  private:
    IOP::Codec* _cdrCodec;
    ConnectionManager* _manager;
    PortableInterceptor::SlotId _slotId_connectionAddr;
    PortableInterceptor::SlotId _slotId_joinedCallChain;
    struct SecretSession {
      CORBA::ULong id;
      char* remoteid;
      unsigned char* secret;
      CORBA::ULong ticket;
    };
    /* dado uma hash de um profile de uma requisição eu consigo obter uma sessão que me permite 
    ** uma comunicação com o objeto CORBA que está sendo requisitado. */
    std::map<std::string, SecretSession*> _session;
  };
}
}

#endif
