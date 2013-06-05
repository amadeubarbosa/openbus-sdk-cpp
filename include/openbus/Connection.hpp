// -*- coding: iso-8859-1 -*-

/**
* API - SDK Openbus C++
* \file openbus/Connection.hpp
* 
*/

#ifndef TECGRAF_SDK_OPENBUS_CONNECTION_H_
#define TECGRAF_SDK_OPENBUS_CONNECTION_H_

#include "openbus/decl.hpp"
#include "stubs/scs.h"
#include "stubs/access_control.h"
#include "stubs/offer_registry.h"
#include "openbus/interceptors/ClientInterceptor_impl.hpp"
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/crypto/PrivateKey.hpp"
#include "openbus/crypto/PublicKey.hpp"

#include <CORBA.h>
#include <boost/function.hpp>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <cstring>

/**
* \brief openbus
*/
namespace openbus 
{
namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;

class OpenBusContext;
#ifndef OPENBUS_SDK_MULTITHREAD
class RenewLogin;
#endif
  
struct OPENBUS_SDK_DECL BusChanged : public std::exception 
{ 
  const char *what() const throw()
  { 
    return "openbus::BusChanged";
  }
};

struct OPENBUS_SDK_DECL AlreadyLoggedIn : public std::exception 
{ 
  const char *what() const throw()
  { 
    return "openbus::AlreadyLoggedIn"; 
  }
};

struct OPENBUS_SDK_DECL InvalidLoginProcess : public std::exception 
{
  const char *what() const throw()
  { 
    return "openbus::InvalidLoginProcess"; 
  }
};

struct OPENBUS_SDK_DECL InvalidPropertyValue : public std::exception 
{
  InvalidPropertyValue(const std::string &p, const std::string &v) throw()
    : property(p), value(v) 
  { 
  }

  ~InvalidPropertyValue() throw()
  { 
  }

  const char *what() const throw() 
  { 
    return "openbus::InvalidPropertyValue"; 
  }

  const std::string property;
  const std::string value;
};

/**
 * \brief Conex�o para acesso identificado a um barramento.
 *
 * Uma conex�o � usada para realizar acessos identificados a um barramento.
 * Denominamos esses acessos identificados ao barramento de login. Cada login
 * possui um identificador �nico e est� sempre associado ao nome de uma
 * entidade que � autenticada no momento do estabelecimento do login.
 * H� basicamente tr�s formas de autentica��o de entidade dispon�veis:
 * - Por Senha: veja a opera��o 'loginByPassword'
 * - Por Certificado de login: veja a opera��o 'loginByCertificate'
 * - Por Autentica��o compartilhada: veja a opera��o 'loginBySharedAuth'
 *
 * A entidade associada ao login � respons�vel por todas as chamadas feitas
 * atrav�s daquela conex�o e essa entidade deve ser levada em considera��o
 * pelos servi�os ofertados no barramento para decidir aceitar ou recusar
 * chamadas.
 *
 * � poss�vel uma aplica��o assumir m�ltiplas identidades ao acessar um ou mais
 * barramentos criando m�ltiplas conex�es para esses barramentos.
 * 
 * � importante notar que a conex�o n�o � usada diretamente pela aplica��o ao
 * realizar ou receber chamadas, pois as chamadas ocorrem usando proxies e
 * servants de um ORB. As conex�es que s�o efetivamente usadas nas chamadas do
 * ORB s�o definidas atrav�s do OpenBusContext associado ao ORB.
 */
class OPENBUS_SDK_DECL Connection 
{
public:
  typedef std::vector<std::pair<std::string, std::string> > 
    ConnectionProperties;
  /**
   * \brief Callback de login inv�lido.
   * 
   * Tipo que representa um objeto fun��o ('function object') a ser chamado
   * quando uma notifica��o de login inv�lido � recebida. Caso alguma exce��o
   * ocorra durante a execu��o do m�todo e n�o seja tratada, o erro ser�
   * capturado pelo interceptador e registrado no log.
   * 
   * O tipo InvalidLoginCallback_t � um typedef de boost::function. Para
   * documenta��o dessa biblioteca acesse
   * http://www.boost.org/doc/libs/1_47_0/doc/html/function.html
   *
   * \param conn Conex�o que recebeu a notifica��o de login inv�lido.
   * \param login Informa��es do login que se tornou inv�lido.
   */
  typedef boost::function<void (Connection &, idl_ac::LoginInfo)> 
    InvalidLoginCallback_t;
  
  /**
  * Efetua login no barramento como uma entidade usando autentica��o por senha.
  * 
  * A autentica��o por senha � validada usando um dos validadores de senha
  * definidos pelo adminsitrador do barramento.
  *
  * @param[in] entity Identificador da entidade a ser autenticada.
  * @param[in] password Senha de autentica��o no barramento da entidade.
  * 
  * @throw AlreadyLoggedIn A conex�o j� est� logada.
  * @throw idl_ac::AccessDenied
  *        Senha fornecida para autentica��o da entidade n�o foi validada pelo 
  *        barramento.
  * @throw idl::ServiceFailure 
  *        Ocorreu uma falha interna nos servi�os do barramento que impediu a 
  *        autentica��o da conex�o.
  * @throw CORBA::Exception
  */
  void loginByPassword(const std::string &entity, const std::string &password);
  
