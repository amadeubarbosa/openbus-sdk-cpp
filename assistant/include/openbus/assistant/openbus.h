#ifndef OPENBUS_ASSISTANT_OPENBUS_H
#define OPENBUS_ASSISTANT_OPENBUS_H

#include <openbus/assistant/reference.h>

#ifdef ASSISTANT_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

#include <scs/IComponent.h>
#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <CORBA.h>

#include <boost/variant.hpp>

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
  CORBA::ORB_var const orb;
  assistant_detail::authentication_info const auth_info;
  std::string const host;
  unsigned short const port;
  bool new_queued_components;
  bool work_exit;
  boost::function<void(std::string /*error*/)> login_error;
  boost::function<void(scs::core::IComponent_var
                       , idl_or::ServicePropertySeq
                       , std::string /*error*/)> register_error;
  boost::function<void(const char* /*error*/)> fatal_error;
  std::auto_ptr<openbus::Connection> connection;
  std::vector<std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> > components;
  std::vector<std::pair<scs::core::IComponent_var, idl_or::ServicePropertySeq> > queued_components;
#ifdef ASSISTANT_SDK_MULTITHREAD
  boost::thread work_thread;
  boost::thread orb_thread;
  boost::mutex mutex;
  boost::condition_variable work_cond_var;
#endif

  shared_state(CORBA::ORB_var orb, authentication_info auth_info
               , const char* host, unsigned short port)
    : orb(orb), auth_info(auth_info), host(host), port(port)
    , new_queued_components(false), work_exit(false) {}
  ~shared_state();
};

namespace idl = tecgraf::openbus::core::v2_0;
namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace idl_cr = tecgraf::openbus::core::v2_0::credential;

}
#endif

/** \brief Classe Openbus com API do assistants
 *
 * A classe Openbus deve ser instanciada por um de seus dois
 * named parameters startByPassword e startByCertificate. Essa
 * instancia mantera as ofertas adicionadas a ela e um login
 * validos no barramento indicado em sua construcao enquanto
 * um erro fatal nao ocorre e/ou um shutdown nao e invocado.
 */
struct Openbus
{
  /** \brief Constroi um Openbus com informacao de autenticacao
   *   por usuario e senha
   */
  static Openbus startByPassword(const char* username, const char* password
                                 , const char* host, unsigned short port
                                 , int argc, const char** argv);

  /** \brief Constriu um Openbus com informacao de autenticacao
   *  por certificado
   */
  static Openbus startByCertificate(const char* entity, const idl::OctetSeq privKey
                                    , const char* host, unsigned short port
                                    , int argc, const char** argv);

  /** 
   * \brief Faz copia de um Openbus
   */
  Openbus(Openbus const& other);

  /** 
   * \brief Operador de atribuicao
   */
  Openbus& operator=(Openbus const& other);

  /** 
   * \brief Destrutor
   */
  ~Openbus();

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
   * \param timeout Númeo de segundos de timeout. 0 significa retornar
   *  imediatamente, -1 esperar infinitamente. O tempo de retorno da
   *  funçãpode demorar mais que o tempo especificado, mas a funcao nao
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
  Openbus(CORBA::ORB_var orb, const char* host, unsigned short port
          , assistant_detail::authentication_info info);

  boost::shared_ptr<assistant_detail::shared_state> state;
};

} }

#endif
