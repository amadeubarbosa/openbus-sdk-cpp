/**
* API - SDK Openbus C++
* \file manager.h
*/

#ifndef TECGRAF_CONNECTION_H_
#define TECGRAF_CONNECTION_H_

#include <CORBA.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <openssl/evp.h>

#include "stubs/scs.h"
#include "stubs/core.h"
#include "stubs/credential.h"
#include "stubs/access_control.h"
#include "stubs/offer_registry.h"

namespace openbus {  
  namespace idl = tecgraf::openbus::core::v2_00;
  namespace idl_ac = tecgraf::openbus::core::v2_00::services::access_control;
  namespace idl_accesscontrol = idl_ac;
  namespace idl_or = tecgraf::openbus::core::v2_00::services::offer_registry;
  namespace idl_offerregistry = idl_or;
  namespace idl_cr = tecgraf::openbus::core::v2_00::credential;
  namespace idl_credential = idl_cr;
}

#include "interceptors/orbInitializer_impl.h"
#ifdef OPENBUS_SDK_MULTITHREAD
#include "util/mutex_impl.h"
#endif

/* forward declarations */
namespace openbus {
  class LoginCache;
  class RenewLogin;
  struct CallerChain;  
  namespace interceptors {
    class ClientInterceptor;
    class ServerInterceptor;
    class ORBInitializer;
  }
  class ConnectionManager;
}

/**
* \brief openbus
*/
namespace openbus {  
  struct AccessDenied  { const char* name() const { return "openbus::AccessDenied"; } };
  struct AlreadyLoggedIn { const char* name() const { return "openbus::AlreadyLoggedIn"; } };
  struct CorruptedPrivateKey{ const char* name() const { return "openbus::CorruptedPrivateKey";}};
  struct WrongPrivateKey {const char* name() const { return "openbus::WrongPrivateKey"; } };
  struct WrongSecret { const char* name() const { return "openbus::WrongSecret"; } };
  struct InvalidLoginProcess{ const char* name() const { return "openbus::InvalidLoginProcess";}};

  /**
  * Conexão com um barramento.
  */
  class Connection {
  public:
    /**
    * Callback de expiração de login.
    *
    * @return 'true' se a chamada que recebeu a indicação que o login se tornou inválido deve ser 
    * refeita, ou 'false' caso a execção de NO_PERMISSION deve ser lançada.
    */
    typedef bool (*InvalidLoginCallback_ptr) (const Connection*, const idl_ac::LoginInfo*);
  
	  /**
	  * Efetua login no barramento como uma entidade usando autenticação por senha.
	  * 
	  * @param[in] entity Identificador da entidade a ser conectada.
	  * @param[in] password Senha de autenticação da entidade no barramento.
	  * 
	  * @throw AccessDenied Senha fornecida para autenticação da entidade não foi validada pelo 
	  *        barramento.
	  * @throw AlreadyLoggedIn A conexão já está logada.
	  * @throw ServiceFailure Ocorreu uma falha interna nos serviços do barramento que impediu o 
	  *        tabelecimento da conexão.
	  * @throw CORBA::Exception
	  */
    void loginByPassword(const char* entity, const char* password)
      throw (AccessDenied, AlreadyLoggedIn, idl::services::ServiceFailure, CORBA::Exception);

	  /**
	  * Efetua login no barramento como uma entidade usando autenticação por certificado.
	  * 
	  * @param[in] entity Identificador da entidade a ser conectada.
	  * @param[in] privateKey Chave privada da entidade utilizada na autenticação. Essa chave é 
	  *            passada através de um ponteiro para uma estrutura OpenSSL do tipo EVP_PKEY. 
	  * 
	  * @throw CorruptedPrivateKey A chave privada fornecida está corrompida.
	  * @throw WrongPrivateKey A chave privada fornecida não corresponde ao certificado da entidade 
	  *        registrado no barramento indicado.
	  * @throw AlreadyLoggedIn A conexão já está logada.
	  * @throw MissingCertificate Não há certificado para essa entidade registrado no barramento 
	  *        indicado.
	  * @throw ServiceFailure Ocorreu uma falha interna nos serviços do barramento que impediu o 
	  *        estabelecimento da conexão.
	  * @throw CORBA::Exception
	  */
    void loginByCertificate(const char* entity, EVP_PKEY* privateKey)
      throw (CorruptedPrivateKey, WrongPrivateKey, AlreadyLoggedIn, idl_ac::MissingCertificate, 
      idl::services::ServiceFailure, CORBA::Exception);

