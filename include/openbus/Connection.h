/**
* API - SDK Openbus C++
* \file openbus/ConnectionManager.h
*/

#ifndef TECGRAF_CONNECTION_H_
#define TECGRAF_CONNECTION_H_

#include <CORBA.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <openssl/evp.h>

namespace openbus {
  class Connection;
  struct CallerChain;
}

#include "openbus/interceptors/ORBInitializer_impl.h"
#include "openbus/interceptors/ClientInterceptor_impl.h"
#include "openbus/interceptors/ServerInterceptor_impl.h"
#include "openbus/Connection_impl.h"
#include "openbus/ConnectionManager.h"

/**
* \brief openbus
*/
namespace openbus {
  
  /* exceptions */
  struct AccessDenied  { const char* name() const { return "openbus::AccessDenied"; } };
  struct AlreadyLoggedIn { const char* name() const { return "openbus::AlreadyLoggedIn"; } };
  struct CorruptedPrivateKey{ const char* name() const { return "openbus::CorruptedPrivateKey";}};
  struct WrongPrivateKey {const char* name() const { return "openbus::WrongPrivateKey"; } };
  struct WrongSecret { const char* name() const { return "openbus::WrongSecret"; } };
  struct InvalidLoginProcess{ const char* name() const { return "openbus::InvalidLoginProcess";}};
  /**/
  
  struct CallerChain;

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
    typedef bool (*InvalidLoginCallback_ptr) (Connection&, idl_ac::LoginInfo, const char* busid);
  
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
    void loginByPassword(const char* entity, const char* password);

	  /**
	  * Efetua login no barramento como uma entidade usando autenticação por certificado.
	  * 
	  * @param[in] entity Identificador da entidade a ser conectada.
	  * @param[in] privKey Chave privada da entidade utilizada na autenticação.
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
    void loginByCertificate(const char* entity, const idl::OctetSeq& privKey);

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
    std::pair <idl_ac::LoginProcess*, const unsigned char*> startSharedAuth();
      
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
    void loginBySharedAuth(idl_ac::LoginProcess* loginProcess, const unsigned char* secret);
          
	  /**
	  * Efetua logout no barramento. Se a sua conexão for uma conexão de despacho, remove essa conexão
	  * como despachante do ConnectionManager.
	  *  
	  * @return true se o processo de logout for concluído com êxito e false se a conexão já estiver 
	  * deslogada (login inválido).
	  *
	  * @throw CORBA::Exception
	  */
    bool logout();
	  
	  /**
	  * Caso a thread corrente seja a thread de execução de uma chamada remota oriunda do barramento 
	  * dessa conexão, essa operação devolve um objeto que representa a cadeia de chamadas do 
	  * barramento que esta chamada faz parte. Caso contrário devolve zero.
	  * 
	  * @return Cadeia da chamada em execução.
	  *
	  * @throw CORBA::Exception
	  */
    CallerChain* getCallerChain();
    
	  /**
	  * Associa uma cadeia de chamadas do barramento a thread corrente, de forma que todas as chamadas
	  * remotas seguintes dessa thread através dessa conexão sejam feitas como parte dessa cadeia de 
	  * chamadas.
	  *
	  * @param[in] chain Cadeia de chamadas a ser associada.
	  *
	  * @throw CORBA::NO_PERMISSION { minor = NoLoginCode }
	  * @throw CORBA::NO_PERMISSION { minor = InvalidChainCode }
	  * @throw CORBA::Exception
	  */
	  void joinChain(CallerChain* chain);

    /**
    * Remove a associação da cadeia de chamadas com a thread corrente, fazendo com que todas as 
    * chamadas seguintes da thread corrente feitas através dessa conexão	deixem de fazer parte da 
    * cadeia de chamadas associada previamente. Ou seja, todas as chamadas passam a iniciar novas 
    * cadeias de chamada.
    *
	  * @throw CORBA::Exception
    */
    void exitChain();
    
	  /**
	  * Devolve um objeto que representa a cadeia de chamadas associada a thread atual nessa conexão. 
	  * A cadeia de chamadas informada foi associada previamente pela operação 'joinChain'. Caso a 
	  * thread corrente não tenha nenhuma cadeia associada, essa operação devolve zero.
	  * 
	  * @return Cadeia de chamadas associada.
    *
	  * @throw CORBA::Exception
	  */
    CallerChain* getJoinedChain();
    
