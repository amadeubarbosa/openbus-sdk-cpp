// -*- coding: iso-8859-1 -*-

#ifndef OPENBUS_ASSISTANT_H
#define OPENBUS_ASSISTANT_H

#ifndef OPENBUS_ASSISTANT_DOXYGEN
#ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
#ifdef BOOST_PARAMETER_MAX_ARITY
#if BOOST_PARAMETER_MAX_ARITY < 14
#error The BOOST_PARAMETER_MAX_ARITY must be at least 14 before #including openbus/assistant.h or must not be defined at all. Add -DBOOST_PARAMETER_MAX_ARITY=14 to your build definitions
#endif // #if BOOST_PARAMETER_MAX_ARITY < 14
#else // #ifdef BOOST_PARAMETER_MAX_ARITY
#define BOOST_PARAMETER_MAX_ARITY 14
#endif // #ifdef BOOST_PARAMETER_MAX_ARITY
#include <boost/parameter.hpp>
#endif // #ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
#endif // #ifndef OPENBUS_ASSISTANT_DOXYGEN

#ifndef OPENBUS_ASSISTANT_DOXYGEN
#include <openbus/assistant/overloaded_error_handler.h>
#include <openbus/assistant/exceptions.h>
#include <openbus/assistant/detail/shared_state.h>
#include <openbus/assistant/error_handler_types.h>
#endif // #ifndef OPENBUS_ASSISTANT_DOXYGEN

#include <scs/IComponent.h>
#include <openbus/OpenBusContext.h>
#include <openbus/ORBInitializer.h>
#include <CORBA.h>

#include <boost/mpl/void.hpp>
#include <boost/mpl/vector.hpp>

