// -*- coding: iso-8859-1-unix -*-

/**
* API do OpenBus SDK C++
* \file openbus/bconnection.hpp
* 
*/

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_CONNECTION_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_CONNECTION_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "scsC.h"
#pragma clang diagnostic pop
#include "openbus/idl.hpp"
#include "openbus/detail/decl.hpp"
#include "openbus/detail/interceptors/orb_initializer.hpp"
#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_LRUCACHE_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_DETAIL_LRUCACHE_HPP
#include "openbus/detail/lru_cache.hpp"
#endif
#include "openbus/orb_initializer.hpp"
#include "openbus/detail/openssl/private_key.hpp"

#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
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
  class OpenBusContext;
  class LoginCache;
  class RenewLogin;
  class PublicKey;

  namespace interceptors
  {
    struct orb_info;
    struct ServerInterceptor;
    struct ClientInterceptor;
  }
}
 
namespace openbus 
{ 
struct OPENBUS_SDK_DECL WrongBus : public std::exception 
{ 
  const char *what() const throw()
  { 
    return "openbus::WrongBus";
  }
};

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

struct OPENBUS_SDK_DECL InvalidBusAddress : public std::exception 
{
  const char *what() const throw()
  { 
    return "openbus::InvalidBusAddress"; 
  }
};

class Connection;
  
/**
 * \brief Segredo para compartilhamento de autentica��o.
 *
 * Objeto que representa uma tentativa de compartilhamento de
 * autentica��o atrav�s do compartilhamento de um segredo, que pode
 * ser utilizado para realizar uma autentica��o junto ao barramento em
 * nome da mesma entidade que gerou e compartilhou o segredo.
 *
 * Cada segredo de autentica��o compartilhada pertence a um �nico barramento e
 * s� pode ser utilizado em uma �nica autentica��o.
 *
 */
class OPENBUS_SDK_DECL SharedAuthSecret
{
public:
   /**
    * \brief Cancela o segredo caso esse ainda esteja ativo, de forma
    * que ele n�o poder� ser mais utilizado.  
    *
    */
   void cancel();
  
  /**
   * \brief Retorna o identificador do barramento em que o segredo
   * pode ser utilizado.
   */
  std::string busid() const
  {
    return busid_;
  }
private:
  SharedAuthSecret();
  SharedAuthSecret(
    const std::string &bus_id,
    idl::access::LoginProcess_var,
    idl::legacy::access::LoginProcess_var,
    const idl::core::OctetSeq &secret,
    interceptors::ORBInitializer *);  

  std::string busid_;
  idl::access::LoginProcess_var login_process_;
  idl::legacy::access::LoginProcess_var legacy_login_process_;
  idl::core::OctetSeq secret_;
  interceptors::ORBInitializer *orb_initializer_;
  friend class OpenBusContext;
  friend class Connection;
};

/**
 * \brief Conex�o para acesso identificado a um barramento.
 *
 * Uma conex�o � usada para realizar acessos identificados a um barramento.
 * Denominamos esses acessos identificados ao barramento de login. Cada login
 * possui um identificador �nico e est� sempre associado ao nome de uma
 * entidade que � autenticada no momento do estabelecimento do login.
 * H� basicamente tr�s formas de autentica��o de entidade dispon�veis:
 * - Por Senha: veja a opera��o \ref loginByPassword
 * - Por Certificado de login: veja a opera��o \ref loginByCertificate
 * - Por Autentica��o compartilhada: veja a opera��o \ref loginBySharedAuth
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
  : public boost::enable_shared_from_this<Connection>
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
  typedef boost::function<void (Connection & conn, idl::access::LoginInfo login)> 
    InvalidLoginCallback_t;
  
  /**
  * Efetua login no barramento como uma entidade usando autentica��o por senha.
  * 
  * A autentica��o por senha � validada usando um dos validadores de senha
  * definidos pelo adminsitrador do barramento.
  *
  * @param entity Identificador da entidade a ser autenticada.
  * @param password Senha de autentica��o no barramento da entidade.
  * 
  * @throw AlreadyLoggedIn A conex�o j� est� logada.
  * @throw idl::access::AccessDenied
  *        Senha fornecida para autentica��o da entidade n�o foi validada pelo 
  *        barramento.
  * @throw TooManyAttempts A autentica��o foi recusada por um n�mero 
  *        excessivo de tentativas inv�lidas de login por senha.
  * @throw idl::core::ServiceFailure 
  *        Ocorreu uma falha interna nos servi�os do barramento que impediu a 
  *        autentica��o da conex�o.
	* @throw idl::access::UnknownDomain O dom�nio de autentica��o n�o � conhecido.
  * @throw idl::access::WrongEncoding A autentica��o falhou, pois a senha n�o foi
  *        codificada corretamente com a chave p�blica do barramento.
  * @throw CORBA::Exception
  */
  void loginByPassword(
    const std::string &entity,
    const std::string &password,
    const std::string &domain = "");
  
