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
 *        informa��es que identificam essas chamadas em barramentos OpenBus.
 *
 * O contexto de uma chamada pode ser definido pela linha de execu��o atual
 * do programa em que executa uma chamada, o que pode ser a thread em execu��o
 * ou mais comumente o 'CORBA::PICurrent' do padr�o CORBA. As informa��es
 * acess�veis atrav�s do 'OpenBusContext' se referem basicamente �
 * identifica��o da origem das chamadas, ou seja, nome das entidades que
 * autenticaram os acessos ao barramento que originaram as chamadas.
 * 
 * A identifca��o de chamadas no barramento � controlada atrav�s do
 * OpenBusContext atrav�s da manipula��o de duas abstra��es representadas
 * pelas seguintes interfaces:
 * - Connection: Representa um acesso ao barramento, que � usado tanto para
 *   fazer chamadas como para receber chamadas atrav�s do barramento. Para
 *   tanto a conex�o precisa estar autenticada, ou seja, logada. Cada chamada
 *   feita atrav�s do ORB � enviada com as informa��es do login da conex�o
 *   associada ao contexto em que a chamada foi realizada. Cada chamada
 *   recebida tamb�m deve vir atrav�s de uma conex�o logada, que deve ser o
 *   mesmo login com que chamadas aninhadas a essa chamada original devem ser
 *   feitas.
 * - CallChain: Representa a identica��o de todos os acessos ao barramento que
 *   originaram uma chamada recebida. Sempre que uma chamada � recebida e
 *   executada, � poss�vel obter um CallChain atrav�s do qual � poss�vel
 *   inspecionar as informa��es de acesso que originaram a chamada recebida.
 */
class OPENBUS_SDK_DECL OpenBusContext : public CORBA::LocalObject 
{
public:
  /**
	 * \brief Callback de despacho de chamadas.
	 * 
	 * M�todo a ser implementado pelo objeto de callback a ser chamado quando
	 * uma chamada proveniente de um barramento � recebida. Esse m�todo � chamado
	 * para determinar a conex�o a ser utilizada na valida��o de cada chamada
	 * recebida. Se a conex�o informada n�o estiver conectada ao mesmo barramento
	 * indicado pelo par�metro 'busid', a chamada provavelmente ser� recusada com
	 * um CORBA::NO_PERMISSION{InvalidLogin} pelo fato do login provavelmente n�o
	 * ser v�lido no barramento da conex�o. Como resultado disso o cliente da
	 * chamada poder� indicar que o servidor n�o est� implementado corretamente e
	 * lan�ar a exce��o CORBA::NO_PERMISSION{InvalidRemote}. Caso alguma exce��o
	 * ocorra durante a execu��o do m�todo e n�o seja tratada, o erro ser�
	 * capturado pelo interceptador e registrado no log.
	 * 
	 * @param[in] context Gerenciador de contexto do ORB que recebeu a chamada.
	 * @param[in] busid Identifica��o do barramento atrav�s do qual a chamada foi
	 *                  feita.
	 * @param[in] loginId Informa��es do login do cliente da chamada.
	 * @param[in] operation Nome da opera��o sendo chamada.
	 *
	 * @return Conex�o a ser utilizada para receber a chamada.
	 */
  typedef boost::function<
    boost::shared_ptr<Connection> (
      OpenBusContext &context,
      const std::string busId, 
      const std::string loginId,
      const std::string operation)>
    CallDispatchCallback;

  /**
	 * \brief Define a callback a ser chamada para determinar a conex�o
	 *        a ser utilizada para receber cada chamada.
	 *
	 * Defini��o de um objeto que implementa uma interface de callback a
	 * ser chamada sempre que a conex�o receber uma chamada do
	 * barramento. Essa callback deve devolver a conex�o a ser utilizada
	 * para receber a chamada. A conex�o utilizada para receber a
	 * chamada ser� a �nica conex�o atrav�s da qual novas chamadas
	 * aninhadas � chamada recebida poder�o ser feitas (veja a opera��o
	 * 'joinChain').
	 *
	 * Se o objeto de callback for definido como 'null' ou devolver 'null', a
	 * conex�o padr�o � utilizada para receber a chamada, caso esta esteja
	 * definida.
	 *
	 * Caso esse atributo seja 'null', nenhum objeto de callback � chamado.
	 */
  void onCallDispatch(CallDispatchCallback c);