	  /**
	  * Efetua login no barramento como uma entidade usando autenticação por certificado.
	  * 
	  * @param[in] entity Identificador da entidade a ser conectada.
	  * @param[in] privateKeyFilename Nome do arquivo que contêm a chave privada da entidade utilizada 
	  *            na autenticação. Esse valor será repassado para uma função fopen().
	  * 
	  * @throw CorruptedPrivateKey A chave privada fornecida está corrompida.
	  * @throw WrongPrivateKey A chave privada fornecida não corresponde ao certificado da entidade 
	  *        registrado no barramento indicado.
	  * @throw AlreadyLoggedIn A conexão já está logada.
	  * @throw MissingCertificate Não há certificado para essa entidade registrado no barramento 
	  *        indicado.
	  * @throw ServiceFailure Ocorreu uma falha interna nos serviços do barramento que impediu o 
	  *        estabelecimento da conexão.
	  * @throw CORBA::Exception
	  */
    void loginByCertificate(const char* entity, const char* privateKeyFilename)
      throw (CorruptedPrivateKey, WrongPrivateKey, AlreadyLoggedIn, idl_ac::MissingCertificate, 
      idl::services::ServiceFailure, CORBA::Exception);
    
	  /**
	  * Inicia o processo de login por single sign-on.
	  * 
	  * @return Um par composto de um objeto que representa o processo de login iniciado e de um 
	  *         segredo a ser fornecido na conclusão do processo de login.
    *
	  * @throw ServiceFailure Ocorreu uma falha interna nos serviços do barramento que impediu o 
	  *        estabelecimento da conexão.
	  * @throw CORBA::Exception
	  */
    std::pair <idl_ac::LoginProcess*, unsigned char*> startSingleSignOn() 
      throw (idl::services::ServiceFailure, CORBA::Exception);
      
	  /**
	  * Efetua login no barramento como uma entidade usando autenticação por single sign-on.
	  * 
	  * @param[in] loginProcess Objeto que represeta o processo de login iniciado.
	  * @param[in] secret Segredo a ser fornecido na conclusão do processo de login.
	  * 
	  * @throw WrongSecret O segredo fornecido não corresponde ao esperado pelo barramento.
	  * @throw InvalidLoginProcess O LoginProcess informado é inválido, por exemplo depois de ser 
	  *        cancelado ou ter expirado.
	  * @throw AlreadyLoggedIn A conexão já está logada.
	  * @throw ServiceFailure Ocorreu uma falha interna nos serviços do barramento que impediu o 
	  *        estabelecimento da conexão.
	  * @throw CORBA::Exception
	  */
    void loginBySingleSignOn(idl_ac::LoginProcess* loginProcess, unsigned char* secret)
  		throw(WrongSecret, InvalidLoginProcess, AlreadyLoggedIn, idl::services::ServiceFailure, 
  		CORBA::Exception);
          
	  /**
	  * Efetua logout no barramento. Se a sua conexão for uma conexão de despacho, remove essa conexão
	  * como despachante do ConnectionManager.
	  *  
	  * @return true se o processo de logout for concluído com êxito e false se a conexão já estiver 
	  * deslogada (login inválido).
	  *
	  * @throw CORBA::Exception
	  */
	  bool logout() throw (CORBA::Exception);
	  
	  /**
	  * Caso a thread corrente seja a thread de execução de uma chamada remota oriunda do barramento 
	  * dessa conexão, essa operação devolve um objeto que representa a cadeia de chamadas do 
	  * barramento que esta chamada faz parte. Caso contrário devolve zero.
	  * 
	  * @return Cadeia da chamada em execução.
	  *
	  * @throw CORBA::Exception
	  */
    CallerChain* getCallerChain() throw (CORBA::Exception);
    
	  /**
	  * Associa uma cadeia de chamadas do barramento a thread corrente, de forma que todas as chamadas
	  * remotas seguintes dessa thread através dessa conexão sejam feitas como parte dessa cadeia de 
	  * chamadas.
	  *
	  * @param[in] chain Cadeia de chamadas a ser associada.
	  *
	  * @throw CORBA::NO_PERMISSION { minor = NoLogin }
	  * @throw CORBA::NO_PERMISSION { minor = InvalidChain }
	  * @throw CORBA::Exception
	  */
	  void joinChain(CallerChain* chain) throw (CORBA::Exception);

    /**
    * Remove a associação da cadeia de chamadas com a thread corrente, fazendo com que todas as 
    * chamadas seguintes da thread corrente feitas através dessa conexão	deixem de fazer parte da 
    * cadeia de chamadas associada previamente. Ou seja, todas as chamadas passam a iniciar novas 
    * cadeias de chamada.
    *
	  * @throw CORBA::Exception
    */
    void exitChain() throw (CORBA::Exception);
    