namespace openbus { namespace assistant {

#ifndef OPENBUS_ASSISTANT_DOXYGEN
namespace idl = tecgraf::openbus::core::v2_0;
namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace idl_cr = tecgraf::openbus::core::v2_0::credential;
#endif // #ifndef OPENBUS_ASSISTANT_DOXYGEN

#ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
namespace keywords {
BOOST_PARAMETER_NAME(host);
BOOST_PARAMETER_NAME(port);
BOOST_PARAMETER_NAME(username);
BOOST_PARAMETER_NAME(entity);
BOOST_PARAMETER_NAME(password);
BOOST_PARAMETER_NAME(private_key);
BOOST_PARAMETER_NAME(private_key_filename);
BOOST_PARAMETER_NAME(shared_auth_callback);
BOOST_PARAMETER_NAME(argc);
BOOST_PARAMETER_NAME(argv);
BOOST_PARAMETER_NAME(on_login_error);
BOOST_PARAMETER_NAME(on_register_error);
BOOST_PARAMETER_NAME(on_fatal_error);
BOOST_PARAMETER_NAME(on_find_error);
BOOST_PARAMETER_NAME(retry_wait);
BOOST_PARAMETER_NAME(log_level);
}
using namespace keywords;
#endif // #ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS

#ifndef OPENBUS_ASSISTANT_DOXYGEN
struct AssistantImpl
#else
/** \brief Classe Openbus com API do assistants
 *
 * A classe Openbus deve ser instanciada por um de seus dois
 * named parameters startByPassword e startByCertificate. Essa
 * instancia mantera as ofertas adicionadas a ela e um login
 * validos no barramento indicado em sua construcao enquanto
 * um erro fatal nao ocorre e/ou um shutdown nao e invocado.
 */
struct Assistant
#endif // #ifndef OPENBUS_ASSISTANT_DOXYGEN
{
  typedef assistant::login_error_callback_type login_error_callback_type;
  typedef assistant::register_error_callback_type register_error_callback_type;
  typedef assistant::fatal_error_callback_type fatal_error_callback_type;
  typedef assistant::find_error_callback_type find_error_callback_type;

  typedef boost::function<std::pair<idl_ac::LoginProcess_ptr, idl::OctetSeq>()> shared_auth_callback_type;

#ifndef OPENBUS_ASSISTANT_DOXYGEN
#ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
protected:
  template <typename ArgumentPack>
  AssistantImpl(ArgumentPack const& args)
  {
    namespace mpl = boost::mpl;
    namespace parameter = boost::parameter;
    namespace tag = keywords::tag;

    typedef typename parameter::value_type
      <ArgumentPack, tag::entity, void>::type entity_type;
    typedef mpl::not_<boost::is_same<entity_type, void> > has_entity;

    typedef typename parameter::value_type
      <ArgumentPack, tag::private_key, void>::type private_key_type;
    typedef mpl::not_<boost::is_same<private_key_type, void> > has_private_key;

    typedef typename parameter::value_type
      <ArgumentPack, tag::private_key_filename, void>::type private_key_file_type;
    typedef mpl::not_<boost::is_same<private_key_file_type, void> > has_private_key_file;

    typedef typename parameter::value_type
      <ArgumentPack, tag::username, void>::type username_type;
    typedef mpl::not_<boost::is_same<username_type, void> > has_username;

    typedef typename parameter::value_type
      <ArgumentPack, tag::password, void>::type password_type;
    typedef mpl::not_<boost::is_same<password_type, void> > has_password;

    typedef typename parameter::value_type
      <ArgumentPack, tag::shared_auth_callback, void>::type shared_auth_cb_type;
    typedef mpl::not_<boost::is_same<shared_auth_cb_type, void> > has_shared_auth_cb;

    typedef typename parameter::value_type
      <ArgumentPack, tag::argc, void>::type argc_type;
    typedef mpl::not_<boost::is_same<argc_type, void> > has_argc;

    typedef typename parameter::value_type
      <ArgumentPack, tag::argv, void>::type argv_type;
    typedef mpl::not_<boost::is_same<argv_type, void> > has_argv;

    typedef mpl::and_<has_username, has_password> has_password_credential;
    typedef mpl::and_<mpl::not_<has_password_credential>
                      , mpl::or_<has_username, has_password> >
      has_partial_password_credential;

    typedef mpl::and_<has_entity, mpl::or_<has_private_key, has_private_key_file> > has_certificate_credential;
    typedef mpl::and_<mpl::not_<has_certificate_credential>
                      , mpl::or_<has_entity, has_private_key, has_private_key_file> >
      has_partial_certificate_credential;
    typedef mpl::and_<has_private_key, has_private_key_file> has_private_key_and_file;

    typedef has_shared_auth_cb has_shared_auth_credential;

    typedef mpl::and_<has_argc, has_argv> has_args;
    typedef mpl::or_<has_password_credential, has_certificate_credential, has_shared_auth_credential> has_credential;
    typedef mpl::and_<has_password_credential, has_certificate_credential> has_certificate_and_password_credentials;
    typedef mpl::and_<has_password_credential, has_shared_auth_credential> has_shared_auth_and_password_credentials;
    typedef mpl::and_<has_certificate_credential, has_shared_auth_credential> has_shared_auth_and_certificate_credentials;

    // Se a compila��o falhou nesta asser��o, ent�o seu c�digo
    // preencheu ambos par�metros _private_key e _private_key_filename.
    // Deve-se apenas preencher um deles
    BOOST_MPL_ASSERT((mpl::not_<has_private_key_and_file>));

    // Se a a compila��o falhou nesta asser��o, ent�o seu c�digo
    // preencheu apenas parte das informa��es necess�rias para
    // aut�nticar ao barramento por senha. Ou seja, preencheu
    // _username sem _password ou _password sem _username.
    BOOST_MPL_ASSERT((mpl::not_<has_partial_password_credential>));

    // Se a a compila��o falhou nesta asser��o, ent�o seu c�digo
    // preencheu apenas parte das informa��es necess�rias para
    // aut�nticar ao barramento por certificado. Ou seja, preencheu
    // _entity sem _private_key e _private_key_filename
    // ou preencheu _private_key ou _private_key_filename sem _entity
    BOOST_MPL_ASSERT((mpl::not_<has_partial_certificate_credential>));

    // Se a compila��o falhar nesta asser��o, ent�o seu c�digo esqueceu
    // de passar uma das credenciais para login. Voc� deve fazer um
    // dos tr�s:
    // openbus::assistant::Assistant assistant
    //  ([...], _username = "myusername", _password = "mypassword");
    // ou:
    // openbus::assistant::Assistant assistant
    //  ([...], _entity = "entidade do servi�o", _private_key = private_key_octet_seq);
    // ou:
    // openbus::assistant::Assistant assistant
    //  ([..], _shared_auth_callback = my_shared_auth_callback);
    BOOST_MPL_ASSERT((has_credential));

    // Se a compila��o falhar nesta asser��o, ent�o seu c�digo passou credenciais
    // de duas ou mais formas. Voc� deve escolher entre _username/_password e
    // _entity/_private_key
    BOOST_MPL_ASSERT((mpl::not_<has_certificate_and_password_credentials>));

    // Se a compila��o falhar nesta asser��o, ent�o seu c�digo passou credenciais
    // de duas ou mais formas. Voc� deve escolher entre _username/_password e
    // _shared_auth_callback
    BOOST_MPL_ASSERT((mpl::not_<has_shared_auth_and_password_credentials>));

    // Se a compila��o falhar nesta asser��o, ent�o seu c�digo passou credenciais
    // de duas ou mais formas. Voc� deve escolher entre _entity/_private_key e
    // _shared_auth_callback
    BOOST_MPL_ASSERT((mpl::not_<has_shared_auth_and_certificate_credentials>));

    typedef mpl::void_ void_;
    init_function()
      (this, args[_host]
       , args[_port]
       , args[_username | void_()]
       , args[_password | void_()]
       , args[_entity | void_()]
       , args[_private_key | void_()]
       , args[_private_key_filename | void_()]
       , args[_shared_auth_callback | void_()]
       , args[_argc | void_()]
       , args[_argv | void_()]
       , args[_on_login_error | login_error_callback_type()]
       , args[_on_register_error | register_error_callback_type()]
       , args[_on_fatal_error | fatal_error_callback_type()]
       , args[_on_find_error | find_error_callback_type()]
       , args[_log_level | logger::warning_level]
       );


    typedef boost::chrono::seconds seconds;
    unsigned int retry_wait
      = args[_retry_wait | assistant::default_retry_wait];
    state->retry_wait = seconds(retry_wait);
  }

  typedef boost::mpl::void_ void_;
  struct init_function
  {
    typedef void result_type;
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port
                           , std::string username, std::string password, void_, void_, void_, void_
                           , int& argc, char** argv
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error
                           , find_error_callback_type find_error
                           , logger::level l) const
    {
      self->InitWithPassword(hostname, port, username, password, argc, argv
                             , login_error, register_error, fatal_error, find_error, l);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port
                           , std::string username, std::string password, void_, void_, void_, void_, void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error
                           , find_error_callback_type find_error
                           , logger::level l) const
    {
      self->InitWithPassword(hostname, port, username, password
                             , login_error, register_error, fatal_error, find_error, l);
    }
    result_type operator()(AssistantImpl* self
                           , std::string hostname, unsigned short port, void_, void_
                           , std::string entity, CORBA::OctetSeq private_key, void_, void_
                           , int& argc, char** argv
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error
                           , find_error_callback_type find_error
                           , logger::level l) const
    {
      self->InitWithPrivateKey(hostname, port, entity, private_key, argc, argv
                               , login_error, register_error, fatal_error, find_error, l);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port, void_, void_
                           , std::string entity, CORBA::OctetSeq private_key, void_, void_, void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error
                           , find_error_callback_type find_error
                           , logger::level l) const
    {
      self->InitWithPrivateKey(hostname, port, entity, private_key
                               , login_error, register_error, fatal_error, find_error, l);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port, void_, void_
                           , std::string entity, void_, std::string private_key_filename, void_
                           , int& argc, char** argv
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error
                           , find_error_callback_type find_error
                           , logger::level l) const
    {
      self->InitWithPrivateKeyFile(hostname, port, entity, private_key_filename, argc, argv
                                   , login_error, register_error, fatal_error, find_error, l);
    }
    result_type operator()(AssistantImpl* self
                           , std::string hostname, unsigned short port, void_, void_
                           , std::string entity, void_, std::string private_key_filename, void_, void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error
                           , find_error_callback_type find_error
                           , logger::level l) const
    {
      self->InitWithPrivateKeyFile(hostname, port, entity, private_key_filename
                                   , login_error, register_error, fatal_error, find_error, l);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port, void_, void_
                           , void_, void_, void_
                           , shared_auth_callback_type shared_auth_callback
                           , int& argc, char** argv
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error
                           , find_error_callback_type find_error
                           , logger::level l) const
    {
      self->InitWithSharedAuth(hostname, port, shared_auth_callback, argc, argv
                               , login_error, register_error, fatal_error, find_error, l);
    }
    result_type operator()(AssistantImpl* self
                           , std::string hostname, unsigned short port, void_, void_, void_
                           , void_, void_, void_
                           , shared_auth_callback_type shared_auth_callback
                           , void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error
                           , find_error_callback_type find_error
                           , logger::level l) const
    {
      self->InitWithSharedAuth(hostname, port, shared_auth_callback
                               , login_error, register_error, fatal_error, find_error, l);
    }
  };
public:
#endif // #ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
#else  // #ifndef OPENBUS_ASSISTANT_DOXYGEN
  Assistant(NamedParameters ...);
#endif // #ifndef OPENBUS_ASSISTANT_DOXYGEN