  /**
   * \brief Retorna a callback a ser chamada para determinar a conex�o
   *        a ser utilizada para receber cada chamada
   */
  CallDispatchCallback onCallDispatch() const;

  /**
	 * \brief Cria uma conex�o para um barramento indicado por uma refer�ncia
	 *        CORBA.
   * 
	 * O barramento � indicado por uma refer�ncia CORBA a um componente
	 * SCS que representa os servi�os n�cleo do barramento. Essa fun��o
	 * deve ser utilizada ao inv�s da 'connectByAddress' para permitir o
	 * uso de SSL nas comunica��es com o n�cleo do barramento.
   * 
	 * @param[in] _reference Refer�ncia CORBA a um componente SCS que representa os
	 *        servi�os n�cleo do barramento.
   * @param[in] _port Porta onde os servi�os n�cleo do barramento est�o 
   *            executando.
   * @param[in] _access_key (opcional) chave de acesso a ser utilizada
   *            internamente para a gera��o de credenciais que identificam 
   *            as chamadas atrav�s do barramento. A chave deve ser uma 
   *            chave privada RSA de 2048 bits (256 bytes). O par�metro �
   *            do tipo EVP_PKEY* e na sua aus�ncia uma chave de acesso �
   *            gerada automaticamente.
   *            
   * @param[in] _legacy_support (opcional) boleano que desabilita o suporte
   *            a chamadas usando o protocolo OpenBus 2.0. Por padr�o o suporte
   *            est� habilitado
   *
   * Exemplo de chamadas sem os par�metros opcionais:
   * \code
   * std::stringstream corbaloc;
   * corbaloc << "corbaloc::" << "localhost" << ":" << 2089
              << "/" << tecgraf::openbus::core::v2_1::BusObjectKey;
   * CORBA::Object_var
   *   ref(orb_ctx->orb()->string_to_object(corbaloc.str().c_str()));
   * connectByReference(ref.in());
   * \endcode
   *
   * Exemplo de chamada com os par�metros opcionais:
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
   * @return Conex�o criada.
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
   * \brief Cria uma conex�o para um barramento indicado por um nome 
   *        ou endere�o de rede.
   * 
   * O barramento � indicado por um nome ou endere�o de rede e um
   * n�mero de porta, onde os servi�os n�cleo daquele barramento est�o
   * executando. Este m�todo aceita argumentos no formato Boost Parameter.
   * 
   * @param[in] _host Endere�o ou nome de rede onde os servi�os n�cleo do 
   *            barramento est�o executando.
   * @param[in] _port Porta onde os servi�os n�cleo do barramento est�o 
   *            executando.
   * @param[in] _access_key (opcional) chave de acesso a ser utilizada
   *            internamente para a gera��o de credenciais que identificam 
   *            as chamadas atrav�s do barramento. A chave deve ser uma 
   *            chave privada RSA de 2048 bits (256 bytes). O par�metro �
   *            do tipo EVP_PKEY* e na sua aus�ncia uma chave de acesso �
   *            gerada automaticamente.
   *            
   * @param[in] _legacy_support (opcional) boleano que desabilita o suporte
   *            a chamadas usando o protocolo OpenBus 2.0. Por padr�o o suporte
   *            est� habilitado
   *
   * Exemplo de chamadas sem os par�metros opcionais:
   * \code
   * connectByAddress("localhost", 2089);
   * connectByAddress(openbus::_host="localhost", openbus::_port=2089);
   * \endcode
   *
   * Exemplo de chamadas com os par�metros opcionais:
   * \code
   * connectByAddress("localhost", 2089, 
   *                  _access_key=privake_key, _legacy_support=false);
   * connectByAddress("localhost", 2089, 
   *                  _legacy_support=true, _access_key=privake_key);
   * \endcode
	 * @throw InvalidBusAddress Os par�metros '_host' e '_port' n�o s�o v�lidos.
   * @return Conex�o criada.
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
   * \brief Cria uma conex�o para um barramento indicado por um nome 
   *        ou endere�o de rede (deprecado).
   * 
   * \deprecated O barramento � indicado por um nome ou endere�o de rede e um
   * n�mero de porta, onde os servi�os n�cleo daquele barramento est�o
   * executando. Este m�todo aceita argumentos no formato Boost Parameter.
   * 
   * @param[in] _host Endere�o ou nome de rede onde os servi�os n�cleo do 
   *            barramento est�o executando.
   * @param[in] _port Porta onde os servi�os n�cleo do barramento est�o 
   *            executando.
   * @param[in] _access_key (opcional) chave de acesso a ser utilizada
   *            internamente para a gera��o de credenciais que identificam 
   *            as chamadas atrav�s do barramento. A chave deve ser uma 
   *            chave privada RSA de 2048 bits (256 bytes). O par�metro �
   *            do tipo EVP_PKEY* e na sua aus�ncia uma chave de acesso �
   *            gerada automaticamente.
   *            
   * @param[in] _legacy_support (opcional) boleano que desabilita o suporte
   *            a chamadas usando o protocolo OpenBus 2.0. Por padr�o o suporte
   *            est� habilitado
   *
   * Exemplo de chamadas sem os par�metros opcionais:
   * \code
   * createConnection("localhost", 2089);
   * createConnection(openbus::_host="localhost", openbus::_port=2089);
   * \endcode
   *
   * Exemplo de chamadas com os par�metros opcionais:
   * \code
   * createConnection("localhost", 2089, 
   *                  _access_key=privake_key, _legacy_support=false);
   * createConnection("localhost", 2089, 
   *                  _legacy_support=true, _access_key=privake_key);
   * \endcode
	 * @throw InvalidBusAddress Os par�metros '_host' e '_port' n�o s�o v�lidos.
   * @return Conex�o criada.
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
   * \brief Define a conex�o padr�o a ser usada nas chamadas.
   * 
   * Define uma conex�o a ser utilizada em chamadas sempre que n�o houver uma
	 * conex�o espec�fica definida no contexto atual, como � feito atrav�s da
	 * opera��o \ref setCurrentConnection. 
   * 
   * @param[in] conn Conex�o a ser definida como conex�o padr�o. Um valor nulo 
   * significa nenhuma conex�o definida como padr�o. A propriedade(ownership) 
   * da conex�o n�o � transferida para o \ref OpenBusContext, e a conex�o 
   * deve ser removida do \ref OpenBusContext antes de destruida
   */
  boost::shared_ptr<Connection>
    setDefaultConnection(const boost::shared_ptr<Connection> &);
   
