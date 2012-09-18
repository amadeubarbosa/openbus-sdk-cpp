// -*- coding: iso-8859-1 -*-

#ifndef OPENBUS_ASSISTANT_H
#define OPENBUS_ASSISTANT_H

#include <openbus/assistant/detail/shared_state.h>

#include <scs/IComponent.h>
#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <CORBA.h>

#ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
#define BOOST_PARAMETER_MAX_ARITY 12
#include <boost/parameter.hpp>
#endif

namespace openbus { namespace assistant {

#ifndef OPENBUS_ASSISTANT_DOXYGEN
namespace idl = tecgraf::openbus::core::v2_0;
namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace idl_cr = tecgraf::openbus::core::v2_0::credential;

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
BOOST_PARAMETER_NAME(retry_wait);
}
using namespace keywords;
#endif

struct AssistantImpl
{
  typedef boost::function<void(std::string)> login_error_callback_type;
  typedef boost::function<void(scs::core::IComponent_var
                               , idl_or::ServicePropertySeq
                               , std::string)> register_error_callback_type;
  typedef boost::function<void(const char*)> fatal_error_callback_type;
  typedef boost::function<std::pair<idl_ac::LoginProcess_ptr, idl::OctetSeq>()> shared_auth_callback_type;

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

    // Se a compilação falhou nesta asserção, então seu código
    // preencheu ambos parâmetros _private_key e _private_key_filename.
    // Deve-se apenas preencher um deles
    BOOST_MPL_ASSERT((mpl::not_<has_private_key_and_file>));

    // Se a a compilação falhou nesta asserção, então seu código
    // preencheu apenas parte das informações necessárias para
    // autênticar ao barramento por senha. Ou seja, preencheu
    // _username sem _password ou _password sem _username.
    BOOST_MPL_ASSERT((mpl::not_<has_partial_password_credential>));

    // Se a a compilação falhou nesta asserção, então seu código
    // preencheu apenas parte das informações necessárias para
    // autênticar ao barramento por certificado. Ou seja, preencheu
    // _entity sem _private_key e _private_key_filename
    // ou preencheu _private_key ou _private_key_filename sem _entity
    BOOST_MPL_ASSERT((mpl::not_<has_partial_certificate_credential>));

    // Se a compilação falhar nesta asserção, então seu código esqueceu
    // de passar uma das credenciais para login. Você deve fazer um
    // dos três:
    // openbus::assistant::Assistant assistant
    //  ([...], _username = "myusername", _password = "mypassword");
    // ou:
    // openbus::assistant::Assistant assistant
    //  ([...], _entity = "entidade do serviço", _private_key = private_key_octet_seq);
    // ou:
    // openbus::assistant::Assistant assistant
    //  ([..], _shared_auth_callback = my_shared_auth_callback);
    BOOST_MPL_ASSERT((has_credential));

    // Se a compilação falhar nesta asserção, então seu código passou credenciais
    // de duas ou mais formas. Você deve escolher entre _username/_password e
    // _entity/_private_key
    BOOST_MPL_ASSERT((mpl::not_<has_certificate_and_password_credentials>));

    // Se a compilação falhar nesta asserção, então seu código passou credenciais
    // de duas ou mais formas. Você deve escolher entre _username/_password e
    // _shared_auth_callback
    BOOST_MPL_ASSERT((mpl::not_<has_shared_auth_and_password_credentials>));

