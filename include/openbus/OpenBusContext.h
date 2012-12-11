// -*- coding: iso-8859-1 -*-
/**
* API - SDK Openbus C++
* \file openbus/OpenBusContext.h
*/

#ifndef _TECGRAF_MANAGER_H_
#define _TECGRAF_MANAGER_H_

#include <CORBA.h>
#include <string>
#include <vector>

namespace openbus {
  class OpenBusContext;
  struct CallerChain;
}

#include "openbus/interceptors/ORBInitializer_impl.h"
#include "openbus/Connection.h"


inline bool operator==(openbus::idl_ac::LoginInfo const &lhs, 
                       openbus::idl_ac::LoginInfo const &rhs)
{
  return lhs.id.in() == rhs.id.in() || 
    (lhs.id.in() && rhs.id.in() && !std::strcmp(lhs.id.in(), rhs.id.in()));
}

inline bool operator!=(openbus::idl_ac::LoginInfo const &lhs, 
                       openbus::idl_ac::LoginInfo const &rhs)
{
  return !(lhs == rhs);
}

/**
* \brief openbus
*/
namespace openbus {
/**
 * \brief Cadeia de chamadas oriundas de um barramento.
 * 
 * Cole��o de informa��es dos logins que originaram chamadas em cadeia atrav�s de um
 * barramento. Cadeias de chamadas representam chamadas aninhadas dentro do barramento e s�o �teis
 * para que os sistemas que recebam essas chamadas possam identificar se a chamada foi originada por
 * entidades autorizadas ou n�o.
 */
struct CallerChain {
  /**
  * Barramento atrav�s do qual as chamadas foram originadas.
  */
  const char *busid() const {
    return _busid.c_str();
  }
  
  /**
  * Lista de informa��es de login de todas as entidades que realizaram chamadas que originaram a
  * cadeia de chamadas da qual essa chamada est� inclusa.  Quando essa lista � vazia isso indica
  * que a chamada n�o est� inclusa em uma cadeia de chamadas.
  * 
  * A ordem da sequ�ncia retornada � come�ando da fonte da cadeia at� o pen�ltimo da cadeia na
  * chamada. Assim, originators()[0], se existir, � quem originou a chamada de cadeia.
  */
  const idl_ac::LoginInfoSeq &originators() const {
    return _originators;
  }
  
  /**
   * Informa��o de login da entidade que realizou a �ltima chamada da cadeia.
   */
  const idl_ac::LoginInfo &caller() const {
    return _caller;
  }

  /**
   * \brief Construtor default que indica o valor de CallChain "vazio"
   *
   * O valor de um CallerChain default-constructed pode ser usado para verificar a ausencia de
   * CallerChain da seguinte forma:
   * 
   * CallerChain chain = openbusContext.getCallerChain();
   * if(chain != CallerChain())
   *   // Possui CallerChain
   * else
   *   // Nao possui CallerChain
   *
   */
  CallerChain() {}
private:
  CallerChain(const char *busid, const idl_ac::LoginInfoSeq &b, const idl_ac::LoginInfo &c, 
    const idl_cr::SignedCallChain &d) 
    : _busid(busid), _originators(b), _caller(c), _signedCallChain(d) { }
  
  CallerChain(const char *busid, const idl_ac::LoginInfoSeq &b, const idl_ac::LoginInfo &c) 
    : _busid(busid), _originators(b), _caller(c) { }
  
  std::string _busid;
  idl_ac::LoginInfoSeq _originators;
  idl_ac::LoginInfo _caller;
  idl_cr::SignedCallChain _signedCallChain;
  const idl_cr::SignedCallChain *signedCallChain() const { return &_signedCallChain; }
  void signedCallChain(idl_cr::SignedCallChain p) { _signedCallChain = p; }
  friend class OpenBusContext;
  friend class openbus::interceptors::ClientInterceptor;
  friend inline bool operator==(CallerChain const &lhs, CallerChain const &rhs) {
    return lhs._busid == rhs._busid && lhs._originators == rhs._originators
      && lhs._caller == rhs._caller;
  }
};

inline bool operator!=(CallerChain const &lhs, CallerChain const &rhs) {
  return !(lhs == rhs);
}

class OpenBusContext : public CORBA::LocalObject {
public:
  typedef boost::function<Connection* (OpenBusContext &context, const char *busId, 
                                       const char *loginId, 
                                       const char *operation)> CallDispatchCallback;

