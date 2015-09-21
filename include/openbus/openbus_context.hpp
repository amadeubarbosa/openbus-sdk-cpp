// -*- coding: iso-8859-1-unix -*-
/**
* API do OpenBus SDK C++
* \file openbus/openbus_context.hpp
*/

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_CONTEXT_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_CONTEXT_HPP

#include "openbus/idl.hpp"
#include "openbus/detail/decl.hpp"
#include "openbus/orb_initializer.hpp"
#include "openbus/connection.hpp"
#include "openbus/caller_chain.hpp"

#include <tao/LocalObject.h>
#include <openssl/evp.h>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/parameter.hpp>
#include <boost/uuid/uuid.hpp>
#include <string>
#include <map>
#include <set>

namespace openbus 
{
  namespace interceptors
  {
    struct orb_info;
    struct ClientInterceptor;
  }
}

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

BOOST_PARAMETER_NAME(host)
BOOST_PARAMETER_NAME(port)
BOOST_PARAMETER_NAME(access_key)
BOOST_PARAMETER_NAME(legacy_support)
BOOST_PARAMETER_NAME(reference)

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
    boost::shared_ptr<Connection> (
      OpenBusContext &context,
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
	 * @param[in] _reference Referência CORBA a um componente SCS que representa os
	 *        serviços núcleo do barramento.
   * @param[in] _port Porta onde os serviços núcleo do barramento estão 
   *            executando.
   * @param[in] _access_key (opcional) chave de acesso a ser utilizada
   *            internamente para a geração de credenciais que identificam 
   *            as chamadas através do barramento. A chave deve ser uma 
   *            chave privada RSA de 2048 bits (256 bytes). O parâmetro é
   *            do tipo EVP_PKEY* e na sua ausência uma chave de acesso é
   *            gerada automaticamente.
   *            
   * @param[in] _legacy_support (opcional) boleano que desabilita o suporte
   *            a chamadas usando o protocolo OpenBus 2.0. Por padrão o suporte
   *            está habilitado
   *
   * Exemplo de chamadas sem os parâmetros opcionais:
   * \code
   * std::stringstream corbaloc;
   * corbaloc << "corbaloc::" << "localhost" << ":" << 2089
              << "/" << tecgraf::openbus::core::v2_1::BusObjectKey;
   * CORBA::Object_var
   *   ref(orb_ctx->orb()->string_to_object(corbaloc.str().c_str()));
   * connectByReference(ref.in());
   * \endcode
   *
   * Exemplo de chamada com os parâmetros opcionais:
   * \code
   * std::stringstream corbaloc;
   * corbaloc << "corbaloc::" << "localhost" << ":" << 2089
              << "/" << tecgraf::openbus::core::v2_1::BusObjectKey;
   * CORBA::Object_var
   *   ref(orb_ctx->orb()->string_to_object(corbaloc.str().c_str()));
   * connectByReference(ref.in(),
   *                    _access_key=privake_key, 
   *                    _legacy_support=false);
   * \endcode
   * @return Conexão criada.
   *
   */
  BOOST_PARAMETER_MEMBER_FUNCTION(
    (boost::shared_ptr<Connection>), connectByReference, tag,
    (required
     (reference, (CORBA::Object_var)))
    (optional
     (access_key, (EVP_PKEY*), (EVP_PKEY*)0)
     (legacy_support, (bool), true))
  )
  {
    return connect_by_reference_impl(
      reference, access_key, legacy_support);
  }

  /**
   * \brief Cria uma conexão para um barramento indicado por um nome 
   *        ou endereço de rede.
   * 
   * O barramento é indicado por um nome ou endereço de rede e um
   * número de porta, onde os serviços núcleo daquele barramento estão
   * executando. Este método aceita argumentos no formato Boost Parameter.
   * 
   * @param[in] _host Endereço ou nome de rede onde os serviços núcleo do 
   *            barramento estão executando.
   * @param[in] _port Porta onde os serviços núcleo do barramento estão 
   *            executando.
   * @param[in] _access_key (opcional) chave de acesso a ser utilizada
   *            internamente para a geração de credenciais que identificam 
   *            as chamadas através do barramento. A chave deve ser uma 
   *            chave privada RSA de 2048 bits (256 bytes). O parâmetro é
   *            do tipo EVP_PKEY* e na sua ausência uma chave de acesso é
   *            gerada automaticamente.
   *            
   * @param[in] _legacy_support (opcional) boleano que desabilita o suporte
   *            a chamadas usando o protocolo OpenBus 2.0. Por padrão o suporte
   *            está habilitado
   *
   * Exemplo de chamadas sem os parâmetros opcionais:
   * \code
   * connectByAddress("localhost", 2089);
   * connectByAddress(openbus::_host="localhost", openbus::_port=2089);
   * \endcode
   *
   * Exemplo de chamadas com os parâmetros opcionais:
   * \code
   * connectByAddress("localhost", 2089, 
   *                  _access_key=privake_key, _legacy_support=false);
   * connectByAddress("localhost", 2089, 
   *                  _legacy_support=true, _access_key=privake_key);
   * \endcode
	 * @throw InvalidBusAddress Os parâmetros '_host' e '_port' não são válidos.
   * @return Conexão criada.
   *
   */
  BOOST_PARAMETER_MEMBER_FUNCTION(
    (boost::shared_ptr<Connection>), connectByAddress, tag,
    (required
     (host, (const std::string&))
     (port, (unsigned short)))
    (optional
     (access_key, (EVP_PKEY*), (EVP_PKEY*)0)
     (legacy_support, (bool), true))
  )
  {
    return connect_by_address_impl(
      host, port, access_key, legacy_support);
  }
    
  /**
   * \brief Cria uma conexão para um barramento indicado por um nome 
   *        ou endereço de rede (deprecado).
   * 
   * \deprecated O barramento é indicado por um nome ou endereço de rede e um
   * número de porta, onde os serviços núcleo daquele barramento estão
   * executando. Este método aceita argumentos no formato Boost Parameter.
   * 
   * @param[in] _host Endereço ou nome de rede onde os serviços núcleo do 
   *            barramento estão executando.
   * @param[in] _port Porta onde os serviços núcleo do barramento estão 
   *            executando.
   * @param[in] _access_key (opcional) chave de acesso a ser utilizada
   *            internamente para a geração de credenciais que identificam 
   *            as chamadas através do barramento. A chave deve ser uma 
   *            chave privada RSA de 2048 bits (256 bytes). O parâmetro é
   *            do tipo EVP_PKEY* e na sua ausência uma chave de acesso é
   *            gerada automaticamente.
   *            
   * @param[in] _legacy_support (opcional) boleano que desabilita o suporte
   *            a chamadas usando o protocolo OpenBus 2.0. Por padrão o suporte
   *            está habilitado
   *
   * Exemplo de chamadas sem os parâmetros opcionais:
   * \code
   * createConnection("localhost", 2089);
   * createConnection(openbus::_host="localhost", openbus::_port=2089);
   * \endcode
   *
   * Exemplo de chamadas com os parâmetros opcionais:
   * \code
   * createConnection("localhost", 2089, 
   *                  _access_key=privake_key, _legacy_support=false);
   * createConnection("localhost", 2089, 
   *                  _legacy_support=true, _access_key=privake_key);
   * \endcode
	 * @throw InvalidBusAddress Os parâmetros '_host' e '_port' não são válidos.
   * @return Conexão criada.
   *
   */
  BOOST_PARAMETER_MEMBER_FUNCTION(
    (boost::shared_ptr<Connection>), createConnection, tag,
    (required
     (host, (const std::string&))
     (port, (unsigned short)))
    (optional
     (access_key, (EVP_PKEY*), (EVP_PKEY*)0)
     (legacy_support, (bool), true))
  )
  {
    return connect_by_address_impl(
      host, port, access_key, legacy_support);
  }

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
  boost::shared_ptr<Connection>
    setDefaultConnection(const boost::shared_ptr<Connection> &);
   
  /**
   * \brief Devolve a conexão padrão.
   * 
   * Veja operação \ref setDefaultConnection.
   * 
   * \return Conexão definida como conexão padrão. \ref OpenBusContext não
   * possui propriedade(ownership) dessa conexão e a mesma não é transferida
   * para o código do usuário na execução desta função.
   */
  boost::shared_ptr<Connection>
    getDefaultConnection() const;
   
  /**
   * \brief Define a conexão associada ao contexto corrente.
   * 
   * Define a conexão a ser utilizada em todas as chamadas
   * feitas no contexto atual. 
   * 
   * @param[in] conn Conexão a ser associada ao contexto corrente. 
   * Um valor nulo significa nenhuma conexão definida. A propriedade(ownership) 
   * da conexão não é transferida para o \ref OpenBusContext,
   * e a conexão deve ser removida do \ref OpenBusContext antes de ser 
   * destruída.
   */
  boost::shared_ptr<Connection>
    setCurrentConnection(const boost::shared_ptr<Connection> &);
   
  /**
   * \brief Devolve a conexão associada ao contexto corrente.
   * 
   * @return Conexão ao barramento associada ao contexto
   * corrente, que pode ter sido definida usando a operação 
   * \ref setCurrentConnection ou \ref setDefaultConnection. 
   * \ref OpenBusContext não possui propriedade(ownership) 
   * dessa conexão e a mesma não é transferida para o código do usuário 
   * na execução desta função.
   */
  boost::shared_ptr<Connection>
    getCurrentConnection() const;
       
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
   * \brief Cria uma cadeia de chamadas para a entidade especificada.
   * 
   * O dono da cadeia é a conexão corrente \ref getCurrentConnection()
   * e utiliza-se a cadeia atual \ref getJoinedChain() como a cadeia
   * que se deseja dar seguimento ao encadeamento. É permitido
   * especificar qualquer nome de entidade, tendo ela um login ativo
   * no momento ou não.  A cadeia resultante só conseguirá ser
   * utilizada \ref joinChain() com sucesso por uma conexão que possua
   * a mesma identidade da entidade especificada.
   *
   * Para verificar se a cadeia retornada é válida, o seguinte idioma é usado:
   * \code
   * CallerChain chain(openbusContext.makeChainFor(loginId));
   * if(chain != CallerChain())
   *   // chain é válido
   * else
   *   // chain é inválido
   * \endcode
   * \param entity nome da entidade para a qual deseja-se enviar a
   *        cadeia.
   * \return a cadeia gerada para ser utilizada pela entidade com o login
   *         especificado.
   */
  CallerChain makeChainFor(const std::string &entity) const;

	/**
	 * \brief Cria uma cadeia de chamadas assinada pelo barramento com
	 *        informações de uma autenticação externa ao barramento.
	 * 
	 * A cadeia criada pode ser usada pela entidade do login que faz a chamada.
	 * O conteúdo da cadeia é dado pelas informações obtidas através do token
	 * indicado.
	 * 
	 * \param token Valor opaco que representa uma informação de autenticação 
   *        externa.
	 * \param domain Identificador do domínio de autenticação.
	 * \return A nova cadeia de chamadas assinada.
	 *
	 * \exception idl::access::InvalidToken O token fornecido não foi reconhecido.
	 * \exception idl::access::UnknownDomain O domínio de autenticação não é 
   *            conhecido.
	 * \exception idl::access::WrongEncoding A importação falhou, pois o token não 
   *            foi codificado corretamente com a chave pública do barramento.
	 * \exception idl::core::ServiceFailure Ocorreu uma falha interna nos serviços 
   *            do barramento que impediu a criação da cadeia.
	 */
	CallerChain importChain(
    const CORBA::OctetSeq &token,
    const std::string &domain = "") const;

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
  
  idl::offers::OfferRegistry_ptr getOfferRegistry() const;
  idl::access::LoginRegistry_ptr getLoginRegistry() const;
  ~OpenBusContext();
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

  CORBA::OctetSeq encode_exported_versions(idl::data_export::VersionedDataSeq,
                                           const std::string &tag);

  std::string decode_exported_versions(
    const CORBA::OctetSeq &stream,
    idl::data_export::VersionedDataSeq_out exported_version_seq) const;

  boost::shared_ptr<Connection> connect_by_address_impl(
    const std::string &host,
    unsigned short port,
    EVP_PKEY *access_key,
    bool legacy_support);

  boost::shared_ptr<Connection> connect_by_reference_impl(
    CORBA::Object_var,
    EVP_PKEY *access_key,
    bool legacy_support);

  boost::shared_ptr<Connection> create_connection_impl(
    const std::string &host,
    unsigned short port,
    EVP_PKEY *access_key,
    bool legacy_support);

  CallerChain extract_call_chain(
    idl::creden::SignedData,
    const boost::shared_ptr<Connection> &);
  
  CallerChain extract_legacy_call_chain(
    idl::legacy::creden::SignedCallChain,
    const boost::shared_ptr<Connection> &);

  mutable boost::mutex _mutex;
  interceptors::ORBInitializer * _orb_init;
  CORBA::ORB_ptr _orb;
  boost::weak_ptr<Connection> _def_conn;
  CallDispatchCallback _call_dispatch_cbk;
  std::map<boost::uuids::uuid, boost::weak_ptr<Connection> > id2conn;
  std::set<boost::weak_ptr<Connection> > connections;

  friend boost::shared_ptr<orb_ctx> openbus::ORBInitializer(
    int &argc, char **argv);
  friend struct interceptors::ServerInterceptor;
  friend struct interceptors::ClientInterceptor;
};
}

#endif

