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
 * \brief Segredo para compartilhamento de autenticação.
 *
 * Objeto que representa uma tentativa de compartilhamento de
 * autenticação através do compartilhamento de um segredo, que pode
 * ser utilizado para realizar uma autenticação junto ao barramento em
 * nome da mesma entidade que gerou e compartilhou o segredo.
 *
 * Cada segredo de autenticação compartilhada pertence a um único barramento e
 * só pode ser utilizado em uma única autenticação.
 *
 */
class OPENBUS_SDK_DECL SharedAuthSecret
{
public:
   /**
    * \brief Cancela o segredo caso esse ainda esteja ativo, de forma
    * que ele não poderá ser mais utilizado.  
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
 * \brief Conexão para acesso identificado a um barramento.
 *
 * Uma conexão é usada para realizar acessos identificados a um barramento.
 * Denominamos esses acessos identificados ao barramento de login. Cada login
 * possui um identificador único e está sempre associado ao nome de uma
 * entidade que é autenticada no momento do estabelecimento do login.
 * Há basicamente três formas de autenticação de entidade disponíveis:
 * - Por Senha: veja a operação \ref loginByPassword
 * - Por Certificado de login: veja a operação \ref loginByCertificate
 * - Por Autenticação compartilhada: veja a operação \ref loginBySharedAuth
 *
 * A entidade associada ao login é responsável por todas as chamadas feitas
 * através daquela conexão e essa entidade deve ser levada em consideração
 * pelos serviços ofertados no barramento para decidir aceitar ou recusar
 * chamadas.
 *
 * É possível uma aplicação assumir múltiplas identidades ao acessar um ou mais
 * barramentos criando múltiplas conexões para esses barramentos.
 * 
 * É importante notar que a conexão não é usada diretamente pela aplicação ao
 * realizar ou receber chamadas, pois as chamadas ocorrem usando proxies e
 * servants de um ORB. As conexões que são efetivamente usadas nas chamadas do
 * ORB são definidas através do OpenBusContext associado ao ORB.
 */
class OPENBUS_SDK_DECL Connection
  : public boost::enable_shared_from_this<Connection>
{
public:
  typedef std::vector<std::pair<std::string, std::string> > 
    ConnectionProperties;
  /**
   * \brief Callback de login inválido.
   * 
   * Tipo que representa um objeto função ('function object') a ser chamado
   * quando uma notificação de login inválido é recebida. Caso alguma exceção
   * ocorra durante a execução do método e não seja tratada, o erro será
   * capturado pelo interceptador e registrado no log.
   * 
   * O tipo InvalidLoginCallback_t é um typedef de boost::function. Para
   * documentação dessa biblioteca acesse
   * http://www.boost.org/doc/libs/1_47_0/doc/html/function.html
   *
   * \param conn Conexão que recebeu a notificação de login inválido.
   * \param login Informações do login que se tornou inválido.
   */
  typedef boost::function<void (Connection & conn, idl::access::LoginInfo login)> 
    InvalidLoginCallback_t;
  
  /**
  * Efetua login no barramento como uma entidade usando autenticação por senha.
  * 
  * A autenticação por senha é validada usando um dos validadores de senha
  * definidos pelo adminsitrador do barramento.
  *
  * @param entity Identificador da entidade a ser autenticada.
  * @param password Senha de autenticação no barramento da entidade.
  * 
  * @throw AlreadyLoggedIn A conexão já está logada.
  * @throw idl::access::AccessDenied
  *        Senha fornecida para autenticação da entidade não foi validada pelo 
  *        barramento.
  * @throw TooManyAttempts A autenticação foi recusada por um número 
  *        excessivo de tentativas inválidas de login por senha.
  * @throw idl::core::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu a 
  *        autenticação da conexão.
	* @throw idl::access::UnknownDomain O domínio de autenticação não é conhecido.
  * @throw idl::access::WrongEncoding A autenticação falhou, pois a senha não foi
  *        codificada corretamente com a chave pública do barramento.
  * @throw CORBA::Exception
  */
  void loginByPassword(
    const std::string &entity,
    const std::string &password,
    const std::string &domain = "");
  
 /**
  * \brief Efetua login de uma entidade usando autenticação por certificado.
  * 
  * A autenticação por certificado é validada usando um certificado de login
  * registrado pelo adminsitrador do barramento.
  * 
  * @param entity Identificador da entidade a ser conectada.
  * @param key Chave privada da entidade utilizada na autenticação. 
  *            É uma precondição um EVP_PKEY válido e não nulo.
  * 
  * @throw AlreadyLoggedIn A conexão já está autenticada.
  * @throw idl::access::AccessDenied 
  *        A chave privada fornecida não corresponde ao certificado da entidade 
  *        registrado no barramento indicado.
  * @throw idl::access::MissingCertificate 
  *        Não há certificado para essa entidade registrado no barramento
  *        indicado.
  * @throw idl::access::WrongEncoding A autenticação falhou, pois a senha não foi
  *        codificada corretamente com a chave pública do barramento.
  * @throw idl::core::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu a 
  *        autenticação da conexão.
  * @throw CORBA::Exception
  */
  void loginByCertificate(const std::string &entity, EVP_PKEY *key);
  
  /**
  * \brief Inicia o processo de login por autenticação compartilhada.
  * 
  * A autenticação compartilhada permite criar um novo login compartilhando a
  * mesma autenticação do login atual da conexão. Portanto essa operação só pode
  * ser chamada enquanto a conexão estiver autenticada, caso contrário a exceção
  * de sistema CORBA::NO_PERMISSION{NoLogin} é lançada. As informações
  * fornecidas por essa operação devem ser passadas para a operação
  * 'loginBySharedAuth' para conclusão do processo de login por autenticação
  * compartilhada. Isso deve ser feito dentro do tempo de lease definido pelo
  * administrador do barramento. Caso contrário essas informações se tornam
  * inválidas e não podem mais ser utilizadas para criar um login.
  * 
  * @return Segredo a ser fornecido na conclusão do processo de login.
  *
  * @throw idl::core::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu 
  *        o estabelecimento da conexão.
  * @throw CORBA::Exception
  */
  SharedAuthSecret startSharedAuth();
  
  /**
  * \brief Efetua login de uma entidade usando autenticação compartilhada.
  * 
  * A autenticação compartilhada é feita a partir de um segredo obtido
  * através da operação 'startSharedAuth' de uma conexão autenticada.
  * 
  * @param secret Segredo a ser fornecido na conclusão do processo de login.
  * 
  * @throw WrongBus O segredo não pertence ao barramento contactado.
  * @throw InvalidLoginProcess A tentativa de login associada ao segredo
  *        informado é inválido, por exemplo depois do segredo ser
  *        cancelado, ter expirado, ou já ter sido utilizado.
  * @throw AlreadyLoggedIn A conexão já está autenticada.
  * @throw AccessDenied O segredo fornecido não corresponde ao esperado
  *        pelo barramento.
  * @throw idl::access::WrongEncoding A autenticação falhou, pois a senha não foi
  *        codificada corretamente com a chave pública do barramento.
  * @throw ServiceFailure Ocorreu uma falha interna nos serviços do
  *        barramento que impediu a autenticação da conexão.
  * @throw CORBA::Exception
  */
  void loginBySharedAuth(const SharedAuthSecret &secret);
  
  /**
	 * \brief Efetua logout da conexão, tornando o login atual inválido.
	 * 
	 * Após a chamada a essa operação a conexão fica desautenticada, implicando que
	 * qualquer chamada realizada pelo ORB usando essa conexão resultará numa
	 * exceção de sistema 'CORBA::NO_PERMISSION{NoLogin}' e chamadas recebidas
	 * por esse ORB serão respondidas com a exceção
	 * 'CORBA::NO_PERMISSION{UnknownBus}' indicando que não foi possível
	 * validar a chamada pois a conexão está temporariamente desautenticada.
	 * 
	 * @return Verdadeiro se o processo de logout for concluído com êxito e 
	 *         falso se não for possível invalidar o login atual.
	 */
  bool logout();
  	
  /**
   * \brief Callback a ser chamada quando o login atual se tornar inválido.
   *
   * Esse atributo é utilizado para definir um objeto função (function object)
   * que implementa uma interface de callback a ser chamada sempre que a conexão
   * receber uma notificação de que o seu login está inválido. Essas
   * notificações ocorrem durante chamadas realizadas ou recebidas pelo
   * barramento usando essa conexão. Um login pode se tornar inválido caso o
   * administrador explicitamente o torne inválido ou caso a thread interna de
   * renovação de login não seja capaz de renovar o lease do login a tempo. Caso
   * esse atributo seja um InvalidLoginCallback_t 'default-constructed', nenhum
   * objeto de callback é chamado na ocorrência desse evento.
   *
   * Durante a execução dessa callback um novo login pode ser restabelecido.
   * Neste caso, a chamada do barramento que recebeu a notificação de login
   * inválido é refeita usando o novo login, caso contrário, a chamada original
   * lança a exceção de de sistema 'CORBA::NO_PERMISSION{NoLogin}'.
   * 
   * O tipo InvalidLoginCallback_t é um typedef de boost::function. Para
   * documentação dessa biblioteca acesse
   * http://www.boost.org/doc/libs/1_47_0/doc/html/function.html
   */
  void onInvalidLogin(InvalidLoginCallback_t p);
  
  /**
   * \brief Retorna a callback configurada para ser chamada quando o login atual
   * se torna inválido.
   */
  InvalidLoginCallback_t onInvalidLogin() const;
  
  /**
   * \brief Informações do login dessa conexão ou 'null' se a conexão não está
   * autenticada, ou seja, não tem um login válido no barramento.
   */
  const idl::access::LoginInfo *login() const;
  
  /**
   * Identificador do barramento ao qual essa conexão se refere.
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
