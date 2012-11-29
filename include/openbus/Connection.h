// -*- coding: iso-8859-1 -*-

/**
* API - SDK Openbus C++
* \file openbus/Connection.h
* 
*/

#ifndef TECGRAF_CONNECTION_H_
#define TECGRAF_CONNECTION_H_

#include <CORBA.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <openssl/evp.h>
#include <cstring>

#include <boost/function.hpp>

#ifndef OPENBUS_DOXYGEN
#define SECRET_SIZE 16
#endif

namespace openbus {
  class Connection;
}

#include "openbus/interceptors/ORBInitializer_impl.h"
#include "openbus/interceptors/ClientInterceptor_impl.h"
#include "openbus/interceptors/ServerInterceptor_impl.h"
#include "openbus/Connection_impl.h"
#include "openbus/util/OpenSSL.h"
#include "openbus/OpenBusContext.h"

/**
* \brief openbus
*/
namespace openbus {
  
/* exceptions */
struct BusChanged : public std::exception { 
  const char *what() const throw() { return "openbus::BusChanged"; }
};

struct AlreadyLoggedIn : public std::exception { 
  const char *what() const throw() { return "openbus::AlreadyLoggedIn"; } 
};

struct InvalidBusAddress : public std::exception { 
  const char *what() const throw() { return "openbus::InvalidBusAddress"; }
};

struct InvalidPrivateKey : public std::exception { 
  const char *what() const throw() { return "openbus::InvalidPrivateKey";} 
};

struct InvalidLoginProcess : public std::exception { 
  const char *what() const throw() { return "openbus::InvalidLoginProcess"; }
};

struct InvalidPropertyValue : public std::exception {
  InvalidPropertyValue(std::string p, std::string v) : property(p), value(v) { }
  ~InvalidPropertyValue() throw() { }
  const char *what() const throw() { return "openbus::InvalidBusAddress"; }
  std::string property;
  std::string value;
};
/**/

class Connection;


/**
 * \brief Objeto que representa uma forma de acesso a um barramento.
 *
 * Uma conex�o representa uma forma de acesso a um barramento. Basicamente, uma conex�o � usada para
 * representar uma identidade de acesso a um barramento.  � poss�vel uma aplica��o assumir m�ltiplas
 * identidades ao acessar um ou mais barramentos criando m�ltiplas conex�es para esses barramentos.
 * 
 * Para que as conex�es possam ser efetivamente utilizadas elas precisam estar autenticadas no
 * barramento, que pode ser visto como um identificador de acesso. Cada login possui um
 * identificador �nico e � autenticado em nome de uma entidade, que pode representar um sistema
 * computacional ou mesmo uma pessoa. A fun��o da entidade � atribuir a responsabilidade as chamadas
 * feitas com aquele login.
 * 
 * � importante notar que a conex�o define uma forma de acesso, mas n�o � usada diretamente pela
 * aplica��o ao realizar ou receber chamadas, pois as chamadas ocorrem usando proxies e servants de
 * um ORB. As conex��es que s�o efetivamente usadas nas chamadas do ORB s�o definidas atrav�s do
 * OpenBusContext associado ao ORB.
 */
class Connection {
public:
  /**
   * \brief Callback de login inv�lido.
   * 
   * Tipo que representa um objeto fun��o ('function object') a ser chamado quando uma notifica��o
   * de login inv�lido � recebida. Caso alguma exce��o ocorra durante a execu��o do m�todo e n�o
   * seja tratada, o erro ser� capturado pelo interceptador e registrado no log.
   * 
   * O tipo InvalidLoginCallback_t � um typedef de boost::function. Para documenta��o dessa
   * biblioteca acesse http://www.boost.org/doc/libs/1_47_0/doc/html/function.html
   *
   * \param conn Conex�o que recebeu a notifica��o de login inv�lido.
   * \param login Informa��es do login que se tornou inv�lido.
   */
  typedef boost::function<void (Connection&, idl_ac::LoginInfo)> InvalidLoginCallback_t;
  
  /**
  * Efetua login no barramento como uma entidade usando autentica��o por senha.
  * 
  * @param[in] entity Identificador da entidade a ser autenticada.
  * @param[in] password Senha de autentica��o no barramento da entidade.
  * 
  * @throw AlreadyLoggedIn A conex�o j� est� logada.
  * @throw BusChanged O identificador do barramento mudou. Uma nova conex�o deve ser criada.
  * @throw tecgraf::openbus::core::v2_0::services::access_control::AccessDenied
  *        Senha fornecida para autentica��o da entidade n�o foi validada pelo barramento.
  * @throw tecgraf::openbus::core::v2_0::services::ServiceFailure 
  *        Ocorreu uma falha interna nos servi�os do barramento que impediu a autentica��o da 
  *        conex�o.
  * @throw CORBA::Exception
  */
  void loginByPassword(const char *entity, const char *password);
  