  /**
   * \brief Devolve a conex�o padr�o.
   * 
   * Veja opera��o \ref setDefaultConnection.
   * 
   * \return Conex�o definida como conex�o padr�o. \ref OpenBusContext n�o
   * possui propriedade(ownership) dessa conex�o e a mesma n�o � transferida
   * para o c�digo do usu�rio na execu��o desta fun��o.
   */
  boost::shared_ptr<Connection>
    getDefaultConnection() const;
   
  /**
   * \brief Define a conex�o associada ao contexto corrente.
   * 
   * Define a conex�o a ser utilizada em todas as chamadas
   * feitas no contexto atual. 
   * 
   * @param[in] conn Conex�o a ser associada ao contexto corrente. 
   * Um valor nulo significa nenhuma conex�o definida. A propriedade(ownership) 
   * da conex�o n�o � transferida para o \ref OpenBusContext,
   * e a conex�o deve ser removida do \ref OpenBusContext antes de ser 
   * destru�da.
   */
  boost::shared_ptr<Connection>
    setCurrentConnection(const boost::shared_ptr<Connection> &);
   
  /**
   * \brief Devolve a conex�o associada ao contexto corrente.
   * 
   * @return Conex�o ao barramento associada ao contexto
   * corrente, que pode ter sido definida usando a opera��o 
   * \ref setCurrentConnection ou \ref setDefaultConnection. 
   * \ref OpenBusContext n�o possui propriedade(ownership) 
   * dessa conex�o e a mesma n�o � transferida para o c�digo do usu�rio 
   * na execu��o desta fun��o.
   */
  boost::shared_ptr<Connection>
    getCurrentConnection() const;
       