  /** 
   * \brief Atribui uma funcao callback para erros de login
   */
  void onLoginError(boost::function<void(std::string /*error*/)> f);

  /** 
   * \brief Atribui uma funcao callback para erros de registro
   *  de ofertas
   */
  void onRegisterError(boost::function<void(scs::core::IComponent_var
                                            , idl_or::ServicePropertySeq
                                            , std::string /*error*/)> f);

  /** 
   * \brief Atribui uma funcao callback para erros fatais
   *  que impossibilitam a continuacao da execucao
   *  do assistant
   */
  void onFatalError(boost::function<void(const char* /*error*/)> f);

  /** 
   * \brief Solicita que o assistente registre um servi�o no barramento.
   * 
   * Esse m�todo notifica o assistente de que o servi�o fornecido deve ser
   * mantido como uma oferta de servi�o v�lida no barramento. Para tanto,
   * sempre que o assistente restabelecer o login esse servi�o ser� registrado
   * novamente no barramento.
   *
   * Para que o registro de servi�os seja bem sucedido � necess�rio que o ORB
   * utilizado pelo assistente esteja processando chamadas, por exemplo,
   * fazendo com que a aplica��o chame o m�todo 'ORB::run()'.
   * 
   * Caso ocorram erros, a callback de tratamento de erro apropriada ser�
   * chamada.
   *
   * \param component Refer�ncia do servi�o sendo ofertado.
   * \param properties Propriedades do servi�o sendo ofertado.
   */
  void registerService(scs::core::IComponent_var component, idl_or::ServicePropertySeq properties = idl_or::ServicePropertySeq());

