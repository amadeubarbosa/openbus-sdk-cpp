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
 * Uma conexão representa uma forma de acesso a um barramento. Basicamente, uma conexão é usada para
 * representar uma identidade de acesso a um barramento.  É possível uma aplicação assumir múltiplas
 * identidades ao acessar um ou mais barramentos criando múltiplas conexões para esses barramentos.
 * 
 * Para que as conexões possam ser efetivamente utilizadas elas precisam estar autenticadas no
 * barramento, que pode ser visto como um identificador de acesso. Cada login possui um
 * identificador único e é autenticado em nome de uma entidade, que pode representar um sistema
 * computacional ou mesmo uma pessoa. A função da entidade é atribuir a responsabilidade as chamadas
 * feitas com aquele login.
 * 
 * É importante notar que a conexão define uma forma de acesso, mas não é usada diretamente pela
 * aplicação ao realizar ou receber chamadas, pois as chamadas ocorrem usando proxies e servants de
 * um ORB. As conexções que são efetivamente usadas nas chamadas do ORB são definidas através do
 * OpenBusContext associado ao ORB.
 */
class Connection {
public:
  /**
   * \brief Callback de login inválido.
   * 
   * Tipo que representa um objeto função ('function object') a ser chamado quando uma notificação
   * de login inválido é recebida. Caso alguma exceção ocorra durante a execução do método e não
   * seja tratada, o erro será capturado pelo interceptador e registrado no log.
   * 
   * O tipo InvalidLoginCallback_t é um typedef de boost::function. Para documentação dessa
   * biblioteca acesse http://www.boost.org/doc/libs/1_47_0/doc/html/function.html
   *
   * \param conn Conexão que recebeu a notificação de login inválido.
   * \param login Informações do login que se tornou inválido.
   */
  typedef boost::function<void (Connection&, idl_ac::LoginInfo)> InvalidLoginCallback_t;
  
  /**
  * Efetua login no barramento como uma entidade usando autenticação por senha.
  * 
  * @param[in] entity Identificador da entidade a ser autenticada.
  * @param[in] password Senha de autenticação no barramento da entidade.
  * 
  * @throw AlreadyLoggedIn A conexão já está logada.
  * @throw BusChanged O identificador do barramento mudou. Uma nova conexão deve ser criada.
  * @throw tecgraf::openbus::core::v2_0::services::access_control::AccessDenied
  *        Senha fornecida para autenticação da entidade não foi validada pelo barramento.
  * @throw tecgraf::openbus::core::v2_0::services::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu a autenticação da 
  *        conexão.
  * @throw CORBA::Exception
  */
  void loginByPassword(const char *entity, const char *password);
  
 /**
  * \brief Efetua login de uma entidade usando autenticação por certificado.
  * 
  * A autenticação por certificado é validada usando um certificado de login registrado pelo
  * adminsitrador do barramento.
  * 
  * @param[in] entity Identificador da entidade a ser conectada.
  * @param[in] privKey Chave privada da entidade utilizada na autenticação.
  * 
  * @throw InvalidPrivateKey A chave privada fornecida não é válida.
  * @throw AlreadyLoggedIn A conexão já está autenticada.
  * @throw BusChanged O identificador do barramento mudou. Uma nova conexão deve ser criada.
  * @throw tecgraf::openbus::core::v2_0::services::access_control::AccessDenied 
  *        A chave privada fornecida não corresponde ao certificado da entidade registrado no
  *        barramento indicado.
  * @throw MissingCertificate Não há certificado para essa entidade registrado no barramento 
  *        indicado.
  * @throw tecgraf::openbus::core::v2_0::services::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu a autenticação da 
  *        conexão.
  * @throw CORBA::Exception
  */
  void loginByCertificate(const char *entity, const idl::OctetSeq &privKey);
  
  /**
  * \brief Inicia o processo de login por autenticação compartilhada.
  * 
  * A autenticação compartilhada permite criar um novo login compartilhando a mesma autenticação do
  * login atual da conexão. Portanto essa operação só pode ser chamada enquanto a conexão estiver
  * autenticada, caso contrário a exceção de sistema CORBA::NO_PERMISSION{NoLogin} é lançada. As
  * informações fornecidas por essa operação devem ser passadas para a operação 'loginBySharedAuth'
  * para conclusão do processo de login por autenticação compartilhada. Isso deve ser feito dentro
  * do tempo de lease definido pelo administrador do barramento. Caso contrário essas informações se
  * tornam inválidas e não podem mais ser utilizadas para criar um login.
  * 
  * @return Um par composto de um objeto que representa o processo de login iniciado e de um segredo
  *         a ser fornecido na conclusão do processo de login.
  *
  * @throw tecgraf::openbus::core::v2_0::services::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu o estabelecimento da
  *        conexão.
  * @throw CORBA::Exception
  */
  std::pair <idl_ac::LoginProcess_ptr, idl::OctetSeq> startSharedAuth();
  