 /**
  * \brief Efetua login de uma entidade usando autentica��o por certificado.
  * 
  * A autentica��o por certificado � validada usando um certificado de login registrado pelo
  * adminsitrador do barramento.
  * 
  * @param[in] entity Identificador da entidade a ser conectada.
  * @param[in] privKey Chave privada da entidade utilizada na autentica��o.
  * 
  * @throw InvalidPrivateKey A chave privada fornecida n�o � v�lida.
  * @throw AlreadyLoggedIn A conex�o j� est� autenticada.
  * @throw BusChanged O identificador do barramento mudou. Uma nova conex�o deve ser criada.
  * @throw tecgraf::openbus::core::v2_0::services::access_control::AccessDenied 
  *        A chave privada fornecida n�o corresponde ao certificado da entidade registrado no
  *        barramento indicado.
  * @throw MissingCertificate N�o h� certificado para essa entidade registrado no barramento 
  *        indicado.
  * @throw tecgraf::openbus::core::v2_0::services::ServiceFailure 
  *        Ocorreu uma falha interna nos servi�os do barramento que impediu a autentica��o da 
  *        conex�o.
  * @throw CORBA::Exception
  */
  void loginByCertificate(const char *entity, const idl::OctetSeq &privKey);
  
  /**
  * \brief Inicia o processo de login por autentica��o compartilhada.
  * 
  * A autentica��o compartilhada permite criar um novo login compartilhando a mesma autentica��o do
  * login atual da conex�o. Portanto essa opera��o s� pode ser chamada enquanto a conex�o estiver
  * autenticada, caso contr�rio a exce��o de sistema CORBA::NO_PERMISSION{NoLogin} � lan�ada. As
  * informa��es fornecidas por essa opera��o devem ser passadas para a opera��o 'loginBySharedAuth'
  * para conclus�o do processo de login por autentica��o compartilhada. Isso deve ser feito dentro
  * do tempo de lease definido pelo administrador do barramento. Caso contr�rio essas informa��es se
  * tornam inv�lidas e n�o podem mais ser utilizadas para criar um login.
  * 
  * @return Um par composto de um objeto que representa o processo de login iniciado e de um segredo
  *         a ser fornecido na conclus�o do processo de login.
  *
  * @throw tecgraf::openbus::core::v2_0::services::ServiceFailure 
  *        Ocorreu uma falha interna nos servi�os do barramento que impediu o estabelecimento da
  *        conex�o.
  * @throw CORBA::Exception
  */
  std::pair <idl_ac::LoginProcess_ptr, idl::OctetSeq> startSharedAuth();
  
  /**
  * \brief Efetua login de uma entidade usando autentica��o compartilhada.
  * 
  * A autentica��o compartilhada � feita a partir de informa��es obtidas a atrav�s da opera��o
  * 'startSharedAuth' de uma conex�o autenticada.
  * 
  * @param[in] loginProcess Objeto que represeta o processo de login iniciado.
  * @param[in] secret Segredo a ser fornecido na conclus�o do processo de login.
  * 
  * @throw InvalidLoginProcess O LoginProcess informado � inv�lido, por exemplo depois de ser 
  *        cancelado ou ter expirado.
  * @throw AlreadyLoggedIn A conex�o j� est� autenticada.
  * @throw BusChangedO identificador do barramento mudou. Uma nova conex�o deve ser criada.
  * @throw tecgraf::openbus::core::v2_0::services::access_control::AccessDenied 
  *        O segredo fornecido n�o corresponde ao esperado pelo barramento.
  * @throw tecgraf::openbus::core::v2_0::services::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu o estabelecimento 
  *        da conex�o.
  * @throw CORBA::Exception
  */
  void loginBySharedAuth(idl_ac::LoginProcess_ptr loginProcess, const idl::OctetSeq &secret);
  
 /**
  * \brief Efetua logout da conex�o, tornando o login atual inv�lido.
  * 
  * Ap�s a chamada a essa opera��o a conex�o fica desautenticada, implicando que qualquer chamada
  * realizada pelo ORB usando essa conex�o resultar� numa exce��o de sistema
  * 'CORBA::NO_PERMISSION{NoLogin}' e chamadas recebidas por esse ORB ser�o respondidas com a
  * exce��o 'CORBA::NO_PERMISSION{UnknownBus}' indicando que n�o foi poss�vel validar a chamada pois
  * a conex�o est� temporariamente desautenticada.
  * 
  * @return Verdadeiro se o processo de logout for conclu�do com �xito e falso se a conex�o j� 
  *         estiver desautenticada (login inv�lido).
  *
  * @throw CORBA::Exception
  */
  bool logout();
  	