 /**
  * \brief Efetua login de uma entidade usando autentica��o por certificado.
  * 
  * A autentica��o por certificado � validada usando um certificado de login
  * registrado pelo adminsitrador do barramento.
  * 
  * @param entity Identificador da entidade a ser conectada.
  * @param key Chave privada da entidade utilizada na autentica��o. 
  *            � uma precondi��o um EVP_PKEY v�lido e n�o nulo.
  * 
  * @throw AlreadyLoggedIn A conex�o j� est� autenticada.
  * @throw idl::access::AccessDenied 
  *        A chave privada fornecida n�o corresponde ao certificado da entidade 
  *        registrado no barramento indicado.
  * @throw idl::access::MissingCertificate 
  *        N�o h� certificado para essa entidade registrado no barramento
  *        indicado.
  * @throw idl::access::WrongEncoding A autentica��o falhou, pois a senha n�o foi
  *        codificada corretamente com a chave p�blica do barramento.
  * @throw idl::core::ServiceFailure 
  *        Ocorreu uma falha interna nos servi�os do barramento que impediu a 
  *        autentica��o da conex�o.
  * @throw CORBA::Exception
  */
  void loginByCertificate(const std::string &entity, EVP_PKEY *key);
  
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
  * @return Segredo a ser fornecido na conclus�o do processo de login.
  *
  * @throw idl::core::ServiceFailure 
  *        Ocorreu uma falha interna nos servi�os do barramento que impediu 
  *        o estabelecimento da conex�o.
  * @throw CORBA::Exception
  */
  SharedAuthSecret startSharedAuth();
  
  /**
  * \brief Efetua login de uma entidade usando autentica��o compartilhada.
  * 
  * A autentica��o compartilhada � feita a partir de um segredo obtido
  * atrav�s da opera��o 'startSharedAuth' de uma conex�o autenticada.
  * 
  * @param secret Segredo a ser fornecido na conclus�o do processo de login.
  * 
  * @throw WrongBus O segredo n�o pertence ao barramento contactado.
  * @throw InvalidLoginProcess A tentativa de login associada ao segredo
  *        informado � inv�lido, por exemplo depois do segredo ser
  *        cancelado, ter expirado, ou j� ter sido utilizado.
  * @throw AlreadyLoggedIn A conex�o j� est� autenticada.
  * @throw AccessDenied O segredo fornecido n�o corresponde ao esperado
  *        pelo barramento.
  * @throw idl::access::WrongEncoding A autentica��o falhou, pois a senha n�o foi
  *        codificada corretamente com a chave p�blica do barramento.
  * @throw ServiceFailure Ocorreu uma falha interna nos servi�os do
  *        barramento que impediu a autentica��o da conex�o.
  * @throw CORBA::Exception
  */
  void loginBySharedAuth(const SharedAuthSecret &secret);
  
  /**
	 * \brief Efetua logout da conex�o, tornando o login atual inv�lido.
	 * 
	 * Ap�s a chamada a essa opera��o a conex�o fica desautenticada, implicando que
	 * qualquer chamada realizada pelo ORB usando essa conex�o resultar� numa
	 * exce��o de sistema 'CORBA::NO_PERMISSION{NoLogin}' e chamadas recebidas
	 * por esse ORB ser�o respondidas com a exce��o
	 * 'CORBA::NO_PERMISSION{UnknownBus}' indicando que n�o foi poss�vel
	 * validar a chamada pois a conex�o est� temporariamente desautenticada.
	 * 
	 * @return Verdadeiro se o processo de logout for conclu�do com �xito e 
	 *         falso se n�o for poss�vel invalidar o login atual.
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
  const idl::access::LoginInfo *login() const;
  
  /**
   * Identificador do barramento ao qual essa conex�o se refere.
   */
  const std::string busid() const;
  ~Connection();  
private:
  /**
  * Connection deve ser adquirido atraves de: OpenBusContext::connectByReference()
  */
  Connection(CORBA::Object_var,
             CORBA::ORB_ptr, 
             interceptors::ORBInitializer *,
             OpenBusContext &, 
             EVP_PKEY *access_key,
             bool legacy_support);