  /**
  * \brief Efetua login de uma entidade usando autenticação compartilhada.
  * 
  * A autenticação compartilhada é feita a partir de informações obtidas a através da operação
  * 'startSharedAuth' de uma conexão autenticada.
  * 
  * @param[in] loginProcess Objeto que represeta o processo de login iniciado.
  * @param[in] secret Segredo a ser fornecido na conclusão do processo de login.
  * 
  * @throw InvalidLoginProcess O LoginProcess informado é inválido, por exemplo depois de ser 
  *        cancelado ou ter expirado.
  * @throw AlreadyLoggedIn A conexão já está autenticada.
  * @throw BusChangedO identificador do barramento mudou. Uma nova conexão deve ser criada.
  * @throw tecgraf::openbus::core::v2_0::services::access_control::AccessDenied 
  *        O segredo fornecido não corresponde ao esperado pelo barramento.
  * @throw tecgraf::openbus::core::v2_0::services::ServiceFailure 
  *        Ocorreu uma falha interna nos serviÃ§os do barramento que impediu o estabelecimento 
  *        da conexão.
  * @throw CORBA::Exception
  */
  void loginBySharedAuth(idl_ac::LoginProcess_ptr loginProcess, const idl::OctetSeq &secret);
  
 /**
  * \brief Efetua logout da conexão, tornando o login atual inválido.
  * 
  * Após a chamada a essa operação a conexão fica desautenticada, implicando que qualquer chamada
  * realizada pelo ORB usando essa conexão resultará numa exceção de sistema
  * 'CORBA::NO_PERMISSION{NoLogin}' e chamadas recebidas por esse ORB serão respondidas com a
  * exceção 'CORBA::NO_PERMISSION{UnknownBus}' indicando que não foi possível validar a chamada pois
  * a conexão está temporariamente desautenticada.
  * 
  * @return Verdadeiro se o processo de logout for concluído com êxito e falso se a conexão já 
  *         estiver desautenticada (login inválido).
  *
  * @throw CORBA::Exception
  */
  bool logout();
  	
  /**
   * \brief Estabiliza a callback a ser chamada quando o login atual se tornar inválido.
   *
   * Esse atributo é utilizado para definir um objeto função (function object) que implementa uma
   * interface de callback a ser chamada sempre que a conexão receber uma notificação de que o seu
   * login está inválido. Essas notificações ocorrem durante chamadas realizadas ou recebidas pelo
   * barramento usando essa conexão. Um login pode se tornar inválido caso o administrador
   * explicitamente o torne inválido ou caso a thread interna de renovação de login não seja capaz
   * de renovar o lease do login a tempo. Caso esse atributo seja um InvalidLoginCallback_t
   * 'default-constructed', nenhum objeto de callback é chamado na ocorrência desse evento.
   *
   * Durante a execução dessa callback um novo login pode ser restabelecido.  Neste caso, a chamada
   * do barramento que recebeu a notificação de login inválido é refeita usando o novo login, caso
   * contrário, a chamada original lança a exceção de de sistema 'CORBA::NO_PERMISSION{NoLogin}'.
   * 
   * O tipo InvalidLoginCallback_t é um typedef de boost::function. Para documentação dessa
   * biblioteca acesse http://www.boost.org/doc/libs/1_47_0/doc/html/function.html
   */
  void onInvalidLogin(InvalidLoginCallback_t p);
  
  /**
   * \brief Retorna a callback configurada para ser chamada quando o login atual se torna inválido.
   */
  InvalidLoginCallback_t onInvalidLogin();
  
  /**
   * \brief Informações do login dessa conexão ou 'null' se a conexão não está autenticada, ou seja,
   * não tem um login válido no barramento.
   */
  const idl_ac::LoginInfo* login();
  
  /**
   * Identificador do barramento ao qual essa conexão se refere.
   */
  const char *busid();
  
  ~Connection();
private:
  /**
  * Connection deve ser adquirido através de: OpenBusContext::createConnection()
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
  
  /* Variáveis que são modificadas somente no construtor. */
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
