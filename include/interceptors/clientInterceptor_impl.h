#ifndef TECGRAF_CLIENTINTERCEPTOR_IMPL_H_
#define TECGRAF_CLIENTINTERCEPTOR_IMPL_H_

#include <CORBA.h>
#include <connection.h>
#include <multiplexer.h>

namespace openbus {
  class Connection;
  namespace multiplexed {
    class ConnectionMultiplexer;
  }
}

namespace openbus {
  namespace interceptors {
    class ClientInterceptor : public PortableInterceptor::ClientRequestInterceptor {
    public:
      ClientInterceptor(PortableInterceptor::SlotId slotId_joinedCallChain,IOP::Codec* cdr_codec);
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
      void setConnection(Connection* c) { _conn = c; }
      void setConnectionMultiplexer(multiplexed::ConnectionMultiplexer* m) { _multiplexer = m; }
      void resetCaches() { _profileSecretSession.clear(); }

      /* Flag que indica ao interceptador cliente se este deve verificar a existencia de uma  
      ** credencial anexada a chamada remota em execucao
      */
      bool allowRequestWithoutCredential;
    private:
      IOP::Codec* _cdrCodec;
      Connection* _conn;
      multiplexed::ConnectionMultiplexer* _multiplexer;
      PortableInterceptor::SlotId _slotId_joinedCallChain;
      struct SecretSession {
        CORBA::ULong id;
        char* remoteid;
        unsigned char* secret;
        CORBA::ULong ticket;
      };
      std::map<std::string, SecretSession*> _profileSecretSession;
    };
  }
}

#endif
