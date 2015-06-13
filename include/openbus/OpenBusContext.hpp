// -*- coding: iso-8859-1-unix -*-
/**
* API do OpenBus SDK C++
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
  namespace idl_ac = tecgraf::openbus::core::v2_1::services::access_control;
  namespace idl_cr = tecgraf::openbus::core::v2_1::credential;
  namespace idl_or = tecgraf::openbus::core::v2_1::services::offer_registry;
  namespace idl_data_export = tecgraf::openbus::core::v2_1::data_export;

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
namespace v2_1 
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
  for (CORBA::ULong i(0); i < rhs.length(); ++i)
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
  * \brief Barramento através do qual as chamadas foram originadas.
  */
  const std::string busid() const 
  {
    return _busid;
  }

	/**
   * \brief Entidade para a qual a chamada estava destinada. 
   *
   * Só é possível fazer 
   * chamadas dentro dessa cadeia através do método 
   * \ref OpenBusContext::joinChain se a entidade da conexão 
   * corrente for a mesmo do target.
   *
   */
  const std::string target() const
  {
    return _target;
  }
  
	/**
	 * \brief Lista de informações de login de todas as entidades que originaram as
	 * chamadas nessa cadeia. 
   *
   * Quando essa lista é vazia isso indica que a  chamada não está inclusa em 
   * outra cadeia de chamadas.
	 */
  const idl_ac::LoginInfoSeq &originators() const 
  {
    return _originators;
  }
  
  /**
   * \brief Informação de login da entidade que realizou a última chamada da 
   * cadeia.
   */
  const idl_ac::LoginInfo &caller() const 
  {
    return _caller;
  }

  /**
   * \brief Construtor default que indica há ausência de uma cadeia.
   *
   * O valor de um CallerChain default-constructed pode ser usado para
   * verificar a ausência de uma cadeia da seguinte forma:
   * \code
   * CallerChain chain(openbusContext.getCallerChain());
   * if(chain != CallerChain())
   *   // Possui CallerChain
   * else
   *   // Nao possui CallerChain
   * \endcode
   */
  CallerChain() 
  {
    std::memset(_signedCallChain.signature, ' ', idl::EncryptedBlockSize);
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
              const idl_cr::SignedData &chain) 
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
    std::memset(_signedCallChain.signature, ' ', idl::EncryptedBlockSize);
  }
  
  std::string _busid;
  std::string _target;
  idl_ac::LoginInfoSeq _originators;
  idl_ac::LoginInfo _caller;
  idl_cr::SignedData _signedCallChain;
  
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