	  /**
	  * Devolve um objeto que representa a cadeia de chamadas associada a thread atual nessa conexão. 
	  * A cadeia de chamadas informada foi associada previamente pela operação 'joinChain'. Caso a 
	  * thread corrente não tenha nenhuma cadeia associada, essa operação devolve zero.
	  * 
	  * @return Cadeia de chamadas associada.
    *
	  * @throw CORBA::Exception
	  */
    CallerChain* getJoinedChain() throw (CORBA::Exception);
    
	  /*
	  * Define a callback a ser chamada sempre que o login se torna inválido.
	  * @param[in] p Ponteiro para uma função de callback que implementa a interface de callback a ser
	  *            chamada ou zero caso nenhum objeto deva ser chamado na ocorrência desse evento.
	  */
    void onInvalidLogin(InvalidLoginCallback_ptr p) { _onInvalidLogin = p; }
    
	  /*
	  * Retorna um ponteiro para uma função de callback a ser chamada sempre que o login se torna 
	  * inválido.
	  */
    InvalidLoginCallback_ptr onInvalidLogin() const { return _onInvalidLogin; }   
       
  	/** 
  	* Informações sobre o login da entidade que autenticou essa conexão. 
  	*/
    const idl_ac::LoginInfo* login() const { return _loginInfo.get(); }
    
  	/** 
  	* Barramento ao qual essa conexão se refere. 
  	*/
    const char* busid() const { return CORBA::string_dup(_busid); }
    
    /**
    * Obtenção do serviço de ofertas.
    */
    const idl_or::OfferRegistry_var offers() const { return _offer_registry; }
    
    ~Connection();
  private:
    Connection(const std::string host, const unsigned int port, CORBA::ORB*, 
      const interceptors::ORBInitializer*, ConnectionManager*) throw(CORBA::Exception);
    void fetchBusKey();
    const idl_ac::LoginRegistry_var login_registry() const { return _login_registry; }
    const idl_ac::AccessControl_var access_control() const { return _access_control; }
    EVP_PKEY* key() const { return _key; }
    EVP_PKEY* busKey() const { return _busKey; }
    bool _logout(bool local);
    std::string _host;
    unsigned int _port;
    CORBA::ORB* orb() const { return _orb; }
    CORBA::ORB* _orb;
    PortableInterceptor::Current_var _piCurrent;
    const interceptors::ORBInitializer* _orbInitializer;
    interceptors::ClientInterceptor* _clientInterceptor;
    interceptors::ServerInterceptor* _serverInterceptor;
    std::auto_ptr<RenewLogin> _renewLogin;
    std::auto_ptr<idl_ac::LoginInfo> _loginInfo;
    scs::core::IComponent_var _iComponent;
    idl_ac::AccessControl_var _access_control;
    idl_ac::LoginRegistry_var _login_registry;
    idl_or::OfferRegistry_var _offer_registry;
    char* _busid;
    EVP_PKEY* _busKey;
    idl::OctetSeq_var _buskeyOctetSeq;
    EVP_PKEY* _key;
    InvalidLoginCallback_ptr _onInvalidLogin;
    std::auto_ptr<LoginCache> _loginCache;
    ConnectionManager* _manager;
    #ifdef OPENBUS_SDK_MULTITHREAD
    MICOMT::Mutex _mutex;
    #endif
    friend class openbus::interceptors::ServerInterceptor;
    friend class openbus::interceptors::ClientInterceptor;
    friend class RenewLogin;
    friend class LoginCache;
    friend class ConnectionManager;
  };
  
  /**
  * \brief Cadeia de chamadas oriundas de um barramento.
  */  
  struct CallerChain {
	  /**
	  * Barramento através do qual as chamadas foram originadas.
	  */
    const char* busid() const { return _busid; }
    
    /**
    * Lista de informações de login de todas as entidades que participaram dessa cadeia de chamadas,
    * na ordem em que elas entraram na cadeia.
    */
    const idl_ac::LoginInfoSeq& callers() const { return _callers; }
    
    /**
    * Retorna a cadeia de chamadas assinada pelo barramento.
    */
    const idl_cr::SignedCallChain* signedCallChain() const { return &_signedCallChain; }

    CallerChain() : _busid(0) { _callers.length(0); }
  private:
    char* _busid;
    idl_ac::LoginInfoSeq _callers;
    idl_cr::SignedCallChain _signedCallChain;
    void signedCallChain(idl_cr::SignedCallChain p) { _signedCallChain = p; }
    friend class Connection;
    friend class openbus::interceptors::ClientInterceptor;
  };
}

#endif