  /**
   * \brief Devolve a cadeia de chamadas � qual a execu��o corrente pertence.
   * 
   * Caso a contexto corrente (e.g. definido pelo 'CORBA::PICurrent')
   * seja o contexto de execu��o de uma chamada remota oriunda do
   * barramento dessa conex�o, essa opera��o devolve um objeto que
   * representa a cadeia de chamadas do barramento que esta chamada
   * faz parte. Caso contr�rio, devolve uma cadeia de chamadas
   * 'vazia', representada por um CallerChain 'default-constructed'.
   *
   * Para verificar se a cadeia retornada � v�lida, o seguinte idioma
   * � usado:
   *
   * \code
   * CallerChain chain(connection.getCallerChain())
   * if(chain != CallerChain())
   *   // chain � v�lido
   * else
   *   // chain � inv�lido
   * \endcode
   * \return Cadeia da chamada em execu��o.
   */
  CallerChain getCallerChain();
  
  /**
   * \brief Associa uma cadeia de chamadas ao contexto corrente.
   * 
   * De forma que todas as chamadas remotas seguintes neste mesmo
   * contexto sejam feitas como parte dessa cadeia de chamadas.
   * 
   * \param chain Cadeia de chamadas a ser associada ao contexto
   * corrente. A aus�ncia de valor ou uma cadeia 'vazia' implica na
   * utiliza��o da cadeia obtida atrav�s de getCallerChain.
   *
   */
  void joinChain(const CallerChain &chain = CallerChain());
  
  /**
   * \brief Faz com que nenhuma cadeia de chamadas esteja associada ao
   * contexto corrente.
   * 
   * Remove a associa��o da cadeia de chamadas ao contexto corrente,
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
   * informada foi associada previamente pela opera��o
   * \ref joinChain. Caso o contexto corrente n�o tenha nenhuma cadeia
   * associada, essa opera��o devolve uma cadeia 'vazia'
   * 'default-constructed'
   * 
   * Para verificar se a cadeia retornada � v�lida, o seguinte idioma � usado:
   * \code
   * CallerChain chain(openbusContext.getCallerChain())
   * if(chain != CallerChain())
   *   // chain � v�lido
   * else
   *   // chain � inv�lido
   * \endcode
   * \return Cadeia de chamadas associada ao contexto corrente ou uma
   * cadeia 'vazia'.
   *
   */
  CallerChain getJoinedChain() const;

  /**
   * \brief Cria uma cadeia de chamadas para a entidade especificada.
   * 
   * O dono da cadeia � a conex�o corrente \ref getCurrentConnection()
   * e utiliza-se a cadeia atual \ref getJoinedChain() como a cadeia
   * que se deseja dar seguimento ao encadeamento. � permitido
   * especificar qualquer nome de entidade, tendo ela um login ativo
   * no momento ou n�o.  A cadeia resultante s� conseguir� ser
   * utilizada \ref joinChain() com sucesso por uma conex�o que possua
   * a mesma identidade da entidade especificada.
   *
   * Para verificar se a cadeia retornada � v�lida, o seguinte idioma � usado:
   * \code
   * CallerChain chain(openbusContext.makeChainFor(loginId));
   * if(chain != CallerChain())
   *   // chain � v�lido
   * else
   *   // chain � inv�lido
   * \endcode
   * \param entity nome da entidade para a qual deseja-se enviar a
   *        cadeia.
   * \return a cadeia gerada para ser utilizada pela entidade com o login
   *         especificado.
   */
  CallerChain makeChainFor(const std::string &entity) const;