  /**
  * Connection deve ser adquirido atraves de: OpenBusContext::connectByAddress()
  */
  Connection(const std::string host,
             const unsigned short port,
             CORBA::ORB_ptr, 
             interceptors::ORBInitializer *,
             OpenBusContext &, 
             EVP_PKEY *access_key,
             bool legacy_support);

  Connection(const Connection &);
  Connection &operator=(const Connection &);

  static void renewLogin(
    boost::weak_ptr<Connection> conn,
    idl::access::AccessControl_ptr acs, 
    OpenBusContext &ctx,
    idl::access::ValidityTime t);

  void login(idl::access::LoginInfo &loginInfo, 
             idl::access::ValidityTime validityTime);

  void checkBusid() const;
  bool _logout(bool local = true);
  CORBA::ORB_ptr orb() const 
  { 
    return _orb; 
  }

  idl::access::LoginRegistry_var login_registry() const 
  { 
    return _login_registry; 
  }

  idl::access::AccessControl_var access_control() const 
  { 
    return _access_control; 
  }

  const idl::access::LoginInfo *_login() const 
  { 
    boost::lock_guard<boost::mutex> lock(_mutex);;
    return _loginInfo.get(); 
  }

  idl::offers::OfferRegistry_var getOfferRegistry() const
  { 
    return _offer_registry;
  }

  idl::access::LoginRegistry_var getLoginRegistry() const
  {
    return _login_registry;
  }

  void init();

  idl::access::LoginInfo get_login();

  /* Variaveis que sao modificadas somente no construtor. */
  OpenBusContext &_openbusContext;
  PrivateKey _key;
  idl::access::AccessControl_var _access_control;
  idl::access::LoginRegistry_var _login_registry;
  idl::offers::OfferRegistry_var _offer_registry;
  boost::scoped_ptr<LoginCache> _loginCache;
  std::string _busid;
  boost::scoped_ptr<PublicKey> _buskey;
  bool _legacy_support;
  idl::legacy::access::AccessControl_var _legacy_access_control;
  idl::legacy_support::LegacyConverter_var _legacy_converter;
  /**/

  CORBA::Object_var _component_ref;
  scs::core::IComponent_var _iComponent;
  const std::string _host;
  const unsigned short _port;
  interceptors::ORBInitializer * _orb_init;
  CORBA::ORB_ptr _orb;
  boost::thread _renewLogin;
  mutable boost::mutex _mutex;
  boost::scoped_ptr<idl::access::LoginInfo> _loginInfo, _invalid_login;
  InvalidLoginCallback_t _onInvalidLogin;
  
  enum State 
  {
    LOGGED,
    UNLOGGED,
    INVALID
  } _state;
  
    
  struct SecretSession 
  {
    SecretSession()
      : id(0)
      , ticket(0)
      , is_legacy(false)
    {
      secret.fill(0);
    }
    CORBA::ULong id;
    std::string remote_id, entity;
    boost::array<unsigned char, secret_size> secret;
    CORBA::ULong ticket;
    bool is_legacy;
    friend bool operator==(const SecretSession &lhs, const SecretSession &rhs);
    friend bool operator!=(const SecretSession &lhs, const SecretSession &rhs);
  };
  typedef LRUCache<hash_value, std::string> profile2login_LRUCache;
  profile2login_LRUCache _profile2login;
  LRUCache<std::string, SecretSession> _login2session;

  friend struct openbus::interceptors::ServerInterceptor;
  friend struct openbus::interceptors::ClientInterceptor;
  friend class openbus::OpenBusContext;
  friend bool operator==(const SecretSession &lhs, const SecretSession &rhs);
  friend bool operator!=(const SecretSession &lhs, const SecretSession &rhs);
};

inline bool operator==(const Connection::SecretSession &lhs, 
                const Connection::SecretSession &rhs)
{
  return lhs.id == rhs.id
    && lhs.remote_id == rhs.remote_id
    && lhs.secret == rhs.secret
    && lhs.ticket == rhs.ticket;
}

inline bool operator!=(const Connection::SecretSession &lhs, 
                const Connection::SecretSession &rhs)
{
  return !(lhs == rhs);
}

}

#endif
