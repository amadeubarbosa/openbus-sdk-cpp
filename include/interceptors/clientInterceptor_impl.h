#ifndef TECGRAF_CLIENTINTERCEPTOR_IMPL_H_
#define TECGRAF_CLIENTINTERCEPTOR_IMPL_H_

#include <CORBA.h>
#include <connection.h>

namespace openbus {
  class Connection;
  namespace interceptors {
    class ClientInterceptor : public PortableInterceptor::ClientRequestInterceptor {
    public:
      ClientInterceptor(
        PortableInterceptor::SlotId slotId_joinedCallChain,
        IOP::Codec* cdr_codec);
      ~ClientInterceptor();
      void send_request(PortableInterceptor::ClientRequestInfo*) throw (CORBA::Exception);
      void send_poll(PortableInterceptor::ClientRequestInfo*) throw (CORBA::Exception) { }
      void receive_reply(PortableInterceptor::ClientRequestInfo*) throw (CORBA::Exception) { }
      void receive_exception(PortableInterceptor::ClientRequestInfo*) 
      throw (
        CORBA::Exception, 
        PortableInterceptor::ForwardRequest);
      void receive_other(PortableInterceptor::ClientRequestInfo* ri) 
      throw (
        CORBA::Exception, 
        PortableInterceptor::ForwardRequest) { }
      char* name() throw (CORBA::Exception) { return CORBA::string_dup("ClientInterceptor"); }
      void destroy() { }
      void setConnection(Connection* c) { _conn = c; }

      /** Flag que indica ao interceptador cliente se este deve verificar a existência de uma  
      *   credencial anexada a chamada remota em execução
      */
      bool allowRequestWithoutCredential;
    private:
      IOP::Codec* _cdrCodec;
      Connection* _conn;
      PortableInterceptor::SlotId _slotId_joinedCallChain;
      struct CredentialSession {
        CORBA::ULong id;
        char* remoteid;
        unsigned char* secret;
        CORBA::ULong ticket;
      };
      std::map<std::string, std::string> _profile2login;
      std::map<std::string, CredentialSession*> _login2credsession;
    };
  }
}

#endif