  /** 
   * \brief Constroi sequencia de propriedades para determinada faceta e entidade
   *
   * \param facet Nome da faceta que a oferta deve possuir
   * \param entity Nome da entidade criadora da oferta
   */
  static idl_or::ServicePropertySeq createFacetAndEntityProperty(const char* facet, const char* entity);

  /** 
   * \brief Busca por ofertas que apresentem um conjunto de propriedades
   *        definido.
   * 
   * Ser�o selecionadas apenas as ofertas de servi�o que apresentem todas as 
   * propriedades especificadas. As propriedades utilizadas nas buscas podem
   * ser aquelas fornecidas no momento do registro da oferta de servi�o, assim
   * como as propriedades automaticamente geradas pelo barramento.
   * 
   * Caso ocorram erros, a callback de tratamento de erro apropriada ser�
   * chamada. Se o n�mero de tentativas se esgotar e n�o houver sucesso, uma
   * sequ�ncia vazia ser� retornada.
   *
   * \param properties Propriedades que as ofertas de servi�os encontradas
   *                   devem apresentar.
   * \param retries Par�metro opcional indicando o n�mero de novas tentativas
   *                de busca de ofertas em caso de falhas, como o barramento
   *                estar indispon�vel ou n�o ser poss�vel estabelecer um login
   *                at� o momento. 'retries' com o valor 0 implica que a
   *                opera��o retorna imediatamente ap�s uma �nica tentativa.
   *                Para tentar indefinidamente o valor de 'retries' deve ser
   *                -1. Entre cada tentativa � feita uma pausa dada pelo
   *                par�metro 'interval' fornecido na cria��o do assistente
   *                (veja a interface 'AssistantFactory').
   *
   * \return Sequ�ncia de descri��es de ofertas de servi�o encontradas.
   */
  idl_or::ServiceOfferDescSeq findServices(idl_or::ServicePropertySeq properties
                                           , int retries) const;

