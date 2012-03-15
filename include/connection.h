#ifndef TECGRAF_CONNECTION_H_
#define TECGRAF_CONNECTION_H_

#include <memory>
#include <stdexcept>
#include <string>
#include <CORBA.h>
#include <openssl/evp.h>
#include "interceptors/orbInitializer_impl.h"
#include "stubs/scs.h"
#include "stubs/core.h"
#include "stubs/credential.h"
#include "stubs/access_control.h"
#include "stubs/offer_registry.h"

namespace openbus {  
  namespace openbusidl = tecgraf::openbus::core::v2_00;
  namespace openbusidl_access_control = tecgraf::openbus::core::v2_00::services::access_control;
  namespace openbusidl_offer_registry = tecgraf::openbus::core::v2_00::services::offer_registry;
  namespace openbusidl_credential = tecgraf::openbus::core::v2_00::credential;
  
  namespace interceptors {
    class ClientInterceptor;
    class ServerInterceptor;
    class ORBInitializer;
  }
  class RenewLogin;

  struct Chain {
    openbusidl_access_control::LoginInfoSeq callers;
    char* target;
    char* busId;
  };

  class Connection {
    public:
      /** Exceptions */
      class Exception {
        public:
          virtual const char* name() const 
          { return "Exception"; }
      };
        
      class AlreadyLogged : public Exception {
        public:
          const char* name() const 
            { return "AlreadyLogged"; }
      };

      class CorruptedPrivateKey : public Exception {
        public:
          const char* name() const 
            { return "CorruptedPrivateKey"; }
      };

      class CorruptedBusCertificate : public Exception {
        public:
          const char* name() const 
            { return "CorruptedBusCertificate"; }
      };

      class WrongPrivateKey : public Exception {
        public:
          const char* name() const 
            { return "WrongPrivateKey"; }
      };
      
      /**
      * Cria uma conexão para um barramento a partir de um endereço de rede IP e uma porta.
      * 
      * @param[in] host Endereço de rede IP onde o barramento está executando.
      * @param[in] port Porta do processo do barramento no endereço indicado.
      * @param[in] orb ORB a ser utilizado.
      * 
      * @return Conexão ao barramento referenciado.
      */
      Connection(
        const std::string host,
        const unsigned int port,
        CORBA::ORB* orb,
        const interceptors::ORBInitializer* orbInitializer) throw(CORBA::Exception);
      ~Connection();
      
      /**
      * Efetua login no barramento como uma entidade usando autenticação por senha.
      * 
      * @param[in] entity Identificador da entidade a ser conectada.
      * @param[in] password Senha de autenticação da entidade no barramento.
      * 
      * @throw AccessDenied Senha fornecida  para  autenticação  da  entidade não foi validada pelo 
      *        barramento.
      * @throw AlreadyLogged A conexão já está logada.
      * @throw ServiceFailure Ocorreu  uma  falha interna nos serviços do barramento que impediu  o
      *        estabelecimento da conexão.
      */
      void loginByPassword(const char* entity, const char* password)
      throw (
        AlreadyLogged, 
        openbusidl_access_control::AccessDenied, 
        openbusidl_access_control::WrongEncoding,
        openbusidl::services::ServiceFailure,
        CORBA::Exception);

     /**
     * Efetua login no barramento como uma entidade usando autenticação por certificado.
     * 
     * @param [in] entity Identificador da entidade a ser conectada.
     * @param [in] privateKey Chave privada da entidade utilizada na autenticação.
     * 
     * @exception MissingCertificate Não há certificado para essa entidade registrado no barramento
     *            indicado.
     * @exception CorruptedPrivateKey A chave privada fornecida está corrompida.
     * @exception CorruptedBusCertificate O certificado do barramento está corrompida.
     * @exception WrongPrivateKey A chave privada  fornecida  não  corresponde  ao  certificado  da 
     *            entidade registrado no barramento indicado.
     * @exception AlreadyLogged A conexão já está logada.
  	 * @exception WrongEncoding A autenticação falhou, pois a resposta ao desafio não foi codificada
  	 *                          corretamente com a chave pública do barramento.
  	 * @exception AccessDenied A autenticação falhou, pois o dado codificado na
  	 *                         resposta ao desafio não é o esperado.
     * @exception ServiceFailure Ocorreu uma falha interna nos serviços do barramento que impediu o
     *            estabelecimento da conexão.
     */
     void loginByCertificate(const char* entity, EVP_PKEY* privateKey)
       throw (
         CorruptedPrivateKey, 
         CorruptedBusCertificate,
         WrongPrivateKey,
         AlreadyLogged, 
         openbusidl_access_control::MissingCertificate, 
         openbusidl_access_control::AccessDenied, 
         openbusidl_access_control::WrongEncoding,
         openbusidl::services::ServiceFailure,
         CORBA::Exception);