	  /*
	  * Define a callback a ser chamada sempre que o login se torna inválido.
	  * @param[in] p Ponteiro para uma função de callback que implementa a interface de callback a ser
	  *            chamada ou zero caso nenhum objeto deva ser chamado na ocorrência desse evento.
	  */
    void onInvalidLogin(InvalidLoginCallback_ptr p);
    
	  /*
	  * Retorna um ponteiro para uma função de callback a ser chamada sempre que o login se torna 
	  * inválido.
	  */
    InvalidLoginCallback_ptr onInvalidLogin();
       
  	/** 
  	* Informações sobre o login da entidade que autenticou essa conexão. 
  	*/
    const idl_ac::LoginInfo login() const { return *(_loginInfo.get()); }
    
  	/** 
  	* Barramento ao qual essa conexão se refere. 
  	*/
    const char* busid() const { return CORBA::string_dup(_busid); }
    
    /**
    * Obtenção do serviço de ofertas.
    */
    idl_or::OfferRegistry_var offers() const { return _offer_registry; }
    
    ~Connection();
  private:
    /**
    * Connection deve ser adquirido através de:
    *   ConnectionManager::createConnection()
    */
    Connection(const std::string host, const unsigned short port, CORBA::ORB*, IOP::Codec*, 
      PortableInterceptor::SlotId slotId_joinedCallChain, 
      PortableInterceptor::SlotId slotId_signedCallChain, 
      PortableInterceptor::SlotId slotId_legacyCallChain, 
      ConnectionManager*);
    EVP_PKEY* fetchBusKey();
    bool _logout(bool local);
    EVP_PKEY* __key() const { return _key; }
    EVP_PKEY* __buskey() const { return _buskey; }
    CORBA::ORB* orb() const { return _orb; }
    idl_ac::LoginRegistry_var login_registry() const { return _login_registry; }
    idl_ac::AccessControl_var access_control() const { return _access_control; }
    idl_ac::LoginInfo* _login() const { return _loginInfo.get(); }
    const std::string _host;
    const unsigned short _port;
    CORBA::ORB* _orb;
    IOP::Codec* _codec;
    PortableInterceptor::SlotId _slotId_joinedCallChain; 
    PortableInterceptor::SlotId _slotId_signedCallChain;
    PortableInterceptor::SlotId _slotId_legacyCallChain;
    
    /* Variáveis que são modificadas somente no construtor. */
    ConnectionManager* _manager;
    EVP_PKEY* _key;
    PortableInterceptor::Current_var _piCurrent;
    scs::core::IComponent_var _iComponent;
    idl_ac::AccessControl_var _access_control;
    idl_ac::LoginRegistry_var _login_registry;
    idl_or::OfferRegistry_var _offer_registry;
    std::auto_ptr<LoginCache> _loginCache;
    /**/
    
    std::auto_ptr<RenewLogin> _renewLogin;
    std::auto_ptr<idl_ac::LoginInfo> _loginInfo;
    const char* _busid;
    EVP_PKEY* _buskey;
    InvalidLoginCallback_ptr _onInvalidLogin;
    MICOMT::Mutex _mutex;

    friend class openbus::interceptors::ServerInterceptor;
    friend class openbus::interceptors::ClientInterceptor;
    friend class openbus::ConnectionManager;
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
	  * Lista de informações de login de todas as entidades que realizaram chamadas
	  * que originaram a cadeia de chamadas da qual essa chamada está inclusa.
 	  * Quando essa lista é vazia isso indica que a chamada não está inclusa numa 
	  * cadeia de chamadas.
 	  */
    const idl_ac::LoginInfoSeq& originators() const { return _originators; }
    
	  /**
	  * Informação de login da entidade que iniciou a chamada
	  */
    const idl_ac::LoginInfo& caller() const { return _caller; }
  private:
    CallerChain(const char* busid, const idl_ac::LoginInfoSeq& b, const idl_ac::LoginInfo& c, 
      const idl_cr::SignedCallChain& d) 
      : _busid(busid), _originators(b), _caller(c), _signedCallChain(d) { }

    CallerChain(const char* busid, const idl_ac::LoginInfoSeq& b, const idl_ac::LoginInfo& c) 
      : _busid(busid), _originators(b), _caller(c) { }

    const char* _busid;
    idl_ac::LoginInfoSeq _originators;
    idl_ac::LoginInfo _caller;
    idl_cr::SignedCallChain _signedCallChain;
    const idl_cr::SignedCallChain* signedCallChain() const { return &_signedCallChain; }
    void signedCallChain(idl_cr::SignedCallChain p) { _signedCallChain = p; }
    friend class Connection;
    friend class openbus::interceptors::ClientInterceptor;
  };
}

#endif