  /**
   * \brief Estabiliza a callback a ser chamada quando o login atual se tornar inv�lido.
   *
   * Esse atributo � utilizado para definir um objeto fun��o (function object) que implementa uma
   * interface de callback a ser chamada sempre que a conex�o receber uma notifica��o de que o seu
   * login est� inv�lido. Essas notifica��es ocorrem durante chamadas realizadas ou recebidas pelo
   * barramento usando essa conex�o. Um login pode se tornar inv�lido caso o administrador
   * explicitamente o torne inv�lido ou caso a thread interna de renova��o de login n�o seja capaz
   * de renovar o lease do login a tempo. Caso esse atributo seja um InvalidLoginCallback_t
   * 'default-constructed', nenhum objeto de callback � chamado na ocorr�ncia desse evento.
   *
   * Durante a execu��o dessa callback um novo login pode ser restabelecido.  Neste caso, a chamada
   * do barramento que recebeu a notifica��o de login inv�lido � refeita usando o novo login, caso
   * contr�rio, a chamada original lan�a a exce��o de de sistema 'CORBA::NO_PERMISSION{NoLogin}'.
   * 
   * O tipo InvalidLoginCallback_t � um typedef de boost::function. Para documenta��o dessa
   * biblioteca acesse http://www.boost.org/doc/libs/1_47_0/doc/html/function.html
   */
  void onInvalidLogin(InvalidLoginCallback_t p);
  
  /**
   * \brief Retorna a callback configurada para ser chamada quando o login atual se torna inv�lido.
   */
  InvalidLoginCallback_t onInvalidLogin();
  
  /**
   * \brief Informa��es do login dessa conex�o ou 'null' se a conex�o n�o est� autenticada, ou seja,
   * n�o tem um login v�lido no barramento.
   */
  const idl_ac::LoginInfo* login();
  
  /**
   * Identificador do barramento ao qual essa conex�o se refere.
   */
  const char *busid();
  
  ~Connection();
private:
  /**
  * Connection deve ser adquirido atrav�s de: OpenBusContext::createConnection()
  */
  Connection(const std::string host, const unsigned short port, CORBA::ORB*, IOP::Codec*, 
    PortableInterceptor::SlotId slotId_joinedCallChain, 
    PortableInterceptor::SlotId slotId_signedCallChain, 
    PortableInterceptor::SlotId slotId_legacyCallChain, 
    PortableInterceptor::SlotId slotId_receiveConnection, 
    OpenBusContext*, std::vector<std::string> props);
  openssl::pkey fetchBusKey();
  void checkBusid() const;
  bool _logout(bool local);
  openssl::pkey __key() const { return _key; }
  openssl::pkey __buskey() const { return _buskey; }
  CORBA::ORB *orb() const { return _orb; }
  idl_ac::LoginRegistry_var login_registry() const { return _login_registry; }
  idl_ac::AccessControl_var access_control() const { return _access_control; }
  const idl_ac::LoginInfo *_login() const { return _loginInfo.get(); }
  const std::string _host;
  const unsigned short _port;
  CORBA::ORB *_orb;
  IOP::Codec *_codec;
  PortableInterceptor::SlotId _slotId_joinedCallChain; 
  PortableInterceptor::SlotId _slotId_signedCallChain;
  PortableInterceptor::SlotId _slotId_legacyCallChain;
  PortableInterceptor::SlotId _slotId_receiveConnection;
  std::auto_ptr<RenewLogin> _renewLogin;
  std::auto_ptr<idl_ac::LoginInfo> _loginInfo;
  InvalidLoginCallback_t _onInvalidLogin;
  Mutex _mutex;
  
  enum LegacyDelegate {
    CALLER,
    ORIGINATOR
  };
  
  enum State {
    LOGGED,
    UNLOGGED,
    INVALID
  } _state;
  
  /* Vari�veis que s�o modificadas somente no construtor. */
  OpenBusContext *_openbusContext;
  openssl::pkey _key;
  PortableInterceptor::Current_var _piCurrent;
  scs::core::IComponent_var _iComponent;
  idl_ac::AccessControl_var _access_control;
  idl_ac::LoginRegistry_var _login_registry;
  idl_or::OfferRegistry_var _offer_registry;
  std::auto_ptr<LoginCache> _loginCache;
  const char *_busid;
  openssl::pkey _buskey;
  LegacyDelegate _legacyDelegate;
  /**/
  
  idl_or::OfferRegistry_var getOfferRegistry() const
  { 
    return _offer_registry;
  }

  idl_ac::LoginRegistry_var getLoginRegistry() const
  {
    return _login_registry;
  }
  
  friend class openbus::interceptors::ServerInterceptor;
  friend class openbus::interceptors::ClientInterceptor;
  friend class openbus::OpenBusContext;
};

}

#endif