     void loginByCertificate(const char* entity, const char* privateKeyFilename)
       throw (
         CorruptedPrivateKey, 
         CorruptedBusCertificate,
         WrongPrivateKey,
         AlreadyLogged, 
         openbusidl_access_control::MissingCertificate, 
         openbusidl_access_control::AccessDenied, 
         openbusidl_access_control::WrongEncoding,
         openbusidl::services::ServiceFailure,
         CORBA::Exception);

      /**
      * Efetua logout no barramento.
      *  
      * \return Verdadeiro se o processo de logout for concluído com êxito e 
      *         falso se não houver um login ativo para a conexão ou se alguma 
      *         exceção for lançada pelo barramento. 
      */
      bool logout();

      /*
      * Encerra essa conexão, tornando-a inválida daqui em diante.
      */
      void close();
      
      Chain* getCallerChain();

      bool isLoggedIn() const { return _loginInfo.get(); }

      /**
      * Retorna o ORB associado a esta conexão.
      *
      * @return ORB associado a esta conexão.
      */
      CORBA::ORB* orb() const { return _orb; }
    
      /**
      * Retorna a identificação do barramento que está sendo referenciado.
      *
      * @return Identificação do barramento.
      */
      const char* busId() const { return _busId; }

      /**
      * Retorna a referência para o serviço de acesso do barramento.
      *
      * @return Referência para o serviço de acesso do barramento.
      */
      const openbusidl_access_control::AccessControl_var access_control() const 
        { return _access_control; }
    
      const openbusidl_offer_registry::OfferRegistry_var offer_registry() const 
        { return _offer_registry; }

      const openbusidl_access_control::LoginRegistry_var login_registry() const 
        { return _login_registry; }

      /**
      * Verifica o estado da conexão.
      *
      * @return Retorna 'true' se há um login associado a conexão e 'false' caso contrário.
      */
      openbusidl_access_control::LoginInfo* loginInfo() const { return _loginInfo.get(); }
          
      EVP_PKEY* prvKey() const { return _prvKey; }
      // openbusidl_access_control::CallerChain* callerChain() const
      //   { return _callerChain.get(); }
  
    private:
      /** Endereço de rede onde o barramento está executando. */
      std::string _host;
      
      /** Porta do processo do barramento no endereço indicado. */
      unsigned int _port;
      
      /** ORB utilizado pela conexão. */
      CORBA::ORB* _orb;
      
      /** Inicializador do ORB. */
      const interceptors::ORBInitializer* _orbInitializer;

      /** Referência para o interceptador cliente. */
      interceptors::ClientInterceptor* _clientInterceptor;

      /** Referência para o interceptador servidor. */
      interceptors::ServerInterceptor* _serverInterceptor;

      /** Referência para a thread reponsável pela renovação do login. */
      std::auto_ptr<RenewLogin> _renewLogin;

      /** Dados do login. */
      std::auto_ptr<openbusidl_access_control::LoginInfo> _loginInfo;

      /** Referência para o SCS::IComponent do barramento. */
      scs::core::IComponent_var _iComponent;

      /** Referência para o serviço de acesso do barramento.  */
      openbusidl_access_control::AccessControl_var _access_control;
    
      /** Referência para o serviço de login.  */
      openbusidl_access_control::LoginRegistry_var _login_registry;

      /** Referência para o serviço de ofertas.  */
      openbusidl_offer_registry::OfferRegistry_var _offer_registry;

      /** Identificação do barramento. */
      openbusidl::Identifier_var _busId;
      
      /** Chave pública do barramento. */
      EVP_PKEY* _buskey;
      openbusidl::OctetSeq_var buskeyOctetSeq;
      
      /** Chave privada associada a esta conexão. */
      EVP_PKEY* _prvKey;
      
      /** Cadeia de chamadas do barramento. */
      // std::auto_ptr<openbusidl_access_control::CallerChain> _callerChain; 
  };
}

#endif
