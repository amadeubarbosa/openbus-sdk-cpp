// -*- coding: iso-8859-1-unix -*-
/**
* API - SDK Openbus C++
* \file openbus/OpenBusContext.hpp
*/

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_CONTEXT_H_
#define TECGRAF_SDK_OPENBUS_OPENBUS_CONTEXT_H_

#include "openbus/ORBInitializer.hpp"
#include "openbus/decl.hpp"
#include "openbus/Connection.hpp"
#include "credentialC.h"
#include "access_controlC.h"
#include "offer_registryC.h"
#include "data_exportC.h"

#include <tao/LocalObject.h>

#include <boost/function.hpp>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/shared_ptr.hpp>
#include <string>

namespace openbus 
{
  namespace idl_ac = tecgraf::openbus::core::v2_0::services::access_control;
  namespace idl_cr = tecgraf::openbus::core::v2_0::credential;
  namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;
  namespace idl_data_export = tecgraf::openbus::core::v2_0::data_export;

  namespace interceptors
  {
    struct orb_info;
    struct ClientInterceptor;
  }
}

namespace tecgraf 
{ 
namespace openbus 
{ 
namespace core 
{ 
namespace v2_0 
{ 
namespace services 
{ 
namespace access_control 
{

inline bool operator==(const LoginInfo &lhs, const LoginInfo &rhs)
{
  return lhs.id.in() == rhs.id.in() 
    || (lhs.id.in() && rhs.id.in() && !std::strcmp(lhs.id.in(), rhs.id.in()));
}

inline bool operator!=(const LoginInfo &lhs, const LoginInfo &rhs)
{
  return !(lhs == rhs);
}

inline bool operator==(const LoginInfoSeq &lhs, const LoginInfoSeq &rhs)
{
  if (lhs.length() != rhs.length())
  {
    return false;
  }
  for (std::size_t i = 0; i < rhs.length(); ++i)
  {
    if (lhs[i] != rhs[i])
    {
      return false;
    }
  }
  return true;
}

inline bool operator!=(const LoginInfoSeq &lhs, const LoginInfoSeq &rhs)
{
  return !(lhs == rhs);
}

}}}}}}

/**
* \brief openbus
*/
namespace openbus 
{

struct OPENBUS_SDK_DECL InvalidEncodedStream : public std::exception
{
  InvalidEncodedStream();
  InvalidEncodedStream(const std::string &msg);
  ~InvalidEncodedStream() throw();
  const char *what() const throw()
  {
    return msg_.c_str();
  }
private:
  std::string msg_;
};
  
/**
 * \brief Cadeia de chamadas oriundas de um barramento.
 * 
 * Coleção de informações dos logins que originaram chamadas em cadeia
 * através de um barramento. Cadeias de chamadas representam chamadas
 * aninhadas dentro do barramento e são úteis para que os sistemas que
 * recebam essas chamadas possam identificar se a chamada foi
 * originada por entidades autorizadas ou não.
 */
struct OPENBUS_SDK_DECL CallerChain 
{
  /**
  * Barramento através do qual as chamadas foram originadas.
  */
  const std::string busid() const 
  {
    return _busid;
  }

	/**
   * Entidade para a qual a chamada estava destinada. Só é possível fazer 
   * chamadas dentro dessa cadeia (através do método joinChain da interface 
   * OpenBusContext) se a entidade da conexão corrente for o mesmo do target.
   *
   * No caso de conexões legadas, este campo armazenará o nome da entidade 
   * da conexão que atendeu a requisição. Todas as chamadas feitas como 
   * parte de uma cadeia de uma chamada legada serão feitas utilizando 
   * apenas o protocolo do OpenBus 1.5 (apenas com credenciais legadas) 
   * e portanto serão recusadas por serviços que não aceitem chamadas
   * legadas (OpenBus 1.5).
   */
  const std::string target() const
  {
    return _target;
  }
  
	/**
	 * Lista de informações de login de todas as entidades que originaram as
	 * chamadas nessa cadeia. Quando essa lista é vazia isso indica que a
	 * chamada não está inclusa em outra cadeia de chamadas.
	 */
  const idl_ac::LoginInfoSeq &originators() const 
  {
    return _originators;
  }
  
  /**
   * Informação de login da entidade que realizou a última chamada da cadeia.
   */
  const idl_ac::LoginInfo &caller() const 
  {
    return _caller;
  }