 /**
  * \brief Efetua login de uma entidade usando autentica��o por certificado.
  * 
  * A autentica��o por certificado � validada usando um certificado de login
  * registrado pelo adminsitrador do barramento.
  * 
  * @param[in] entity Identificador da entidade a ser conectada.
  * @param[in] privKey Chave privada da entidade utilizada na autentica��o.
  * 
  * @throw AlreadyLoggedIn A conex�o j� est� autenticada.
  * @throw idl_ac::AccessDenied 
  *        A chave privada fornecida n�o corresponde ao certificado da entidade 
  *        registrado no barramento indicado.
  * @throw idl_ac::MissingCertificate 
  *        N�o h� certificado para essa entidade registrado no barramento
  *        indicado.
  * @throw idl::ServiceFailure 
  *        Ocorreu uma falha interna nos servi�os do barramento que impediu a 
  *        autentica��o da conex�o.
  * @throw CORBA::Exception
  */
  void loginByCertificate(const std::string &entity, const PrivateKey &);
  
  /**
  * \brief Inicia o processo de login por autentica��o compartilhada.
  * 
  * A autentica��o compartilhada permite criar um novo login compartilhando a
  * mesma autentica��o do login atual da conex�o. Portanto essa opera��o s� pode
  * ser chamada enquanto a conex�o estiver autenticada, caso contr�rio a exce��o
  * de sistema CORBA::NO_PERMISSION{NoLogin} � lan�ada. As informa��es
  * fornecidas por essa opera��o devem ser passadas para a opera��o
  * 'loginBySharedAuth' para conclus�o do processo de login por autentica��o
  * compartilhada. Isso deve ser feito dentro do tempo de lease definido pelo
  * administrador do barramento. Caso contr�rio essas informa��es se tornam
  * inv�lidas e n�o podem mais ser utilizadas para criar um login.
  * 
  * @return Um par composto de um objeto que representa o processo de login 
  *         iniciado e de um segredo a ser fornecido na conclus�o do processo 
  *         de login.
  *
  * @throw idl::ServiceFailure 
  *        Ocorreu uma falha interna nos servi�os do barramento que impediu 
  *        o estabelecimento da conex�o.
  * @throw CORBA::Exception
  */
  std::pair <idl_ac::LoginProcess_ptr, idl::OctetSeq> startSharedAuth();
  
  /**
  * \brief Efetua login de uma entidade usando autentica��o compartilhada.
  * 
  * A autentica��o compartilhada � feita a partir de informa��es obtidas a 
  * atrav�s da opera��o 'startSharedAuth' de uma conex�o autenticada.
  * 
  * @param[in] loginProcess Objeto que represeta o processo de login iniciado.
  * @param[in] secret Segredo a ser fornecido na conclus�o do processo de login.
  * 
  * @throw InvalidLoginProcess O LoginProcess informado � inv�lido, por exemplo 
  *        depois de ser cancelado ou ter expirado.
  * @throw AlreadyLoggedIn A conex�o j� est� autenticada.
  * @throw idl_ac::AccessDenied 
  *        O segredo fornecido n�o corresponde ao esperado pelo barramento.
  * @throw idl::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu o 
  *        estabelecimento da conex�o.
  * @throw CORBA::Exception
  */
  void loginBySharedAuth(idl_ac::LoginProcess_ptr loginProcess, 
                         const idl::OctetSeq &secret);
  
 /**
  * \brief Efetua logout da conex�o, tornando o login atual inv�lido.
  * 
  * Ap�s a chamada a essa opera��o a conex�o fica desautenticada, implicando que
  * qualquer chamada realizada pelo ORB usando essa conex�o resultar� numa
  * exce��o de sistema 'CORBA::NO_PERMISSION{NoLogin}' e chamadas recebidas por
  * esse ORB ser�o respondidas com a exce��o 'CORBA::NO_PERMISSION{UnknownBus}'
  * indicando que n�o foi poss�vel validar a chamada pois a conex�o est�
  * temporariamente desautenticada.
  * 
  * @return Verdadeiro se o processo de logout for conclu�do com �xito e falso
  *         se a conex�o j� estiver desautenticada (login inv�lido) ou se houver 
  *         uma falha durante o processo remoto do logout.
  *
  */
  bool logout();
  	