/**
 * \class OpenBusContext
 * \brief Permite controlar o contexto das chamadas de um ORB para acessar
 *        informações que identificam essas chamadas em barramentos OpenBus.
 *
 * O contexto de uma chamada pode ser definido pela linha de execução atual
 * do programa em que executa uma chamada, o que pode ser a thread em execução
 * ou mais comumente o 'CORBA::PICurrent' do padrão CORBA. As informações
 * acessíveis através do 'OpenBusContext' se referem basicamente à
 * identificação da origem das chamadas, ou seja, nome das entidades que
 * autenticaram os acessos ao barramento que originaram as chamadas.
 * 
 * A identifcação de chamadas no barramento é controlada através do
 * OpenBusContext através da manipulação de duas abstrações representadas
 * pelas seguintes interfaces:
 * - Connection: Representa um acesso ao barramento, que é usado tanto para
 *   fazer chamadas como para receber chamadas através do barramento. Para
 *   tanto a conexão precisa estar autenticada, ou seja, logada. Cada chamada
 *   feita através do ORB é enviada com as informações do login da conexão
 *   associada ao contexto em que a chamada foi realizada. Cada chamada
 *   recebida também deve vir através de uma conexão logada, que deve ser o
 *   mesmo login com que chamadas aninhadas a essa chamada original devem ser
 *   feitas.
 * - CallChain: Representa a identicação de todos os acessos ao barramento que
 *   originaram uma chamada recebida. Sempre que uma chamada é recebida e
 *   executada, é possível obter um CallChain através do qual é possível
 *   inspecionar as informações de acesso que originaram a chamada recebida.
 */
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
	 * Definição de um objeto que implementa uma interface de callback a
	 * ser chamada sempre que a conexão receber uma chamada do
	 * barramento. Essa callback deve devolver a conexão a ser utilizada
	 * para receber a chamada. A conexão utilizada para receber a
	 * chamada será a única conexão através da qual novas chamadas
	 * aninhadas à chamada recebida poderão ser feitas (veja a operação
	 * 'joinChain').
	 *
	 * Se o objeto de callback for definido como 'null' ou devolver 'null', a
	 * conexão padrão é utilizada para receber a chamada, caso esta esteja
	 * definida.
	 *
	 * Caso esse atributo seja 'null', nenhum objeto de callback é chamado.
	 */
  void onCallDispatch(CallDispatchCallback c);

  /**
   * \brief Retorna a callback a ser chamada para determinar a conexão
   *        a ser utilizada para receber cada chamada
   */
  CallDispatchCallback onCallDispatch() const;

	/**
	 * \brief Cria uma conexão para um barramento indicado por uma referência
	 *        CORBA.
	 * 
	 * O barramento é indicado por uma referência CORBA a um componente
	 * SCS que representa os serviços núcleo do barramento. Essa função
	 * deve ser utilizada ao invés da 'connectByAddress' para permitir o
	 * uso de SSL nas comunicações com o núcleo do barramento.
	 * 
	 * \param reference Referência CORBA a um componente SCS que representa os
	 *        serviços núcleo do barramento.
   * \param props Lista opcional de propriedades da conexão 
   *        (\ref Connection::ConnectionProperties).
	 *
	 * \return Conexão criada.
	 *
	 * \exception InvalidPropertyValue O valor de uma propriedade não é válido.
	 */
  std::auto_ptr<Connection> connectByReference(
    scs::core::IComponent_ptr ref,
    const Connection::ConnectionProperties &props = 
    Connection::ConnectionProperties());

  /**
   * \brief Cria uma conexão para um barramento indicado por um nome 
   *        ou endereço de rede.
   * 
   * O barramento é indicado por um nome ou endereço de rede e um
   * número de porta, onde os serviços núcleo daquele barramento estão
   * executando.
   * 
   * @param[in] host Endereço ou nome de rede onde os serviços núcleo do 
   *            barramento estão executando.
   * @param[in] port Porta onde os serviços núcleo do barramento estão 
   *            executando.
   * @param[in] props Lista opcional de propriedades que definem algumas
   *        configurações sobre a forma que as chamadas realizadas ou validadas
   *        com essa conexão são feitas. 
   *   
   * @throw InvalidPropertyValue O valor de uma propriedade não é válido.
   * @throw CORBA::Exception
   *
   * @return Conexão criada.
   */
  std::auto_ptr<Connection> connectByAddress(
    const std::string &host, unsigned short port, 
    const Connection::ConnectionProperties &props = 
    Connection::ConnectionProperties());
   
  /**
   * \brief Cria uma conexão para um barramento indicado por um nome 
   *        ou endereço de rede (deprecado).
   * 
   * \deprecated O barramento é indicado por um nome ou endereço de rede e um
   * número de porta, onde os serviços núcleo daquele barramento estão
   * executando.
   * 
   * @param[in] host Endereço ou nome de rede onde os serviços núcleo do 
   *            barramento estão executando.
   * @param[in] port Porta onde os serviços núcleo do barramento estão 
   *            executando.
   * @param[in] props Lista opcional de propriedades que definem algumas
   *        configurações sobre a forma que as chamadas realizadas ou validadas
   *        com essa conexão são feitas. 
   *   
   * @throw InvalidPropertyValue O valor de uma propriedade não é válido.
   * @throw CORBA::Exception
   *
   * @return Conexão criada.
   */
  std::auto_ptr<Connection> createConnection(
    const std::string &host, unsigned short port, 
    const Connection::ConnectionProperties &props = 
    Connection::ConnectionProperties());

  /**
   * \brief Define a conexão padrão a ser usada nas chamadas.
   * 
   * Define uma conexão a ser utilizada em chamadas sempre que não houver uma
	 * conexão específica definida no contexto atual, como é feito através da
	 * operação \ref setCurrentConnection. 
   * 
   * @param[in] conn Conexão a ser definida como conexão padrão. Um valor nulo 
   * significa nenhuma conexão definida como padrão. A propriedade(ownership) 
   * da conexão não é transferida para o \ref OpenBusContext, e a conexão 
   * deve ser removida do \ref OpenBusContext antes de destruida
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
   * @return Conexão ao barramento associada a thread
   * corrente. \ref OpenBusContext não possui ownership dessa conexão e o
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
   * \code
   * CallerChain chain(connection.getCallerChain())
   * if(chain != CallerChain())
   *   // chain é válido
   * else
   *   // chain é inválido
   * \endcode
   * \return Cadeia da chamada em execução.
   */
  CallerChain getCallerChain();
  
  /**
   * \brief Associa uma cadeia de chamadas ao contexto corrente.
   * 
   * De forma que todas as chamadas remotas seguintes neste mesmo
   * contexto sejam feitas como parte dessa cadeia de chamadas.
   * 
   * \param chain Cadeia de chamadas a ser associada ao contexto
   * corrente. A ausência de valor ou uma cadeia 'vazia' implica na
   * utilização da cadeia obtida através de getCallerChain.
   *
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
   * A cadeia de chamadas
   * informada foi associada previamente pela operação
   * \ref joinChain. Caso o contexto corrente não tenha nenhuma cadeia
   * associada, essa operação devolve uma cadeia 'vazia'
   * 'default-constructed'
   * 
   * Para verificar se a cadeia retornada é válida, o seguinte idioma é usado:
   * \code
   * CallerChain chain(openbusContext.getCallerChain())
   * if(chain != CallerChain())
   *   // chain é válido
   * else
   *   // chain é inválido
   * \endcode
   * \return Cadeia de chamadas associada ao contexto corrente ou uma
   * cadeia 'vazia'.
   *
   */
  CallerChain getJoinedChain() const;

  /**
   * \brief Cria uma cadeia de chamadas para a entidade com o identificador de
   *        login especificado.
   * 
   * Cria uma nova cadeia de chamadas para a entidade especificada,
   * onde o dono da cadeia é a conexão corrente
   * \ref getCurrentConnection() e utiliza-se a cadeia
   * atual \ref getJoinedChain() como a cadeia que se
   * deseja dar seguimento ao encadeamento. O identificador de login
   * especificado deve ser um login atualmente válido para que a
   * operação tenha sucesso. Caso o contexto corrente não tenha nenhuma
   * cadeia associada, essa operação devolve uma cadeia 'vazia'
   * 'default-constructed'.
   *
   * Para verificar se a cadeia retornada é válida, o seguinte idioma é usado:
   * \code
   * CallerChain chain(openbusContext.makeChainFor(loginId));
   * if(chain != CallerChain())
   *   // chain é válido
   * else
   *   // chain é inválido
   * \endcode
   * \param loginId identificador de login da entidade para a qual deseja-se
   *        enviar a cadeia.
   * \return a cadeia gerada para ser utilizada pela entidade com o login
   *         especificado.
   * 
   * \exception invalid_logins Caso o login especificado seja inválido.
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
   * Espera-se que a stream de bytes esteja codificada em CDR e seja formada por
   * um identificador de versão concatenado com as informações da cadeia. Caso
   * não seja possível decodificar a sequência de octetos passada, essa operação
   * devolve uma cadeia 'vazia' 'default-constructed'.
   *
   * Para verificar se a cadeia retornada é válida, o seguinte idioma é usado:
   *
   * \code
   * CallerChain chain(openbusContext.decodeChain(encoded)); 
   * if(chain != CallerChain())
   *   // chain é válido
   * else
   *   // chain é inválido
   *
   * \endcode
   * 
   * \param encoded O stream de bytes que representa a cadeia.
   * \return A cadeia de chamadas no formato CallerChain.
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
  OpenBusContext(CORBA::ORB_ptr, interceptors::ORBInitializer *);
  
  OpenBusContext(const OpenBusContext&);
  OpenBusContext &operator=(const OpenBusContext &);

  void orb(CORBA::ORB_ptr o) 
  {
    _orb = o;
  }

  CORBA::OctetSeq encode_exported_versions(idl_data_export::VersionedDataSeq,
                                           const std::string &tag);

  std::string decode_exported_versions(
    const CORBA::OctetSeq &stream,
    idl_data_export::VersionedDataSeq_out exported_version_seq) const;

  typedef std::map<std::string, Connection *> BusidConnection;
#ifdef OPENBUS_SDK_MULTITHREAD
  mutable boost::mutex _mutex;
#endif
  interceptors::ORBInitializer * _orb_init;
  CORBA::ORB_ptr _orb;
  Connection *_defaultConnection;
  BusidConnection _busidConnection;
  CallDispatchCallback _callDispatchCallback;

  friend boost::shared_ptr<orb_ctx> openbus::ORBInitializer(
    int &argc, char **argv);
};
}

#endif

