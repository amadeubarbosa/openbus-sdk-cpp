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
      * Cria uma conex�o para um barramento a partir de um endere�o de rede IP e uma porta.
      * 
      * @param[in] host Endere�o de rede IP onde o barramento est� executando.
      * @param[in] port Porta do processo do barramento no endere�o indicado.
      * @param[in] orb ORB a ser utilizado.
      * 
      * @return Conex�o ao barramento referenciado.
      */
      Connection(
        const std::string host,
        const unsigned int port,
        CORBA::ORB* orb,
        const interceptors::ORBInitializer* orbInitializer) throw(CORBA::Exception);
      ~Connection();
      
      /**
      * Efetua login no barramento como uma entidade usando autentica��o por senha.
      * 
      * @param[in] entity Identificador da entidade a ser conectada.
      * @param[in] password Senha de autentica��o da entidade no barramento.
      * 
      * @throw AccessDenied Senha fornecida  para  autentica��o  da  entidade n�o foi validada pelo 
      *        barramento.
      * @throw AlreadyLogged A conex�o j� est� logada.
      * @throw ServiceFailure Ocorreu  uma  falha interna nos servi�os do barramento que impediu  o
      *        estabelecimento da conex�o.
      */
      void loginByPassword(const char* entity, const char* password)
      throw (
        AlreadyLogged, 
        openbusidl_access_control::AccessDenied, 
        openbusidl_access_control::WrongEncoding,
        openbusidl::services::ServiceFailure,
        CORBA::Exception);

     /**
     * Efetua login no barramento como uma entidade usando autentica��o por certificado.
     * 
     * @param [in] entity Identificador da entidade a ser conectada.
     * @param [in] privateKey Chave privada da entidade utilizada na autentica��o.
     * 
     * @exception MissingCertificate N�o h� certificado para essa entidade registrado no barramento
     *            indicado.
     * @exception CorruptedPrivateKey A chave privada fornecida est� corrompida.
     * @exception CorruptedBusCertificate O certificado do barramento est� corrompida.
     * @exception WrongPrivateKey A chave privada  fornecida  n�o  corresponde  ao  certificado  da 
     *            entidade registrado no barramento indicado.
     * @exception AlreadyLogged A conex�o j� est� logada.
  	 * @exception WrongEncoding A autentica��o falhou, pois a resposta ao desafio n�o foi codificada
  	 *                          corretamente com a chave p�blica do barramento.
  	 * @exception AccessDenied A autentica��o falhou, pois o dado codificado na
  	 *                         resposta ao desafio n�o � o esperado.
     * @exception ServiceFailure Ocorreu uma falha interna nos servi�os do barramento que impediu o
     *            estabelecimento da conex�o.
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
      * \return Verdadeiro se o processo de logout for conclu�do com �xito e 
      *         falso se n�o houver um login ativo para a conex�o ou se alguma 
      *         exce��o for lan�ada pelo barramento. 
      */
      bool logout();

      /*
      * Encerra essa conex�o, tornando-a inv�lida daqui em diante.
      */
      void close();
      
      Chain* getCallerChain();

      bool isLoggedIn() const { return _loginInfo.get(); }

      /**
      * Retorna o ORB associado a esta conex�o.
      *
      * @return ORB associado a esta conex�o.
      */
      CORBA::ORB* orb() const { return _orb; }
    
      /**
      * Retorna a identifica��o do barramento que est� sendo referenciado.
      *
      * @return Identifica��o do barramento.
      */
      const char* busId() const { return _busId; }

      /**
      * Retorna a refer�ncia para o servi�o de acesso do barramento.
      *
      * @return Refer�ncia para o servi�o de acesso do barramento.
      */
      const openbusidl_access_control::AccessControl_var access_control() const 
        { return _access_control; }
    
      const openbusidl_offer_registry::OfferRegistry_var offer_registry() const 
        { return _offer_registry; }

      const openbusidl_access_control::LoginRegistry_var login_registry() const 
        { return _login_registry; }

      /**
      * Verifica o estado da conex�o.
      *
      * @return Retorna 'true' se h� um login associado a conex�o e 'false' caso contr�rio.
      */
      openbusidl_access_control::LoginInfo* loginInfo() const { return _loginInfo.get(); }
          
      EVP_PKEY* prvKey() const { return _prvKey; }
      // openbusidl_access_control::CallerChain* callerChain() const
      //   { return _callerChain.get(); }
  
    private:
      /** Endere�o de rede onde o barramento est� executando. */
      std::string _host;
      
      /** Porta do processo do barramento no endere�o indicado. */
      unsigned int _port;
      
      /** ORB utilizado pela conex�o. */
      CORBA::ORB* _orb;
      
      /** Inicializador do ORB. */
      const interceptors::ORBInitializer* _orbInitializer;

      /** Refer�ncia para o interceptador cliente. */
      interceptors::ClientInterceptor* _clientInterceptor;

      /** Refer�ncia para o interceptador servidor. */
      interceptors::ServerInterceptor* _serverInterceptor;

      /** Refer�ncia para a thread repons�vel pela renova��o do login. */
      std::auto_ptr<RenewLogin> _renewLogin;

      /** Dados do login. */
      std::auto_ptr<openbusidl_access_control::LoginInfo> _loginInfo;

      /** Refer�ncia para o SCS::IComponent do barramento. */
      scs::core::IComponent_var _iComponent;

      /** Refer�ncia para o servi�o de acesso do barramento.  */
      openbusidl_access_control::AccessControl_var _access_control;
    
      /** Refer�ncia para o servi�o de login.  */
      openbusidl_access_control::LoginRegistry_var _login_registry;

      /** Refer�ncia para o servi�o de ofertas.  */
      openbusidl_offer_registry::OfferRegistry_var _offer_registry;

      /** Identifica��o do barramento. */
      openbusidl::Identifier_var _busId;
      
      /** Chave p�blica do barramento. */
      EVP_PKEY* _buskey;
      openbusidl::OctetSeq_var buskeyOctetSeq;
      
      /** Chave privada associada a esta conex�o. */
      EVP_PKEY* _prvKey;
      
      /** Cadeia de chamadas do barramento. */
      // std::auto_ptr<openbusidl_access_control::CallerChain> _callerChain; 
  };
}

#endif