    // Se a compilação falhar nesta asserção, então seu código passou credenciais
    // de duas ou mais formas. Você deve escolher entre _entity/_private_key e
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
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPassword(hostname, port, username, password, argc, argv
                             , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port
                           , std::string username, std::string password, void_, void_, void_, void_, void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPassword(hostname, port, username, password
                             , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self
                           , std::string hostname, unsigned short port, void_, void_
                           , std::string entity, CORBA::OctetSeq private_key, void_, void_
                           , int& argc, char** argv
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPrivateKey(hostname, port, entity, private_key, argc, argv
                             , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port, void_, void_
                           , std::string entity, CORBA::OctetSeq private_key, void_, void_, void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPrivateKey(hostname, port, entity, private_key
                               , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port, void_, void_
                           , std::string entity, void_, std::string private_key_filename, void_
                           , int& argc, char** argv
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPrivateKeyFile(hostname, port, entity, private_key_filename, argc, argv
                                   , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self
                           , std::string hostname, unsigned short port, void_, void_
                           , std::string entity, void_, std::string private_key_filename, void_, void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPrivateKeyFile(hostname, port, entity, private_key_filename
                                   , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port, void_, void_
                           , void_, void_, void_
                           , shared_auth_callback_type shared_auth_callback
                           , int& argc, char** argv
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithSharedAuth(hostname, port, shared_auth_callback, argc, argv
                               , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self
                           , std::string hostname, unsigned short port, void_, void_, void_
                           , void_, void_, void_
                           , shared_auth_callback_type shared_auth_callback
                           , void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithSharedAuth(hostname, port, shared_auth_callback
                               , login_error, register_error, fatal_error);
    }
  };
public:
#endif

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
   * \brief Adiciona oferta para ser registrada ao barramento e tenta
   *  sua oferta assincronamente
   *
   * \param component Referencia para componente a ser registrado
   * \param properties Propriedades que devem ser registradas junto
   *  com a oferta
   */
  void addOffer(scs::core::IComponent_var component, idl_or::ServicePropertySeq properties);

  /** 
   * \brief Constroi sequencia de propriedades para determinada faceta e entidade
   *
   * \param facet Nome da faceta que a oferta deve possuir
   * \param entity Nome da entidade criadora da oferta
   */
  static idl_or::ServicePropertySeq createFacetAndEntityProperty(const char* facet, const char* entity);

  /** 
   * \brief Busca oferta de determinada propriedade
   *
   * \param properties Propriedades para busca de ofertas no barramento
   * \param timeout NÃºmeo de segundos de timeout. 0 significa retornar
   *  imediatamente, -1 esperar infinitamente. O tempo de retorno da
   *  funÃ§Ã£pode demorar mais que o tempo especificado, mas a funcao nao
   *  vai continuar tentando depois que o tempo de timeout tiver passado.
   */
  idl_or::ServiceOfferDescSeq findOffers(idl_or::ServicePropertySeq properties, int timeout) const;

  /** 
   * \brief Filtra ofertas e retorna somente as que se encontram responsivas
   *
   * \param offers Ofertas a serem verificadas por responsividade
   */
  static idl_or::ServiceOfferDescSeq filterWorkingOffers(idl_or::ServiceOfferDescSeq offers);
  
  /** 
   * \brief Termina execucao do ORB e conexao com o barramento
   */
  void shutdown();

  /** 
   * \brief Espera pelo termino de execucao do ORB e prove a thread atual
   *  para uso pela API assistants
   */
  void wait();

  /** 
   * \brief Espera pelo termino de execucao do processo de login
   */
  void waitLogin();

  /** 
   * \brief Retorna o ORB utilizado pela API assistants
   */
  CORBA::ORB_var orb() const
  {
    assert(!!state);
    return state->orb;
  }

  CallerChain getCallerChain();
  void joinChain(CallerChain chain);
  void exitChain();
  CallerChain getJoinedChain();
protected:
  AssistantImpl() {}
  void InitWithPassword(std::string const& hostname, unsigned short port
                        , std::string const& username, std::string const& password
                        , int& argc, char** argv
                        , login_error_callback_type login_error
                        , register_error_callback_type register_error
                        , fatal_error_callback_type fatal_error);
  void InitWithPassword(std::string const& hostname, unsigned short port
                        , std::string const& username, std::string const& password
                        , login_error_callback_type login_error
                        , register_error_callback_type register_error
                        , fatal_error_callback_type fatal_error);
  void InitWithPrivateKey(std::string const& hostname, unsigned short port
                          , std::string const& entity, CORBA::OctetSeq const& private_key
                          , int& argc, char** argv
                          , login_error_callback_type login_error
                          , register_error_callback_type register_error
                          , fatal_error_callback_type fatal_error);
  void InitWithPrivateKey(std::string const& hostname, unsigned short port
                          , std::string const& entity, CORBA::OctetSeq const& private_key
                          , login_error_callback_type login_error
                          , register_error_callback_type register_error
                          , fatal_error_callback_type fatal_error);
  void InitWithPrivateKeyFile(std::string const& hostname, unsigned short port
                              , std::string const& entity, std::string const& private_key_file
                              , int& argc, char** argv
                              , login_error_callback_type login_error
                              , register_error_callback_type register_error
                              , fatal_error_callback_type fatal_error);
  void InitWithPrivateKeyFile(std::string const& hostname, unsigned short port
                              , std::string const& entity, std::string const& private_key_file
                              , login_error_callback_type login_error
                              , register_error_callback_type register_error
                              , fatal_error_callback_type fatal_error);
  void InitWithSharedAuth(std::string const& hostname, unsigned short port
                          , shared_auth_callback_type shared_auth_callback
                          , int& argc, char** argv
                          , login_error_callback_type login_error
                          , register_error_callback_type register_error
                          , fatal_error_callback_type fatal_error);
  void InitWithSharedAuth(std::string const& hostname, unsigned short port
                          , shared_auth_callback_type shared_auth_callback
                          , login_error_callback_type login_error
                          , register_error_callback_type register_error
                          , fatal_error_callback_type fatal_error);

  boost::shared_ptr<assistant_detail::shared_state> state;
};
#endif

struct timeout_error : std::exception
{
  const char* what() const throw()
  {
    return "timeout_error";
  }
};

/** \brief Classe Openbus com API do assistants
 *
 * A classe Openbus deve ser instanciada por um de seus dois
 * named parameters startByPassword e startByCertificate. Essa
 * instancia mantera as ofertas adicionadas a ela e um login
 * validos no barramento indicado em sua construcao enquanto
 * um erro fatal nao ocorre e/ou um shutdown nao e invocado.
 */
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

#ifndef OPENBUS_ASSISTANT_DOXYGEN
  BOOST_PARAMETER_CONSTRUCTOR
  (Assistant, (AssistantImpl), keywords::tag
   , (required
      (host, OPENBUS_ASSISTANT_STRING_LAMBDA())
      (port, (unsigned short)))
   (optional
    (username, OPENBUS_ASSISTANT_STRING_LAMBDA())
    (entity, OPENBUS_ASSISTANT_STRING_LAMBDA())
    (password, OPENBUS_ASSISTANT_STRING_LAMBDA())
    (private_key, (CORBA::OctetSeq))
    (in_out(argc), (int&))
    (in_out(argv), (char**))
    (retry_wait, (unsigned int))
    (on_login_error, (boost::function<void(std::string)>))
    (on_register_error, (boost::function<void(scs::core::IComponent_var
                                              , idl_or::ServicePropertySeq
                                              , std::string /*error*/)>))
    (on_fatal_error, (boost::function<void(const char* /*error*/)>))
   )
  )
#else
  Assistant(NamedArguments);
#endif
#endif

  /** \brief Constroi um Openbus com informacao de autenticacao
   *   por usuario e senha
   */
  static Assistant createWithPassword(const char* username, const char* password
                                      , const char* host, unsigned short port
                                      , int& argc, char** argv
                                      , login_error_callback_type login_error = login_error_callback_type()
                                      , register_error_callback_type register_error
                                         = register_error_callback_type()
                                      , fatal_error_callback_type fatal_error = fatal_error_callback_type());

  /** \brief Constriu um Openbus com informacao de autenticacao
   *  por certificado
   */
  static Assistant createWithPrivateKey(const char* entity, const idl::OctetSeq privKey
                                        , const char* host, unsigned short port
                                        , int& argc, char** argv
                                        , login_error_callback_type login_error = login_error_callback_type()
                                        , register_error_callback_type register_error
                                          = register_error_callback_type()
                                        , fatal_error_callback_type fatal_error = fatal_error_callback_type());

  /** \brief Constriu um Openbus com informacao de autenticacao
   *  por Shared Authentication
   */
  static Assistant createWithSharedAuth(shared_auth_callback_type shared_auth_callback
                                        , const char* host, unsigned short port
                                        , int& argc, char** argv
                                        , login_error_callback_type login_error = login_error_callback_type()
                                        , register_error_callback_type register_error
                                          = register_error_callback_type()
                                        , fatal_error_callback_type fatal_error = fatal_error_callback_type());
private:
#ifndef OPENBUS_ASSISTANT_DISABLE_NAMED_PARAMETERS
  Assistant() {}
#endif
};

} }

#endif