  /**
   * \brief Construtor default que indica o valor de CallChain "vazio"
   *
   * O valor de um CallerChain default-constructed pode ser usado para
   * verificar a ausencia de CallerChain da seguinte forma:
   * 
   * CallerChain chain = openbusContext.getCallerChain();
   * if(chain != CallerChain())
   *   // Possui CallerChain
   * else
   *   // Nao possui CallerChain
   *
   */
  CallerChain() 
  {
  }
//private:
#ifndef OPENBUS_SDK_TEST
private:
#else
public:
#endif
  CallerChain(const std::string &busid, 
              const std::string &target,
              const idl_ac::LoginInfoSeq &originators, 
              const idl_ac::LoginInfo &caller,
              const idl_cr::SignedCallChain &chain) 
    : _busid(busid), _target(target), _originators(originators), 
      _caller(caller), _signedCallChain(chain)
  {
  }
  
  CallerChain(const std::string &busid, 
              const std::string &target,
              const idl_ac::LoginInfoSeq &originators, 
              const idl_ac::LoginInfo &caller) 
    : _busid(busid), _target(target), _originators(originators), 
    _caller(caller) 
  {
  }
  
  std::string _busid;
  std::string _target;
  idl_ac::LoginInfoSeq _originators;
  idl_ac::LoginInfo _caller;
  idl_cr::SignedCallChain _signedCallChain;
  
  bool is_legacy() const;
  
  friend class OpenBusContext;
  friend struct openbus::interceptors::ClientInterceptor;
  friend inline bool operator==(CallerChain const &lhs, 
                                CallerChain const &rhs) 
  {
    return lhs._busid == rhs._busid && lhs._originators == rhs._originators
      && lhs._caller == rhs._caller;
  }
};

inline bool operator!=(CallerChain const &lhs, CallerChain const &rhs) 
{
  return !(lhs == rhs);
}

class OPENBUS_SDK_DECL OpenBusContext : public CORBA::LocalObject 
{
public:
  /**
	 * \brief Callback de despacho de chamadas.
	 * 
	 * Método a ser implementado pelo objeto de callback a ser chamado quando
	 * uma chamada proveniente de um barramento é recebida. Esse método é chamado
	 * para determinar a conexão a ser utilizada na validação de cada chamada
	 * recebida. Se a conexão informada não estiver conectada ao mesmo barramento
	 * indicado pelo parâmetro 'busid', a chamada provavelmente será recusada com
	 * um CORBA::NO_PERMISSION{InvalidLogin} pelo fato do login provavelmente não
	 * ser válido no barramento da conexão. Como resultado disso o cliente da
	 * chamada poderá indicar que o servidor não está implementado corretamente e
	 * lançar a exceção CORBA::NO_PERMISSION{InvalidRemote}. Caso alguma exceção
	 * ocorra durante a execução do método e não seja tratada, o erro será
	 * capturado pelo interceptador e registrado no log.
	 * 
	 * @param[in] context Gerenciador de contexto do ORB que recebeu a chamada.
	 * @param[in] busid Identificação do barramento através do qual a chamada foi
	 *                  feita.
	 * @param[in] loginId Informações do login do cliente da chamada.
	 * @param[in] operation Nome da operação sendo chamada.
	 *
	 * @return Conexão a ser utilizada para receber a chamada.
	 */
  typedef boost::function<
    Connection* (OpenBusContext &context,
                 const std::string busId, 
                 const std::string loginId,
                 const std::string operation)>
    CallDispatchCallback;

  /**
	 * \brief Define a callback a ser chamada para determinar a conexão
	 *        a ser utilizada para receber cada chamada.
	 *
	 * Esse atributo é utilizado para definir um objeto que implementa uma
	 * interface de callback a ser chamada sempre que a conexão receber uma do
	 * barramento. Essa callback deve devolver a conexão a ser utilizada para
	 * receber a chamada. A conexão utilizada para receber a chamada será
	 * a única conexão através da qual novas chamadas aninhadas à chamada
	 * recebida poderão ser feitas (veja a operação 'joinChain').
	 *
	 * Se o objeto de callback for definido como 'null' ou devolver 'null', a
	 * conexão padrão é utilizada para receber a chamada, caso esta esteja
	 * definida.
	 *
	 * Caso esse atributo seja 'null', nenhum objeto de callback é chamado na
	 * ocorrência desse evento e 
	 */
  void onCallDispatch(CallDispatchCallback c);

