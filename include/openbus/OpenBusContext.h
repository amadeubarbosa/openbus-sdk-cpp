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
 * Coleção de informações dos logins que originaram chamadas em cadeia através de um
 * barramento. Cadeias de chamadas representam chamadas aninhadas dentro do barramento e são úteis
 * para que os sistemas que recebam essas chamadas possam identificar se a chamada foi originada por
 * entidades autorizadas ou não.
 */
struct CallerChain {
  /**
  * Barramento através do qual as chamadas foram originadas.
  */
  const char *busid() const {
    return _busid.c_str();
  }
  
  /**
  * Lista de informações de login de todas as entidades que realizaram chamadas que originaram a
  * cadeia de chamadas da qual essa chamada está inclusa.  Quando essa lista é vazia isso indica
  * que a chamada não está inclusa em uma cadeia de chamadas.
  * 
  * A ordem da sequência retornada é começando da fonte da cadeia até o penúltimo da cadeia na
  * chamada. Assim, originators()[0], se existir, é quem originou a chamada de cadeia.
  */
  const idl_ac::LoginInfoSeq &originators() const {
    return _originators;
  }
  
  /**
   * Informação de login da entidade que realizou a última chamada da cadeia.
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
   * \brief Cria uma conexão para um barramento.
   * 
   * Cria uma conexão para um barramento. O barramento é indicado por um nome ou endereço de rede
   * e um número de porta, onde os serviços núcleo daquele barramento estão executando.
   * 
   * @param[in] host Endereço ou nome de rede onde os serviços núcleo do barramento estão 
   *            executando.
   * @param[in] port Porta onde os serviços núcleo do barramento estão executando.
   * @param[in] props Lista opcional de propriedades que definem algumas
   *        configurações sobre a forma que as chamadas realizadas ou validadas
   *        com essa conexão são feitas. A seguir são listadas as propriedades
   *        válidas:
   *        - access.key: chave de acesso a ser utiliza internamente para a
   *          geração de credenciais que identificam as chamadas através do
   *          barramento. A chave deve ser uma chave privada RSA de 2048 bits
   *          (256 bytes). Quando essa propriedade não é fornecida, uma chave
   *          de acesso é gerada automaticamente.
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
  std::auto_ptr<Connection> createConnection(const char *host, short port,
                                             std::vector<std::string> props 
                                             = std::vector<std::string>());
   
  /**
   * \brief Define a conexão padrão a ser usada nas chamadas.
   * 
   * Define uma conexão a ser utilizada como "Requester" e "Dispatcher" de chamadas sempre que não
   * houver uma conexão "Requester" e "Dispatcher" específica definida para o caso específico,
   * como é feito através das operações 'setCurrentConnection' e 'setDispatcher'.
   * 
   * @param[in] conn Conexão a ser definida como conexão padrão. O 'ownership' da conexão não é
   * transferida para o OpenBusContext, e a conexão deve ser removida do OpenBusContext
   * antes de destruida
   */
  Connection *setDefaultConnection(Connection *);
   
  /**
   * \brief Devolve a conexão padrão.
   * 
   * Veja operação 'setDefaultConnection'.
   * 
   * \return Conexão definida como conexão padrão. OpenBusContext não possui ownership dessa
   * conexão e o mesmo não é transferido para o código de usuário na execução desta função
   */
  Connection * getDefaultConnection() const;
   
  /**
   * \brief Define a conexão "Requester" do contexto corrente.
   * 
   * Define a conexão "Requester" a ser utilizada em todas as chamadas feitas no contexto
   * atual. Quando 'conn' é 'null' o contexto passa a ficar sem nenhuma conexão associada.
   * 
   * @param[in] conn Conexão a ser associada ao contexto corrente. O 'ownership' da conexão não é
   * transferida para o OpenBusContext, e a conexão deve ser removida do OpenBusContext
   * antes de destruida
   */
  Connection *setCurrentConnection(Connection *);
   
  /**
   * \brief Devolve a conexão associada ao contexto corrente.
   * 
   * @throw CORBA::Exception
   *
   * @return Conexão a barramento associada a thread corrente. OpenBusContext não possui
   * ownership dessa conexão e o mesmo não é transferido para o código de usuário na execução
   * desta função
   */
  Connection *getCurrentConnection() const;
       
  /**
   * \brief Devolve a cadeia de chamadas à qual a execução corrente pertence.
   * 
   * Caso a contexto corrente (e.g. definido pelo 'CORBA::PICurrent') seja o contexto de execução de
   * uma chamada remota oriunda do barramento dessa conexão, essa operação devolve um objeto que
   * representa a cadeia de chamadas do barramento que esta chamada faz parte. Caso contrário,
   * devolve uma cadeia de chamadas 'vazia', representada por um CallerChain 'default-constructed'.
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
   * Remove a associação da cadeia de chamadas ao contexto corrente, fazendo com que todas as
   * chamadas seguintes feitas neste mesmo contexto deixem de fazer parte da cadeia de chamadas
   * associada previamente. Ou seja, todas as chamadas passam a iniciar novas cadeias de chamada.
   */
  void exitChain();

  /**
   * \brief Devolve a cadeia de chamadas associada ao contexto corrente.
   * 
   * Devolve um objeto que representa a cadeia de chamadas associada ao contexto corrente nesta
   * conexão.  A cadeia de chamadas informada foi associada previamente pela operação
   * 'joinChain'. Caso o contexto corrente não tenha nenhuma cadeia associada, essa operação devolve
   * uma cadeia 'vazia' 'default-constructed'
   * 
   * Para verificar se a cadeia retornada é válida, o seguinte idioma é usado:
   *
   * CallerChain chain = openbusContext.getCallerChain()
   * if(chain != CallerChain())
   *   // chain é válido
   * else
   *   // chain é inválido
   * 
   * \return Cadeia de chamadas associada ao contexto corrente ou uma cadeia 'vazia'.
   *
   * @throw CORBA::Exception
   */
  CallerChain getJoinedChain();
  
  /** 
   * ORB utilizado pela conexão. 
   */
  CORBA::ORB * orb() const {
    return _orb;
  }
  
  idl_or::OfferRegistry_ptr getOfferRegistry() const;
  idl_ac::LoginRegistry_ptr getLoginRegistry() const;
private:
  /**
   * OpenBusContext deve ser adquirido através de:
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
