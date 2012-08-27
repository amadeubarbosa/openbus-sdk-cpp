// -*- coding: iso-8859-1 -*-

#ifndef OPENBUS_ASSISTANT_OPENBUS_H
#define OPENBUS_ASSISTANT_OPENBUS_H

#define BOOST_PARAMETER_MAX_ARITY 12

#include <openbus/assistant/reference.h>

#ifdef ASSISTANT_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

#include <scs/IComponent.h>
#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <CORBA.h>

#include <boost/parameter.hpp>
#include <boost/preprocessor/iteration/iterate.hpp>
#include <boost/preprocessor/repetition/enum_trailing_params.hpp>
#include <boost/preprocessor/repetition/enum_trailing_binary_params.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>
#include <boost/preprocessor/arithmetic/dec.hpp>

#include <boost/variant.hpp>

#include <log/logger.h>

namespace openbus { namespace assistant {

#ifndef OPENBUS_ASSISTANT_DOXYGEN
namespace assistant_detail {

struct password_authentication_info
{
  std::string username, password;
};

struct certificate_authentication_info
{
  std::string entity;
  idl::OctetSeq private_key;
};

typedef boost::variant<password_authentication_info
                       , certificate_authentication_info>
  authentication_info;

struct shared_state
{
  typedef boost::function<void(std::string)> login_error_callback_type;
  typedef boost::function<void(scs::core::IComponent_var
                               , idl_or::ServicePropertySeq
                               , std::string)> register_error_callback_type;
  typedef boost::function<void(const char*)> fatal_error_callback_type;

  // connection_ready = true ==> (connection.get() != 0
  //  /\ connection.get() is never modified again)
  // attribution to connection 'happens before' connection_ready = true
  // so that if connection_ready is true, we don't need to lock any mutexes
  // to read connection member variable

  logger::logger logging;
  CORBA::ORB_var const orb;
  assistant_detail::authentication_info const auth_info;
  std::string const host;
  unsigned short const port;
  login_error_callback_type login_error;
  register_error_callback_type register_error;
  fatal_error_callback_type fatal_error;
  std::auto_ptr<openbus::Connection> connection;
  std::vector<std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> > components;
  std::vector<std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> > queued_components;
#ifdef ASSISTANT_SDK_MULTITHREAD
  bool new_queued_components;
  bool work_exit;
  boost::thread work_thread;
  boost::thread orb_thread;
  boost::mutex mutex;
  // work_cond_var is notified for exiting the work thread and to
  // consume queued components for registration
  boost::condition_variable work_cond_var;
  boost::condition_variable connection_ready_var;
  bool connection_ready;
#endif

  shared_state(CORBA::ORB_var orb, authentication_info auth_info
               , std::string const& host, unsigned short port
               , login_error_callback_type login_error
               , register_error_callback_type register_error
               , fatal_error_callback_type fatal_error)
    : orb(orb), auth_info(auth_info), host(host), port(port)
    , new_queued_components(false), work_exit(false)
    , login_error(login_error), register_error(register_error)
    , fatal_error(fatal_error) {}
};

namespace idl = tecgraf::openbus::core::v2_0;
namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace idl_cr = tecgraf::openbus::core::v2_0::credential;

}

namespace keywords {
BOOST_PARAMETER_NAME(host);
BOOST_PARAMETER_NAME(port);
BOOST_PARAMETER_NAME(username);
BOOST_PARAMETER_NAME(entity);
BOOST_PARAMETER_NAME(password);
BOOST_PARAMETER_NAME(private_key);
BOOST_PARAMETER_NAME(private_key_filename);
BOOST_PARAMETER_NAME(argc);
BOOST_PARAMETER_NAME(argv);
BOOST_PARAMETER_NAME(on_login_error);
BOOST_PARAMETER_NAME(on_register_error);
BOOST_PARAMETER_NAME(on_fatal_error);
}

using namespace keywords;

struct AssistantImpl
{
  typedef boost::function<void(std::string)> login_error_callback_type;
  typedef boost::function<void(scs::core::IComponent_var
                               , idl_or::ServicePropertySeq
                               , std::string)> register_error_callback_type;
  typedef boost::function<void(const char*)> fatal_error_callback_type;

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