  void onCallDispatch(CallDispatchCallback c);

  CallDispatchCallback onCallDispatch() const;

  /**
   * \brief Cria uma conex�o para um barramento.
   * 
   * Cria uma conex�o para um barramento. O barramento � indicado por um nome ou endere�o de rede
   * e um n�mero de porta, onde os servi�os n�cleo daquele barramento est�o executando.
   * 
   * @param[in] host Endere�o ou nome de rede onde os servi�os n�cleo do barramento est�o 
   *            executando.
   * @param[in] port Porta onde os servi�os n�cleo do barramento est�o executando.
   * @param[in] props Lista opcional de propriedades que definem algumas
   *        configura��es sobre a forma que as chamadas realizadas ou validadas
   *        com essa conex�o s�o feitas. A seguir s�o listadas as propriedades
   *        v�lidas:
   *        - access.key: chave de acesso a ser utiliza internamente para a
   *          gera��o de credenciais que identificam as chamadas atrav�s do
   *          barramento. A chave deve ser uma chave privada RSA de 2048 bits
   *          (256 bytes). Quando essa propriedade n�o � fornecida, uma chave
   *          de acesso � gerada automaticamente.
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
  std::auto_ptr<Connection> createConnection(const char *host, short port,
                                             std::vector<std::string> props 
                                             = std::vector<std::string>());
   
  /**
   * \brief Define a conex�o padr�o a ser usada nas chamadas.
   * 
   * Define uma conex�o a ser utilizada como "Requester" e "Dispatcher" de chamadas sempre que n�o
   * houver uma conex�o "Requester" e "Dispatcher" espec�fica definida para o caso espec�fico,
   * como � feito atrav�s das opera��es 'setCurrentConnection' e 'setDispatcher'.
   * 
   * @param[in] conn Conex�o a ser definida como conex�o padr�o. O 'ownership' da conex�o n�o �
   * transferida para o OpenBusContext, e a conex�o deve ser removida do OpenBusContext
   * antes de destruida
   */
  Connection *setDefaultConnection(Connection *);
   
  /**
   * \brief Devolve a conex�o padr�o.
   * 
   * Veja opera��o 'setDefaultConnection'.
   * 
   * \return Conex�o definida como conex�o padr�o. OpenBusContext n�o possui ownership dessa
   * conex�o e o mesmo n�o � transferido para o c�digo de usu�rio na execu��o desta fun��o
   */
  Connection * getDefaultConnection() const;
   
  /**
   * \brief Define a conex�o "Requester" do contexto corrente.
   * 
   * Define a conex�o "Requester" a ser utilizada em todas as chamadas feitas no contexto
   * atual. Quando 'conn' � 'null' o contexto passa a ficar sem nenhuma conex�o associada.
   * 
   * @param[in] conn Conex�o a ser associada ao contexto corrente. O 'ownership' da conex�o n�o �
   * transferida para o OpenBusContext, e a conex�o deve ser removida do OpenBusContext
   * antes de destruida
   */
  Connection *setCurrentConnection(Connection *);
   
  /**
   * \brief Devolve a conex�o associada ao contexto corrente.
   * 
   * @throw CORBA::Exception
   *
   * @return Conex�o a barramento associada a thread corrente. OpenBusContext n�o possui
   * ownership dessa conex�o e o mesmo n�o � transferido para o c�digo de usu�rio na execu��o
   * desta fun��o
   */
  Connection *getCurrentConnection() const;
       