  /**
   * \brief Devolve uma lista de todas as ofertas de servi�o registradas.
   * 
   * Caso ocorram erros, a callback de tratamento de erro apropriada ser�
   * chamada. Se o n�mero de tentativas se esgotar e n�o houver sucesso, uma
   * sequ�ncia vazia ser� retornada.
   * 
   * \param retries Par�metro opcional indicando o n�mero de novas tentativas
   *                de busca de ofertas em caso de falhas, como o barramento
   *                estar indispon�vel ou n�o for poss�vel estabelecer um login
   *                at� o momento. 'retries' com o valor 0 implica que a
   *                opera��o retorna imediatamente ap�s uma �nica tentativa.
   *                Para tentar indefinidamente o valor de 'retries' deve ser
   *                -1. Entre cada tentativa � feita uma pausa dada pelo
   *                par�metro 'interval' fornecido na cria��o do assistente
   *                (veja a interface 'AssistantFactory').
   * 
   * \return Sequ�ncia de descri��es de ofertas de servi�o registradas.
   */
  idl_or::ServiceOfferDescSeq getAllServices(int retries) const;

  /** 
   * \brief Filtra ofertas e retorna somente as que se encontram responsivas
   *
   * \param offers Ofertas a serem verificadas por responsividade
   */
  static idl_or::ServiceOfferDescSeq filterWorkingOffers(idl_or::ServiceOfferDescSeq offers);
  
  /**
   * \brief Inicia o processo de login por autentica��o compartilhada.
   * 
   * A autentica��o compartilhada permite criar um novo login compartilhando a
   * mesma autentica��o do login atual da conex�o. Portanto essa opera��o s�
   * pode ser chamada enquanto a conex�o estiver autenticada, caso contr�rio a
   * exce��o de sistema CORBA::NO_PERMISSION{NoLogin} � lan�ada. As informa��es
   * fornecidas por essa opera��o devem ser passadas para a opera��o
   * 'loginBySharedAuth' para conclus�o do processo de login por autentica��o
   * compartilhada. Isso deve ser feito dentro do tempo de lease definido pelo
   * administrador do barramento. Caso contr�rio essas informa��es se tornam
   * inv�lidas e n�o podem mais ser utilizadas para criar um login.
   * 
   * \param secret Segredo a ser fornecido na conclus�o do processo de login.
   * \param retries Par�metro opcional indicando o n�mero de novas tentativas
   *                de busca de ofertas em caso de falhas, como o barramento
   *                estar indispon�vel ou n�o for poss�vel estabelecer um login
   *                at� o momento. 'retries' com o valor 0 implica que a
   *                opera��o retorna imediatamente ap�s uma �nica tentativa.
   *                Para tentar indefinidamente o valor de 'retries' deve ser
   *                -1. Entre cada tentativa � feita uma pausa dada pelo
   *                par�metro 'interval' fornecido na cria��o do assistente
   *                (veja a interface 'AssistantFactory').
   *
   * \return Objeto que representa o processo de login iniciado.
   */
  std::pair<idl_ac::LoginProcess_ptr, idl::OctetSeq> startSharedAuth(int retries);

