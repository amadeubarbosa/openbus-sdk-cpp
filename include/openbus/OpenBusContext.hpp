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
 * Cole��o de informa��es dos logins que originaram chamadas em cadeia
 * atrav�s de um barramento. Cadeias de chamadas representam chamadas
 * aninhadas dentro do barramento e s�o �teis para que os sistemas que
 * recebam essas chamadas possam identificar se a chamada foi
 * originada por entidades autorizadas ou n�o.
 */
struct OPENBUS_SDK_DECL CallerChain 
{
  /**
  * Barramento atrav�s do qual as chamadas foram originadas.
  */
  const std::string busid() const 
  {
    return _busid;
  }

	/**
   * Entidade para a qual a chamada estava destinada. S� � poss�vel fazer 
   * chamadas dentro dessa cadeia (atrav�s do m�todo joinChain da interface 
   * OpenBusContext) se a entidade da conex�o corrente for o mesmo do target.
   *
   * No caso de conex�es legadas, este campo armazenar� o nome da entidade 
   * da conex�o que atendeu a requisi��o. Todas as chamadas feitas como 
   * parte de uma cadeia de uma chamada legada ser�o feitas utilizando 
   * apenas o protocolo do OpenBus 1.5 (apenas com credenciais legadas) 
   * e portanto ser�o recusadas por servi�os que n�o aceitem chamadas
   * legadas (OpenBus 1.5).
   */
  const std::string target() const
  {
    return _target;
  }
  
	/**
	 * Lista de informa��es de login de todas as entidades que originaram as
	 * chamadas nessa cadeia. Quando essa lista � vazia isso indica que a
	 * chamada n�o est� inclusa em outra cadeia de chamadas.
	 */
  const idl_ac::LoginInfoSeq &originators() const 
  {
    return _originators;
  }
  
  /**
   * Informa��o de login da entidade que realizou a �ltima chamada da cadeia.
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
    Connection* (OpenBusContext &context,
                 const std::string busId, 
                 const std::string loginId,
                 const std::string operation)>
    CallDispatchCallback;

  /**
	 * \brief Define a callback a ser chamada para determinar a conex�o
	 *        a ser utilizada para receber cada chamada.
	 *
	 * Esse atributo � utilizado para definir um objeto que implementa uma
	 * interface de callback a ser chamada sempre que a conex�o receber uma do
	 * barramento. Essa callback deve devolver a conex�o a ser utilizada para
	 * receber a chamada. A conex�o utilizada para receber a chamada ser�
	 * a �nica conex�o atrav�s da qual novas chamadas aninhadas � chamada
	 * recebida poder�o ser feitas (veja a opera��o 'joinChain').
	 *
	 * Se o objeto de callback for definido como 'null' ou devolver 'null', a
	 * conex�o padr�o � utilizada para receber a chamada, caso esta esteja
	 * definida.
	 *
	 * Caso esse atributo seja 'null', nenhum objeto de callback � chamado na
	 * ocorr�ncia desse evento e 
	 */
  void onCallDispatch(CallDispatchCallback c);

  /**
   * \brief Retorna a callback a ser chamada para determinar a conex�o
   *        a ser utilizada para receber cada chamada
   */
  CallDispatchCallback onCallDispatch() const;

  /**
   * \brief Cria uma conex�o para um barramento.
   * 
   * Cria uma conex�o para um barramento. O barramento � indicado por
   * um nome ou endere�o de rede e um n�mero de porta, onde os
   * servi�os n�cleo daquele barramento est�o executando.
   * 
   * @param[in] host Endere�o ou nome de rede onde os servi�os n�cleo do 
   *            barramento estao executando.
   * @param[in] port Porta onde os servi�os n�cleo do barramento est�o 
   *            executando.
   * @param[in] props Lista opcional de propriedades que definem algumas
   *        configura��es sobre a forma que as chamadas realizadas ou validadas
   *        com essa conex�o s�o feitas. A seguir s�o listadas as propriedades
   *        v�lidas:
   *        - legacy.disable: desabilita o suporte a chamadas usando protocolo
   *          OpenBus 1.5. Por padr�o o suporte est� habilitado.
   *        - legacy.delegate: indica como � preenchido o campo 'delegate' das
   *          credenciais enviadas em chamadas usando protocolo OpenBus 1.5. H�
   *          duas formas poss�veis (o padr�o � 'caller'):
   *          - caller: o campo 'delegate' � preenchido sempre com a entidade
   *            do campo 'caller' da cadeia de chamadas.
   *          - originator: o campo 'delegate' � preenchido sempre com a
   *            entidade que originou a cadeia de chamadas, que � o primeiro
   *            login do campo 'originators' ou o campo 'caller' quando este
   *            � vazio.
   *   
   * @throw InvalidPropertyValue O valor de uma propriedade n�o � v�lido.
   * @throw CORBA::Exception
   *
   * @return Conex�o criada.
   */
  std::auto_ptr<Connection> createConnection(
    const std::string host, unsigned short port, 
    const Connection::ConnectionProperties &props = 
    Connection::ConnectionProperties());
   