  /**
   * \brief Callback a ser chamada quando o login atual se tornar inv�lido.
   *
   * Esse atributo � utilizado para definir um objeto fun��o (function object)
   * que implementa uma interface de callback a ser chamada sempre que a conex�o
   * receber uma notifica��o de que o seu login est� inv�lido. Essas
   * notifica��es ocorrem durante chamadas realizadas ou recebidas pelo
   * barramento usando essa conex�o. Um login pode se tornar inv�lido caso o
   * administrador explicitamente o torne inv�lido ou caso a thread interna de
   * renova��o de login n�o seja capaz de renovar o lease do login a tempo. Caso
   * esse atributo seja um InvalidLoginCallback_t 'default-constructed', nenhum
   * objeto de callback � chamado na ocorr�ncia desse evento.
   *
   * Durante a execu��o dessa callback um novo login pode ser restabelecido.
   * Neste caso, a chamada do barramento que recebeu a notifica��o de login
   * inv�lido � refeita usando o novo login, caso contr�rio, a chamada original
   * lan�a a exce��o de de sistema 'CORBA::NO_PERMISSION{NoLogin}'.
   * 
   * O tipo InvalidLoginCallback_t � um typedef de boost::function. Para
   * documenta��o dessa biblioteca acesse
   * http://www.boost.org/doc/libs/1_47_0/doc/html/function.html
   */
  void onInvalidLogin(InvalidLoginCallback_t p);
  
  /**
   * \brief Retorna a callback configurada para ser chamada quando o login atual
   * se torna inv�lido.
   */
  InvalidLoginCallback_t onInvalidLogin() const;
  
  /**
   * \brief Informa��es do login dessa conex�o ou 'null' se a conex�o n�o est�
   * autenticada, ou seja, n�o tem um login v�lido no barramento.
   */
  const idl_ac::LoginInfo *login() const;
  
  /**
   * Identificador do barramento ao qual essa conex�o se refere.
   */
  const std::string busid() const;
  ~Connection();  
private:
  /**
  * Connection deve ser adquirido atraves de: OpenBusContext::createConnection()
  */
  Connection(const std::string host, const unsigned short port, CORBA::ORB_ptr, 
             IOP::Codec *, PortableInterceptor::SlotId slotId_joinedCallChain, 
             PortableInterceptor::SlotId slotId_signedCallChain, 
             PortableInterceptor::SlotId slotId_legacyCallChain, 
             PortableInterceptor::SlotId slotId_receiveConnection, 
             OpenBusContext &, 
             const ConnectionProperties &props);

  Connection(const Connection &);
  Connection &operator=(const Connection &);

#ifdef OPENBUS_SDK_MULTITHREAD  
  static void renewLogin(Connection &conn, idl_ac::AccessControl_ptr acs, 
                         OpenBusContext &ctx, idl_ac::ValidityTime t);
#endif
  void login(idl_ac::LoginInfo &loginInfo, 
             idl_ac::ValidityTime validityTime);

  void checkBusid() const;
  bool _logout(bool local = true);
  CORBA::ORB_ptr orb() const 
  { 
    return _orb; 
  }

  idl_ac::LoginRegistry_var login_registry() const 
  { 
    return _login_registry; 
  }

  idl_ac::AccessControl_var access_control() const 
  { 
    return _access_control; 
  }

  const idl_ac::LoginInfo *_login() const 
  { 
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);;
#endif
    return _loginInfo.get(); 
  }

  idl_or::OfferRegistry_var getOfferRegistry() const
  { 
    return _offer_registry;
  }

  idl_ac::LoginRegistry_var getLoginRegistry() const
  {
    return _login_registry;
  }

  const std::string _host;
  const unsigned short _port;
  CORBA::ORB *_orb;
  IOP::Codec *_codec;
  PortableInterceptor::SlotId _slotId_joinedCallChain; 
  PortableInterceptor::SlotId _slotId_signedCallChain;
  PortableInterceptor::SlotId _slotId_legacyCallChain;
  PortableInterceptor::SlotId _slotId_receiveConnection;
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread _renewLogin;
  mutable boost::mutex _mutex;
#else
  std::auto_ptr<RenewLogin> _renewLogin;
#endif
  std::auto_ptr<idl_ac::LoginInfo> _loginInfo;
  InvalidLoginCallback_t _onInvalidLogin;
  
  enum LegacyDelegate 
  {
    CALLER,
    ORIGINATOR
  };
  
  enum State 
  {
    LOGGED,
    UNLOGGED,
    INVALID
  } _state;
  
  /* Variaveis que sao modificadas somente no construtor. */
  OpenBusContext &_openbusContext;
  PrivateKey _key;
  PortableInterceptor::Current_var _piCurrent;
  scs::core::IComponent_var _iComponent;
  idl_ac::AccessControl_var _access_control;
  idl_ac::LoginRegistry_var _login_registry;
  idl_or::OfferRegistry_var _offer_registry;
  std::auto_ptr<LoginCache> _loginCache;
  std::string _busid;
  std::auto_ptr<PublicKey> _buskey;
  LegacyDelegate _legacyDelegate;
  bool _legacyEnabled;
  /**/
    
  friend class openbus::interceptors::ServerInterceptor;
  friend class openbus::interceptors::ClientInterceptor;
  friend class openbus::OpenBusContext;
};
}

#endif