  /** 
   * \brief Encerra o funcionamento do assistente liberando todos os recursos
   *        alocados por ele.
   * 
   * Essa opera��o deve ser chamada antes do assistente ser descartado, pois
   * como o assistente tem um funcionamento ativo, ele continua funcionando e
   * consumindo recursos mesmo que a aplica��o n�o tenha mais refer�ncias a ele.
   * Em particular, alguns dos recursos gerenciados pelo assistente s�o:
   * - Login no barramento;
   * - Ofertas de servi�o registradas no barramento;
   * - Observadores de servi�o registrados no barramento;
   * - Threads de manuten��o desses recursos no barramento;
   * - Conex�o default no ORB sendo utilizado;
   *
   * Em particular, o processamento de requisi��es do ORB (e.g. atrav�s da
   * opera��o 'ORB::run()') n�o � gerido pelo assistente, portanto �
   * responsabilidade da aplica��o iniciar e parar esse processamento (e.g.
   * atrav�s da opera��o 'ORB::shutdown()')
   */
  void shutdown();

  /** 
   * \brief ORB utilizado pelo assistente.
   */
  CORBA::ORB_var orb() const
  {
    assert(!!state);
    return state->orb;
  }

  logger::logger& logging() const
  {
    assert(!!state);
    return state->logging;
  }

  CallerChain getCallerChain();
  void joinChain(CallerChain chain);
  void exitChain();
  CallerChain getJoinedChain();
#ifndef OPENBUS_ASSISTANT_DOXYGEN
protected:
  friend class assistant_access;
  AssistantImpl() {}
  void InitWithPassword(std::string const& hostname, unsigned short port
                        , std::string const& username, std::string const& password
                        , int& argc, char** argv
                        , login_error_callback_type login_error
                        , register_error_callback_type register_error
                        , fatal_error_callback_type fatal_error
                        , find_error_callback_type find_error
                        , logger::level l);
  void InitWithPassword(std::string const& hostname, unsigned short port
                        , std::string const& username, std::string const& password
                        , login_error_callback_type login_error
                        , register_error_callback_type register_error
                        , fatal_error_callback_type fatal_error
                        , find_error_callback_type find_error
                        , logger::level l);
  void InitWithPrivateKey(std::string const& hostname, unsigned short port
                          , std::string const& entity, CORBA::OctetSeq const& private_key
                          , int& argc, char** argv
                          , login_error_callback_type login_error
                          , register_error_callback_type register_error
                          , fatal_error_callback_type fatal_error
                          , find_error_callback_type find_error
                          , logger::level l);
  void InitWithPrivateKey(std::string const& hostname, unsigned short port
                          , std::string const& entity, CORBA::OctetSeq const& private_key
                          , login_error_callback_type login_error
                          , register_error_callback_type register_error
                          , fatal_error_callback_type fatal_error
                          , find_error_callback_type find_error
                          , logger::level l);
  void InitWithPrivateKeyFile(std::string const& hostname, unsigned short port
                              , std::string const& entity, std::string const& private_key_file
                              , int& argc, char** argv
                              , login_error_callback_type login_error
                              , register_error_callback_type register_error
                              , fatal_error_callback_type fatal_error
                              , find_error_callback_type find_error
                              , logger::level l);
  void InitWithPrivateKeyFile(std::string const& hostname, unsigned short port
                              , std::string const& entity, std::string const& private_key_file
                              , login_error_callback_type login_error
                              , register_error_callback_type register_error
                              , fatal_error_callback_type fatal_error
                              , find_error_callback_type find_error
                              , logger::level l);
  void InitWithSharedAuth(std::string const& hostname, unsigned short port
                          , shared_auth_callback_type shared_auth_callback
                          , int& argc, char** argv
                          , login_error_callback_type login_error
                          , register_error_callback_type register_error
                          , fatal_error_callback_type fatal_error
                          , find_error_callback_type find_error
                          , logger::level l);
  void InitWithSharedAuth(std::string const& hostname, unsigned short port
                          , shared_auth_callback_type shared_auth_callback
                          , login_error_callback_type login_error
                          , register_error_callback_type register_error
                          , fatal_error_callback_type fatal_error
                          , find_error_callback_type find_error
                          , logger::level l);

