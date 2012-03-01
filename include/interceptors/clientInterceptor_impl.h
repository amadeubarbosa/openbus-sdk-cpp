#ifndef TECGRAF_CLIENTINTERCEPTOR_IMPL_H_
#define TECGRAF_CLIENTINTERCEPTOR_IMPL_H_

#include <CORBA.h>
#include <connection.h>

namespace openbus {
  class Connection;
  namespace interceptors {
    class ClientInterceptor : public PortableInterceptor::ClientRequestInterceptor {
      public:
        ClientInterceptor(IOP::Codec* cdr_codec);
        ~ClientInterceptor();
        void send_request(PortableInterceptor::ClientRequestInfo* ri)
          throw (CORBA::Exception);
        void send_poll(PortableInterceptor::ClientRequestInfo* ri) 
          throw (CORBA::Exception) { }
        void receive_reply(PortableInterceptor::ClientRequestInfo* ri)
          throw (CORBA::Exception) { }
        void receive_exception(PortableInterceptor::ClientRequestInfo* ri)
          throw (CORBA::Exception, PortableInterceptor::ForwardRequest);
        void receive_other(PortableInterceptor::ClientRequestInfo* ri) 
          throw (CORBA::Exception, PortableInterceptor::ForwardRequest) { }
        char* name() throw (CORBA::Exception) 
          { return CORBA::string_dup("ClientInterceptor"); }
        void destroy() { }
        void addConnection(Connection* connection);
        void removeConnection(Connection* connection);

        /** Flag que indica ao interceptador cliente se este deve verificar a existência de uma  
        *   credencial anexada a chamada remota em execução
        */
        bool allowRequestWithoutCredential;
      private:
        IOP::Codec* cdr_codec;
        Connection* connection;
        struct CredentialSession {
          char* remoteid;
          unsigned char* secret;
          CORBA::ULong ticket;
        };
        std::map<std::string, std::string> profile2login;
        std::map<std::string, CredentialSession*> login2credsession;
    };
  }
}

#endif