  /**
   * \brief Retorna a callback a ser chamada para determinar a conexão
   *        a ser utilizada para receber cada chamada
   */
  CallDispatchCallback onCallDispatch() const;

  /**
   * \brief Cria uma conexão para um barramento.
   * 
   * Cria uma conexão para um barramento. O barramento é indicado por
   * um nome ou endereço de rede e um número de porta, onde os
   * serviços núcleo daquele barramento estão executando.
   * 
   * @param[in] host Endereço ou nome de rede onde os serviços núcleo do 
   *            barramento estao executando.
   * @param[in] port Porta onde os serviços núcleo do barramento estão 
   *            executando.
   * @param[in] props Lista opcional de propriedades que definem algumas
   *        configurações sobre a forma que as chamadas realizadas ou validadas
   *        com essa conexão são feitas. A seguir são listadas as propriedades
   *        válidas:
   *        - legacy.disable: desabilita o suporte a chamadas usando protocolo
   *          OpenBus 1.5. Por padrão o suporte está habilitado.
   *        - legacy.delegate: indica como é preenchido o campo 'delegate' das
   *          credenciais enviadas em chamadas usando protocolo OpenBus 1.5. Há
   *          duas formas possíveis (o padrão é 'caller'):
   *          - caller: o campo 'delegate' é preenchido sempre com a entidade
   *            do campo 'caller' da cadeia de chamadas.
   *          - originator: o campo 'delegate' é preenchido sempre com a
   *            entidade que originou a cadeia de chamadas, que é o primeiro
   *            login do campo 'originators' ou o campo 'caller' quando este
   *            é vazio.
   *   
   * @throw InvalidPropertyValue O valor de uma propriedade não é válido.
   * @throw CORBA::Exception
   *
   * @return Conexão criada.
   */
  std::auto_ptr<Connection> createConnection(
    const std::string host, unsigned short port, 
    const Connection::ConnectionProperties &props = 
    Connection::ConnectionProperties());
   
  /**
   * \brief Define a conexão padrão a ser usada nas chamadas.
   * 
   * Define uma conexão a ser utilizada como "Requester" e
   * "Dispatcher" de chamadas sempre que não houver uma conexão
   * "Requester" e "Dispatcher" específica definida para o caso
   * específico, como é feito através das operações
   * 'setCurrentConnection' e 'setDispatcher'.
   * 
   * @param[in] conn Conexão a ser definida como conexão padrão. O
   * 'ownership' da conexão não é transferida para o OpenBusContext, e
   * a conexão deve ser removida do OpenBusContext antes de destruida
   */
  Connection *setDefaultConnection(Connection *);
   
  /**
   * \brief Devolve a conexão padrão.
   * 
   * Veja operação 'setDefaultConnection'.
   * 
   * \return Conexão definida como conexão padrão. OpenBusContext não
   * possui ownership dessa conexão e o mesmo não é transferido para o
   * código de usuário na execução desta função
   */
  Connection *getDefaultConnection() const;
   
  /**
   * \brief Define a conexão "Requester" do contexto corrente.
   * 
   * Define a conexão "Requester" a ser utilizada em todas as chamadas
   * feitas no contexto atual. Quando 'conn' é 'null' o contexto passa
   * a ficar sem nenhuma conexão associada.
   * 
   * @param[in] conn Conexão a ser associada ao contexto corrente. O
   * 'ownership' da conexão não é transferida para o OpenBusContext, e
   * a conexão deve ser removida do OpenBusContext antes de destruida
   */
  Connection *setCurrentConnection(Connection *);
   
  /**
   * \brief Devolve a conexão associada ao contexto corrente.
   * 
   * @throw CORBA::Exception
   *
   * @return Conexão a barramento associada a thread
   * corrente. OpenBusContext não possui ownership dessa conexão e o
   * mesmo não é transferido para o código de usuário na execução
   * desta função
   */
  Connection *getCurrentConnection() const;
       