  /**
   * \brief Define a conex�o padr�o a ser usada nas chamadas.
   * 
   * Define uma conex�o a ser utilizada como "Requester" e
   * "Dispatcher" de chamadas sempre que n�o houver uma conex�o
   * "Requester" e "Dispatcher" espec�fica definida para o caso
   * espec�fico, como � feito atrav�s das opera��es
   * 'setCurrentConnection' e 'setDispatcher'.
   * 
   * @param[in] conn Conex�o a ser definida como conex�o padr�o. O
   * 'ownership' da conex�o n�o � transferida para o OpenBusContext, e
   * a conex�o deve ser removida do OpenBusContext antes de destruida
   */
  Connection *setDefaultConnection(Connection *);
   
  /**
   * \brief Devolve a conex�o padr�o.
   * 
   * Veja opera��o 'setDefaultConnection'.
   * 
   * \return Conex�o definida como conex�o padr�o. OpenBusContext n�o
   * possui ownership dessa conex�o e o mesmo n�o � transferido para o
   * c�digo de usu�rio na execu��o desta fun��o
   */
  Connection *getDefaultConnection() const;
   
  /**
   * \brief Define a conex�o "Requester" do contexto corrente.
   * 
   * Define a conex�o "Requester" a ser utilizada em todas as chamadas
   * feitas no contexto atual. Quando 'conn' � 'null' o contexto passa
   * a ficar sem nenhuma conex�o associada.
   * 
   * @param[in] conn Conex�o a ser associada ao contexto corrente. O
   * 'ownership' da conex�o n�o � transferida para o OpenBusContext, e
   * a conex�o deve ser removida do OpenBusContext antes de destruida
   */
  Connection *setCurrentConnection(Connection *);
   
  /**
   * \brief Devolve a conex�o associada ao contexto corrente.
   * 
   * @throw CORBA::Exception
   *
   * @return Conex�o a barramento associada a thread
   * corrente. OpenBusContext n�o possui ownership dessa conex�o e o
   * mesmo n�o � transferido para o c�digo de usu�rio na execu��o
   * desta fun��o
   */
  Connection *getCurrentConnection() const;
       
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
   * CallerChain chain = connection.getCallerChain()
   * if(chain != CallerChain())
   *   // chain � v�lido
   * else
   *   // chain � inv�lido
   * 
   * \return Cadeia da chamada em execu��o.
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
   * corrente. A aus�ncia de valor ou uma cadeia 'vazia' implica na
   * utiliza��o da cadeia obtida atrav�s de getCallerChain.
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
   * Devolve um objeto que representa a cadeia de chamadas associada
   * ao contexto corrente nesta conex�o.  A cadeia de chamadas
   * informada foi associada previamente pela opera��o
   * 'joinChain'. Caso o contexto corrente n�o tenha nenhuma cadeia
   * associada, essa opera��o devolve uma cadeia 'vazia'
   * 'default-constructed'
   * 
   * Para verificar se a cadeia retornada � v�lida, o seguinte idioma � usado:
   *
   * CallerChain chain = openbusContext.getCallerChain()
   * if(chain != CallerChain())
   *   // chain � v�lido
   * else
   *   // chain � inv�lido
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
   * onde o dono da cadeia � a conex�o corrente
   * (OpenBusContext::getCurrentConnection()) e utiliza-se a cadeia
   * atual (OpenBusContext::getJoinedChain()) como a cadeia que se
   * deseja dar seguimento ao encadeamento. O identificador de login
   * especificado deve ser um login atualmente v�lido para que a
   * opera��o tenha sucesso.Caso o contexto corrente n�o tenha nenhuma
   * cadeia associada, essa opera��o devolve uma cadeia 'vazia'
   * 'default-constructed'.
   *
   * Para verificar se a cadeia retornada � v�lida, o seguinte idioma � usado:
   *
   * CallerChain chain = openbusContext.makeChainFor(loginId);
   * if(chain != CallerChain())
   *   // chain � v�lido
   * else
   *   // chain � inv�lido
   * 
   * \param loginId identificador de login da entidade para a qual deseja-se
   *        enviar a cadeia.
   * \return a cadeia gerada para ser utilizada pela entidade com o login
   *         especificado.
   * 
   * \exception InvalidLogins Caso o login especificado seja inv�lido.
   * \exception ServiceFailure Ocorreu uma falha interna nos servi�os do 
   *            barramento que impediu a cria��o da cadeia.
   */
  CallerChain makeChainFor(const std::string &loginId) const;

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
   * Decodifica um stream de bytes de uma cadeia para o formato CallerChain.
   * Espera-se que a stream de bytes esteja codificada em CDR e seja formada por
   * um identificador de vers�o concatenado com as informa��es da cadeia. Caso
   * n�o seja poss�vel decodificar a sequ�ncia de octetos passada, essa opera��o
   * devolve uma cadeia 'vazia' 'default-constructed'.
   *
   * Para verificar se a cadeia retornada � v�lida, o seguinte idioma � usado:
   *
   * CallerChain chain = openbusContext.decodeChain(encoded);
   * if(chain != CallerChain())
   *   // chain � v�lido
   * else
   *   // chain � inv�lido
   * 
   * \param encoded O stream de bytes que representa a cadeia.
   * \return A cadeia de chamadas no formato CallerChain, 'vazia' em caso de
   * falha da decodifica��o.
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
   * Decodifica um segredo de autentica��o compartilhada a partir de um stream
   * de bytes.
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

