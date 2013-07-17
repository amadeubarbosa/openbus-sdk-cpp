// -*- coding: iso-8859-1-unix-*-

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
#ifndef TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#define TECGRAF_SDK_OPENBUS_LRUCACHE_H_
#include "openbus/LRUCache_impl.hpp"
#endif

#include <CORBA.h>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/shared_ptr.hpp>
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
 * \brief Conexão para acesso identificado a um barramento.
 *
 * Uma conexão é usada para realizar acessos identificados a um barramento.
 * Denominamos esses acessos identificados ao barramento de login. Cada login
 * possui um identificador único e está sempre associado ao nome de uma
 * entidade que é autenticada no momento do estabelecimento do login.
 * Há basicamente três formas de autenticação de entidade disponíveis:
 * - Por Senha: veja a operação 'loginByPassword'
 * - Por Certificado de login: veja a operação 'loginByCertificate'
 * - Por Autenticação compartilhada: veja a operação 'loginBySharedAuth'
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
  typedef boost::function<void (Connection &, idl_ac::LoginInfo)> 
    InvalidLoginCallback_t;
  
  /**
  * Efetua login no barramento como uma entidade usando autenticação por senha.
  * 
  * A autenticação por senha é validada usando um dos validadores de senha
  * definidos pelo adminsitrador do barramento.
  *
  * @param[in] entity Identificador da entidade a ser autenticada.
  * @param[in] password Senha de autenticação no barramento da entidade.
  * 
  * @throw AlreadyLoggedIn A conexão já está logada.
  * @throw idl_ac::AccessDenied
  *        Senha fornecida para autenticação da entidade não foi validada pelo 
  *        barramento.
  * @throw idl::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu a 
  *        autenticação da conexão.
  * @throw CORBA::Exception
  */
  void loginByPassword(const std::string &entity, const std::string &password);
  
 /**
  * \brief Efetua login de uma entidade usando autenticação por certificado.
  * 
  * A autenticação por certificado é validada usando um certificado de login
  * registrado pelo adminsitrador do barramento.
  * 
  * @param[in] entity Identificador da entidade a ser conectada.
  * @param[in] privKey Chave privada da entidade utilizada na autenticação.
  * 
  * @throw AlreadyLoggedIn A conexão já está autenticada.
  * @throw idl_ac::AccessDenied 
  *        A chave privada fornecida não corresponde ao certificado da entidade 
  *        registrado no barramento indicado.
  * @throw idl_ac::MissingCertificate 
  *        Não há certificado para essa entidade registrado no barramento
  *        indicado.
  * @throw idl::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu a 
  *        autenticação da conexão.
  * @throw CORBA::Exception
  */
  void loginByCertificate(const std::string &entity, const PrivateKey &);
  
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
  * @return Um par composto de um objeto que representa o processo de login 
  *         iniciado e de um segredo a ser fornecido na conclusão do processo 
  *         de login.
  *
  * @throw idl::ServiceFailure 
  *        Ocorreu uma falha interna nos serviços do barramento que impediu 
  *        o estabelecimento da conexão.
  * @throw CORBA::Exception
  */
  std::pair <idl_ac::LoginProcess_ptr, idl::OctetSeq> startSharedAuth();
  
  /**
  * \brief Efetua login de uma entidade usando autenticação compartilhada.
  * 
  * A autenticação compartilhada é feita a partir de informações obtidas a 
  * através da operação 'startSharedAuth' de uma conexão autenticada.
  * 
  * @param[in] loginProcess Objeto que represeta o processo de login iniciado.
  * @param[in] secret Segredo a ser fornecido na conclusão do processo de login.
  * 
  * @throw InvalidLoginProcess O LoginProcess informado é inválido, por exemplo 
  *        depois de ser cancelado ou ter expirado.
  * @throw AlreadyLoggedIn A conexão já está autenticada.
  * @throw idl_ac::AccessDenied 
  *        O segredo fornecido não corresponde ao esperado pelo barramento.
  * @throw idl::ServiceFailure 
  *        Ocorreu uma falha interna nos serviÃ§os do barramento que impediu o 
  *        estabelecimento da conexão.
  * @throw CORBA::Exception
  */
  void loginBySharedAuth(idl_ac::LoginProcess_ptr loginProcess, 
                         const idl::OctetSeq &secret);
  
 /**
  * \brief Efetua logout da conexão, tornando o login atual inválido.
  * 
  * Após a chamada a essa operação a conexão fica desautenticada, implicando que
  * qualquer chamada realizada pelo ORB usando essa conexão resultará numa
  * exceção de sistema 'CORBA::NO_PERMISSION{NoLogin}' e chamadas recebidas por
  * esse ORB serão respondidas com a exceção 'CORBA::NO_PERMISSION{UnknownBus}'
  * indicando que não foi possível validar a chamada pois a conexão está
  * temporariamente desautenticada.
  * 
  * @return Verdadeiro se o processo de logout for concluído com êxito e falso
  *         se a conexão já estiver desautenticada (login inválido) ou se houver 
  *         uma falha durante o processo remoto do logout.
  *
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
  const idl_ac::LoginInfo *login() const;
  
  /**
   * Identificador do barramento ao qual essa conexão se refere.
   */
  const std::string busid() const;
  ~Connection();  
private:
  /**
  * Connection deve ser adquirido atraves de: OpenBusContext::createConnection()
  */
  Connection(const std::string host, const unsigned short port, CORBA::ORB_ptr, 
             boost::shared_ptr<interceptors::orb_info>, OpenBusContext &, 
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
  CORBA::ORB_ptr _orb;
  boost::shared_ptr<interceptors::orb_info> _orb_info;
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread _renewLogin;
  mutable boost::mutex _mutex;
#else
  boost::scoped_ptr<RenewLogin> _renewLogin;
#endif
  boost::scoped_ptr<idl_ac::LoginInfo> _loginInfo;
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
  boost::scoped_ptr<LoginCache> _loginCache;
  std::string _busid;
  boost::scoped_ptr<PublicKey> _buskey;
  LegacyDelegate _legacyDelegate;
  bool _legacyEnabled;
  /**/
    
  struct SecretSession 
  {
    CORBA::ULong id;
    CORBA::String_var remoteId;
    CORBA::OctetSeq_var secret;
    CORBA::ULong ticket;
  };
  LRUCache<std::string, SecretSession> _profile2session;

  friend class openbus::interceptors::ServerInterceptor;
  friend class openbus::interceptors::ClientInterceptor;
  friend class openbus::OpenBusContext;
};
}

#endif