  /**
   * \brief Devolve a cadeia de chamadas à qual a execução corrente pertence.
   * 
   * Caso a contexto corrente (e.g. definido pelo 'CORBA::PICurrent')
   * seja o contexto de execução de uma chamada remota oriunda do
   * barramento dessa conexão, essa operação devolve um objeto que
   * representa a cadeia de chamadas do barramento que esta chamada
   * faz parte. Caso contrário, devolve uma cadeia de chamadas
   * 'vazia', representada por um CallerChain 'default-constructed'.
   *
   * Para verificar se a cadeia retornada é válida, o seguinte idioma
   * é usado:
   *
   * CallerChain chain = connection.getCallerChain()
   * if(chain != CallerChain())
   *   // chain é válido
   * else
   *   // chain é inválido
   * 
   * \return Cadeia da chamada em execução.
   */
  CallerChain getCallerChain();
  
  /**
   * \brief Associa uma cadeia de chamadas ao contexto corrente.
   * 
   * Associa uma cadeia de chamadas ao contexto corrente, de forma que
   * todas as chamadas remotas seguintes neste mesmo contexto sejam
   * feitas como parte dessa cadeia de chamadas.
   * 
   * \param chain Cadeia de chamadas a ser associada ao contexto
   * corrente. A ausência de valor ou uma cadeia 'vazia' implica na
   * utilização da cadeia obtida através de getCallerChain.
   *
   * @throw
   * CORBA::NO_PERMISSION {minor = NoLoginCode} @throw
   * CORBA::NO_PERMISSION {minor = InvalidChainCode} @throw
   * CORBA::Exception
   */
  void joinChain(const CallerChain &chain = CallerChain());
  
  /**
   * \brief Faz com que nenhuma cadeia de chamadas esteja associada ao
   * contexto corrente.
   * 
   * Remove a associação da cadeia de chamadas ao contexto corrente,
   * fazendo com que todas as chamadas seguintes feitas neste mesmo
   * contexto deixem de fazer parte da cadeia de chamadas associada
   * previamente. Ou seja, todas as chamadas passam a iniciar novas
   * cadeias de chamada.
   */
  void exitChain();

  /**
   * \brief Devolve a cadeia de chamadas associada ao contexto corrente.
   * 
   * Devolve um objeto que representa a cadeia de chamadas associada
   * ao contexto corrente nesta conexão.  A cadeia de chamadas
   * informada foi associada previamente pela operação
   * 'joinChain'. Caso o contexto corrente não tenha nenhuma cadeia
   * associada, essa operação devolve uma cadeia 'vazia'
   * 'default-constructed'
   * 
   * Para verificar se a cadeia retornada é válida, o seguinte idioma é usado:
   *
   * CallerChain chain = openbusContext.getCallerChain()
   * if(chain != CallerChain())
   *   // chain é válido
   * else
   *   // chain é inválido
   * 
   * \return Cadeia de chamadas associada ao contexto corrente ou uma
   * cadeia 'vazia'.
   *
   * @throw CORBA::Exception
   */
  CallerChain getJoinedChain() const;

  /**
   * \brief Cria uma cadeia de chamadas para a entidade com o identificador de
   *        login especificado.
   * 
   * Cria uma nova cadeia de chamadas para a entidade especificada,
   * onde o dono da cadeia é a conexão corrente
   * (OpenBusContext::getCurrentConnection()) e utiliza-se a cadeia
   * atual (OpenBusContext::getJoinedChain()) como a cadeia que se
   * deseja dar seguimento ao encadeamento. O identificador de login
   * especificado deve ser um login atualmente válido para que a
   * operação tenha sucesso.Caso o contexto corrente não tenha nenhuma
   * cadeia associada, essa operação devolve uma cadeia 'vazia'
   * 'default-constructed'.
   *
   * Para verificar se a cadeia retornada é válida, o seguinte idioma é usado:
   *
   * CallerChain chain = openbusContext.makeChainFor(loginId);
   * if(chain != CallerChain())
   *   // chain é válido
   * else
   *   // chain é inválido
   * 
   * \param loginId identificador de login da entidade para a qual deseja-se
   *        enviar a cadeia.
   * \return a cadeia gerada para ser utilizada pela entidade com o login
   *         especificado.
   * 
   * \exception InvalidLogins Caso o login especificado seja inválido.
   * \exception ServiceFailure Ocorreu uma falha interna nos serviços do 
   *            barramento que impediu a criação da cadeia.
   */
  CallerChain makeChainFor(const std::string &loginId) const;