  /**
   * \brief Devolve a cadeia de chamadas � qual a execu��o corrente pertence.
   * 
   * Caso a contexto corrente (e.g. definido pelo 'CORBA::PICurrent') seja o contexto de execu��o de
   * uma chamada remota oriunda do barramento dessa conex�o, essa opera��o devolve um objeto que
   * representa a cadeia de chamadas do barramento que esta chamada faz parte. Caso contr�rio,
   * devolve uma cadeia de chamadas 'vazia', representada por um CallerChain 'default-constructed'.
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
   * Associa uma cadeia de chamadas ao contexto corrente, de forma que todas as chamadas remotas
   * seguintes neste mesmo contexto sejam feitas como parte dessa cadeia de chamadas.
   * 
   * \param chain Cadeia de chamadas a ser associada ao contexto corrente.
   * @throw CORBA::NO_PERMISSION {minor = NoLoginCode}
   * @throw CORBA::NO_PERMISSION {minor = InvalidChainCode}
   * @throw CORBA::Exception
   */
  void joinChain(CallerChain const& chain);
  
  /**
   * \brief Faz com que nenhuma cadeia de chamadas esteja associada ao contexto corrente.
   * 
   * Remove a associa��o da cadeia de chamadas ao contexto corrente, fazendo com que todas as
   * chamadas seguintes feitas neste mesmo contexto deixem de fazer parte da cadeia de chamadas
   * associada previamente. Ou seja, todas as chamadas passam a iniciar novas cadeias de chamada.
   */
  void exitChain();

  /**
   * \brief Devolve a cadeia de chamadas associada ao contexto corrente.
   * 
   * Devolve um objeto que representa a cadeia de chamadas associada ao contexto corrente nesta
   * conex�o.  A cadeia de chamadas informada foi associada previamente pela opera��o
   * 'joinChain'. Caso o contexto corrente n�o tenha nenhuma cadeia associada, essa opera��o devolve
   * uma cadeia 'vazia' 'default-constructed'
   * 
   * Para verificar se a cadeia retornada � v�lida, o seguinte idioma � usado:
   *
   * CallerChain chain = openbusContext.getCallerChain()
   * if(chain != CallerChain())
   *   // chain � v�lido
   * else
   *   // chain � inv�lido
   * 
   * \return Cadeia de chamadas associada ao contexto corrente ou uma cadeia 'vazia'.
   *
   * @throw CORBA::Exception
   */
  CallerChain getJoinedChain();
  
  /** 
   * ORB utilizado pela conex�o. 
   */
  CORBA::ORB * orb() const {
    return _orb;
  }
  
  idl_or::OfferRegistry_ptr getOfferRegistry() const;
  idl_ac::LoginRegistry_ptr getLoginRegistry() const;
private:
  /**
   * OpenBusContext deve ser adquirido atrav�s de:
   *   orb->resolve_initial_references("OpenBusContext")
   */
  OpenBusContext(CORBA::ORB *, IOP::Codec *, 
                 PortableInterceptor::SlotId slotId_joinedCallChain, 
                 PortableInterceptor::SlotId slotId_signedCallChain, 
                 PortableInterceptor::SlotId slotId_legacyCallChain,
                 PortableInterceptor::SlotId slotId_requesterConnection,
                 PortableInterceptor::SlotId slotId_receiveConnection);
  ~OpenBusContext();
  void orb(CORBA::ORB *o) {
    _orb = o;
  }
  typedef std::map<std::string, Connection*> BusidConnection;
  mutable Mutex _mutex;
  CORBA::ORB *_orb;
  PortableInterceptor::Current_var _piCurrent;
  IOP::Codec *_codec;
  PortableInterceptor::SlotId _slotId_joinedCallChain; 
  PortableInterceptor::SlotId _slotId_signedCallChain;
  PortableInterceptor::SlotId _slotId_legacyCallChain;
  PortableInterceptor::SlotId _slotId_requesterConnection;
  PortableInterceptor::SlotId _slotId_receiveConnection;
  Connection *_defaultConnection;
  BusidConnection _busidConnection;
  CallDispatchCallback _callDispatchCallback;
  friend CORBA::ORB *openbus::ORBInitializer(int& argc, char **argv);
};
}

#endif