	/**
	 * \brief Cria uma cadeia de chamadas assinada pelo barramento com
	 *        informa��es de uma autentica��o externa ao barramento.
	 * 
	 * A cadeia criada pode ser usada pela entidade do login que faz a chamada.
	 * O conte�do da cadeia � dado pelas informa��es obtidas atrav�s do token
	 * indicado.
	 * 
	 * \param token Valor opaco que representa uma informa��o de autentica��o 
   *        externa.
	 * \param domain Identificador do dom�nio de autentica��o.
	 * \return A nova cadeia de chamadas assinada.
	 *
	 * \exception idl::access::InvalidToken O token fornecido n�o foi reconhecido.
	 * \exception idl::access::UnknownDomain O dom�nio de autentica��o n�o � 
   *            conhecido.
	 * \exception idl::access::WrongEncoding A importa��o falhou, pois o token n�o 
   *            foi codificado corretamente com a chave p�blica do barramento.
	 * \exception idl::core::ServiceFailure Ocorreu uma falha interna nos servi�os 
   *            do barramento que impediu a cria��o da cadeia.
	 */
	CallerChain importChain(
    const CORBA::OctetSeq &token,
    const std::string &domain = "") const;

  /**
   * \brief Codifica uma cadeia de chamadas (CallerChain) para um stream de
   * bytes.
   * 
   * Codifica uma cadeia de chamadas em um stream de bytes para permitir a
   * persist�ncia ou transfer�ncia da informa��o. A codifica��o � realizada em
   * CDR e possui um identificador de vers�o concatenado com as informa��es da
   * cadeia. Sendo assim, a stream s� ser� decodificada com sucesso por algu�m
   * que entenda esta mesma codifica��o.
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
   * um identificador de vers�o concatenado com as informa��es da cadeia. Caso
   * n�o seja poss�vel decodificar a sequ�ncia de octetos passada, essa opera��o
   * devolve uma cadeia 'vazia' 'default-constructed'.
   *
   * Para verificar se a cadeia retornada � v�lida, o seguinte idioma � usado:
   *
   * \code
   * CallerChain chain(openbusContext.decodeChain(encoded)); 
   * if(chain != CallerChain())
   *   // chain � v�lido
   * else
   *   // chain � inv�lido
   *
   * \endcode
   * 
   * \param encoded O stream de bytes que representa a cadeia.
   * \return A cadeia de chamadas no formato CallerChain.
   * 
   * \throw InvalidEncodedStream Caso o stream de bytes n�o seja do
   * formato esperado.
   */
  CallerChain decodeChain(const CORBA::OctetSeq &encoded) const;
  
  /**
	 * \brief Codifica um segredo de autentica��o compartilhada
   *        SharedAuthSecret para um stream de bytes.
   * 
   * Codifica um segredo de autentica��o compartilhada em um stream de bytes
   * para permitir a persist�ncia ou transfer�ncia da informa��o.
   * 
   * \param secret Segredo de autentica��o compartilhada a ser codificado.
   * \return Cadeia codificada em um stream de bytes.
   */
  CORBA::OctetSeq encodeSharedAuthSecret(const SharedAuthSecret &secret);

  /**
   * \brief Decodifica um segredo de autentica��o compartilhada
   *				SharedAuthSecret a partir de um stream de bytes.
   * 
   * \param encoded Stream de bytes contendo a codifica��o do segredo.
   * \return Segredo de autentica��o compartilhada decodificado.
   * \exception InvalidEncodedStream Caso a stream de bytes n�o seja do formato
   *						 esperado.
   */
  SharedAuthSecret decodeSharedAuthSecret(const CORBA::OctetSeq &encoded);

  /** 
   * ORB utilizado pela conex�o. 
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