  /**
   * \brief Codifica uma cadeia de chamadas (CallerChain) para um stream de
   * bytes.
   * 
   * Codifica uma cadeia de chamadas em um stream de bytes para permitir a
   * persistência ou transferência da informação. A codificação é realizada em
   * CDR e possui um identificador de versão concatenado com as informações da
   * cadeia. Sendo assim, a stream só será decodificada com sucesso por alguém
   * que entenda esta mesma codificação.
   * 
   * \param chain A cadeia a ser codificada.
   * \return A cadeia codificada em um stream de bytes.
   */
  CORBA::OctetSeq encodeChain(const CallerChain chain);

  /**
   * \brief Decodifica um stream de bytes de uma cadeia para o formato
   * CallerChain.
   * 
   * Decodifica um stream de bytes de uma cadeia para o formato CallerChain.
   * Espera-se que a stream de bytes esteja codificada em CDR e seja formada por
   * um identificador de versão concatenado com as informações da cadeia. Caso
   * não seja possível decodificar a sequência de octetos passada, essa operação
   * devolve uma cadeia 'vazia' 'default-constructed'.
   *
   * Para verificar se a cadeia retornada é válida, o seguinte idioma é usado:
   *
   * CallerChain chain = openbusContext.decodeChain(encoded);
   * if(chain != CallerChain())
   *   // chain é válido
   * else
   *   // chain é inválido
   * 
   * \param encoded O stream de bytes que representa a cadeia.
   * \return A cadeia de chamadas no formato CallerChain, 'vazia' em caso de
   * falha da decodificação.
   * 
   * \throw InvalidEncodedStream Caso o stream de bytes não seja do
   * formato esperado.
   */
  CallerChain decodeChain(const CORBA::OctetSeq &encoded) const;
  
  /**
	 * \brief Codifica um segredo de autenticação compartilhada
   *        SharedAuthSecret para um stream de bytes.
   * 
   * Codifica um segredo de autenticação compartilhada em um stream de bytes
   * para permitir a persistência ou transferência da informação.
   * 
   * \param secret Segredo de autenticação compartilhada a ser codificado.
   * \return Cadeia codificada em um stream de bytes.
   */
  CORBA::OctetSeq encodeSharedAuthSecret(const SharedAuthSecret &secret);

  /**
   * \brief Decodifica um segredo de autenticação compartilhada
   *				SharedAuthSecret a partir de um stream de bytes.
   * 
   * Decodifica um segredo de autenticação compartilhada a partir de um stream
   * de bytes.
   * 
   * \param encoded Stream de bytes contendo a codificação do segredo.
   * \return Segredo de autenticação compartilhada decodificado.
   * \exception InvalidEncodedStream Caso a stream de bytes não seja do formato
   *						 esperado.
   */
  SharedAuthSecret decodeSharedAuthSecret(const CORBA::OctetSeq &encoded);

  /** 
   * ORB utilizado pela conexão. 
   */
  CORBA::ORB_ptr orb() const 
  {
    return _orb;
  }
  
  idl_or::OfferRegistry_ptr getOfferRegistry() const;
  idl_ac::LoginRegistry_ptr getLoginRegistry() const;
private:
  /**
   * OpenBusContext deve ser adquirido atraves de:
   *   orb->resolve_initial_references("OpenBusContext")
   */
  OpenBusContext(CORBA::ORB_ptr, boost::shared_ptr<interceptors::orb_info>);
  
  OpenBusContext(const OpenBusContext&);
  OpenBusContext &operator=(const OpenBusContext &);

  void orb(CORBA::ORB_ptr o) 
  {
    _orb = o;
  }

  CORBA::OctetSeq encode_exported_versions(idl_data_export::ExportedVersionSeq,
                                           const std::string &tag);

  std::string decode_exported_versions(
    const CORBA::OctetSeq &stream,
    idl_data_export::ExportedVersionSeq_out exported_version_seq) const;

  typedef std::map<std::string, Connection *> BusidConnection;
#ifdef OPENBUS_SDK_MULTITHREAD
  mutable boost::mutex _mutex;
#endif
  CORBA::ORB_ptr _orb;
  boost::shared_ptr<interceptors::orb_info> _orb_info;
  ::IOP::Codec_var _codec;
  PortableInterceptor::Current_var _piCurrent;
  Connection *_defaultConnection;
  BusidConnection _busidConnection;
  CallDispatchCallback _callDispatchCallback;

  friend CORBA::ORB_ptr openbus::ORBInitializer(int &argc, char **argv);
};
}

#endif