    typedef mpl::and_<has_argc, has_argv> has_args;
    typedef mpl::or_<has_password_credential, has_certificate_credential> has_credential;
    typedef mpl::and_<has_password_credential, has_certificate_credential> has_certificate_and_password_credentials;

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
    // dos dois:
    // openbus::assistant::Assistant assistant
    //  ([...], _username = "myusername", _password = "mypassword");
    // ou:
    // openbus::assistant::Assistant assistant
    //  ([...], _entity = "entidade do serviço", _private_key = private_key_octet_seq);
    BOOST_MPL_ASSERT((has_credential));

    // Se a compilação falhar nesta asserção, então seu código passou credenciais
    // de ambas as formas. Você deve escolher entre _username/_password e
    // _entity/_private_key
    BOOST_MPL_ASSERT((mpl::not_<has_certificate_and_password_credentials>));

    typedef mpl::void_ void_;
    init_function()
      (this, args[_host]
       , args[_port]
       , args[_username | void_()]
       , args[_password | void_()]
       , args[_entity | void_()]
       , args[_private_key | void_()]
       , args[_private_key_filename | void_()]
       , args[_argc | void_()]
       , args[_argv | void_()]
       , args[_on_login_error | login_error_callback_type()]
       , args[_on_register_error | register_error_callback_type()]
       , args[_on_fatal_error | fatal_error_callback_type()]
       );
  }

  typedef boost::mpl::void_ void_;
  struct init_function
  {
    typedef void result_type;
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port
                           , std::string username, std::string password, void_, void_, void_
                           , int& argc, char** argv
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPassword(hostname, port, username, password, argc, argv
                             , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port
                           , std::string username, std::string password, void_, void_, void_, void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPassword(hostname, port, username, password
                             , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self
                           , std::string hostname, unsigned short port, void_, void_
                           , std::string entity, CORBA::OctetSeq private_key, void_
                           , int& argc, char** argv
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPrivateKey(hostname, port, entity, private_key, argc, argv
                             , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port, void_, void_
                           , std::string entity, CORBA::OctetSeq private_key, void_, void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPrivateKey(hostname, port, entity, private_key
                               , login_error, register_error, fatal_error);
    }
    result_type operator()(AssistantImpl* self, std::string hostname, unsigned short port, void_, void_
                           , std::string entity, void_, std::string private_key_filename
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
                           , std::string entity, void_, std::string private_key_filename, void_, void_
                           , login_error_callback_type login_error
                           , register_error_callback_type register_error
                           , fatal_error_callback_type fatal_error) const
    {
      self->InitWithPrivateKeyFile(hostname, port, entity, private_key_filename
                                   , login_error, register_error, fatal_error);
    }
  };

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

  /** 
   * \brief Atribui uma funcao callback para erros de login
   */
  void onLoginError(boost::function<void(std::string /*error*/)> f)
  {
    assert(!!state);
    boost::unique_lock<boost::mutex> lock(state->mutex);
    state->login_error = f;
  }

  /** 
   * \brief Atribui uma funcao callback para erros de registro
   *  de ofertas
   */
  void onRegisterError(boost::function<void(scs::core::IComponent_var
                                            , idl_or::ServicePropertySeq
                                            , std::string /*error*/)> f)
  {
    assert(!!state);
    boost::unique_lock<boost::mutex> lock(state->mutex);
    state->register_error = f;
  }

  /** 
   * \brief Atribui uma funcao callback para erros fatais
   *  que impossibilitam a continuacao da execucao
   *  do assistant
   */
  void onFatalError(boost::function<void(const char* /*error*/)> f)
  {
    assert(!!state);
    boost::unique_lock<boost::mutex> lock(state->mutex);
    state->fatal_error = f;
  }

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
private:
  boost::shared_ptr<assistant_detail::shared_state> state;
};
#endif

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

  // /** \brief Constroi um Openbus com informacao de autenticacao
  //  *   por usuario e senha
  //  */
  // static Assistant startByPassword(const char* username, const char* password
  //                                  , const char* host, unsigned short port
  //                                  , int argc, const char** argv);

  // /** \brief Constriu um Openbus com informacao de autenticacao
  //  *  por certificado
  //  */
  // static Assistant startByCertificate(const char* entity, const idl::OctetSeq privKey
  //                                     , const char* host, unsigned short port
  //                                     , int argc, const char** argv);

};

} }

#endif