  boost::shared_ptr<assistant_detail::shared_state> state;
};

struct Assistant : AssistantImpl
{
#ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
#define OPENBUS_ASSISTANT_STRING_LAMBDA() \
  *(boost::mpl::or_                       \
  <                                       \
    boost::is_convertible                 \
    <boost::mpl::_                        \
     , char const*                        \
    >                                     \
    , boost::is_convertible               \
    <boost::mpl::_                        \
    , std::string> >)

  BOOST_PARAMETER_CONSTRUCTOR
  (Assistant, (AssistantImpl), keywords::tag
   , (required
      (host, OPENBUS_ASSISTANT_STRING_LAMBDA())
      (port, (unsigned short)))
   (optional
    (log_level, (logger::level))
    (username, OPENBUS_ASSISTANT_STRING_LAMBDA())
    (entity, OPENBUS_ASSISTANT_STRING_LAMBDA())
    (password, OPENBUS_ASSISTANT_STRING_LAMBDA())
    (private_key, (CORBA::OctetSeq))
    (in_out(argc), (int&))
    (in_out(argv), (char**))
    (retry_wait, (unsigned int))
    (on_login_error, (login_error_callback_type))
    (on_register_error, (register_error_callback_type))
    (on_fatal_error, (fatal_error_callback_type))
    (on_find_error, (find_error_callback_type))
   )
  )
#endif // #ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
#endif // #ifdef OPENBUS_ASSISTANT_DOXYGEN

  /** \brief Constroi um Openbus com informacao de autenticacao
   *   por usuario e senha
   */
  static Assistant createWithPassword(const char* username, const char* password
                                      , const char* host, unsigned short port
                                      , int& argc, char** argv
                                      , login_error_callback_type login_error = login_error_callback_type()
                                      , register_error_callback_type register_error
                                         = register_error_callback_type()
                                      , fatal_error_callback_type fatal_error = fatal_error_callback_type()
                                      , find_error_callback_type find_error = find_error_callback_type()
                                      , logger::level l = logger::warning_level);

  /** \brief Constriu um Openbus com informacao de autenticacao
   *  por certificado
   */
  static Assistant createWithPrivateKey(const char* entity, const idl::OctetSeq privKey
                                        , const char* host, unsigned short port
                                        , int& argc, char** argv
                                        , login_error_callback_type login_error = login_error_callback_type()
                                        , register_error_callback_type register_error
                                          = register_error_callback_type()
                                        , fatal_error_callback_type fatal_error = fatal_error_callback_type()
                                        , find_error_callback_type find_error = find_error_callback_type()
                                        , logger::level l = logger::warning_level);

  /** \brief Constriu um Openbus com informacao de autenticacao
   *  por Shared Authentication
   */
  static Assistant createWithSharedAuth(shared_auth_callback_type shared_auth_callback
                                        , const char* host, unsigned short port
                                        , int& argc, char** argv
                                        , login_error_callback_type login_error = login_error_callback_type()
                                        , register_error_callback_type register_error
                                          = register_error_callback_type()
                                        , fatal_error_callback_type fatal_error = fatal_error_callback_type()
                                        , find_error_callback_type find_error = find_error_callback_type()
                                        , logger::level l = logger::warning_level);
private:
#ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
  Assistant() {}
#endif // #ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
};

} }

#endif // #ifndef OPENBUS_ASSISTANT_H
