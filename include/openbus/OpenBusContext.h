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
}

#include "openbus/interceptors/ORBInitializer_impl.h"
#include "openbus/Connection.h"

#define OPENBUS_CONTEXT_ID "OpenBusContext"

/**
* \brief openbus
*/
namespace openbus {
  /**
   * \brief Gerencia conex�es de acesso a barramentos OpenBus atrav�s de um ORB.
   *
   * Conex�es representam formas diferentes de acesso ao barramento. O OpenBusContext permite
   * criar essas conex�es e gerenci�-las, indicando quais s�o utilizadas em cada chamada. As
   * conex�es s�o usadas basicamente de duas formas no tratamento das chamadas: 
   * - para realizar uma chamada remota (cliente), neste caso a conex�o � denominada "Requester".
   * - para validar uma chamada recebida (servidor), neste caso a conex�o � denominada "Dispatcher".
   */
  class OpenBusContext : public CORBA::LocalObject {
  public:
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
     * @throw InvalidBusAddress Os par�metros 'host' e 'port' n�o s�o v�lidos.
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
     * como � feito atrav�s das opera��es 'setRequester' e 'setDispatcher'.
     * 
     * @param[in] conn Conex�o a ser definida como conex�o padr�o. O 'ownership' da conex�o n�o �
     * transferida para o OpenBusContext, e a conex�o deve ser removida do OpenBusContext
     * antes de destruida
     */
    void setDefaultConnection(Connection *conn) { _defaultConnection = conn; }
    
    /**
     * \brief Devolve a conex�o padr�o.
     * 
     * Veja opera��o 'setDefaultConnection'.
     * 
     * \return Conex�o definida como conex�o padr�o. OpenBusContext n�o possui ownership dessa
     * conex�o e o mesmo n�o � transferido para o c�digo de usu�rio na execu��o desta fun��o
     */
    Connection * getDefaultConnection() const { return _defaultConnection; }
    
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
    void setRequester(Connection *conn);
    
    /**
     * \brief Devolve a conex�o associada ao contexto corrente.
     * 
     * @throw CORBA::Exception
     *
     * @return Conex�o a barramento associada a thread corrente. OpenBusContext n�o possui
     * ownership dessa conex�o e o mesmo n�o � transferido para o c�digo de usu�rio na execu��o
     * desta fun��o
     */
    Connection *getRequester() const;
    
    /**
     * \brief Define uma a conex�o como "Dispatcher" de barramento.
     * 
     * Define a conex�o como "Dispatcher" do barramento ao qual ela est� conectada, de forma que
     * todas as chamadas originadas por entidades conectadas a este barramento ser�o validadas com
     * essa conex�o. S� pode haver uma conex�o "Dispatcher" para cada barramento, portanto se j�
     * houver outra conex�o "Dispatcher" para o mesmo barramento essa ser� substitu�da pela nova
     * conex�o.
     * 
     * @param[in] conn Conex�o a ser definida como "Dispatcher". O 'ownership' da conex�o n�o �
     * transferida para o OpenBusContext, e a conex�o deve ser removida do OpenBusContext
     * antes de destruida
     */
    void setDispatcher(Connection &conn);
    
    /**
     * \brief Devolve a conex�o "Dispatcher" do barramento indicado.
     * 
     * @param[in] busid Identificador do barramento ao qual a conex�o est�
     * associada. OpenBusContext n�o possui ownership dessa conex�o e o mesmo n�o � transferido
     * para o c�digo de usu�rio na execu��o desta fun��o
     * 
     * @return Conex�o "Dispatcher" do barramento indicado, ou 'null' caso n�o haja nenhuma conex�o
     *         "Dispatcher" associada ao barramento indicado. OpenBusContext n�o possui ownership
     *         dessa conex�o e o mesmo n�o � transferido para o c�digo de usu�rio na execu��o desta
     *         fun��o
     */
    Connection * getDispatcher(const char *busid);
    
    /**
     * \brief Remove a conex�o "Dispatcher" associada ao barramento indicado.
     * 
     * \param busid Identificador do barramento ao qual a conex�o est� associada.
     * 
     * \return Conex�o "Dispatcher" associada ao barramento ou 'null' se n�o houver nenhuma conex�o
     *         associada. OpenBusContext n�o possui ownership dessa conex�o e o mesmo n�o �
     *         transferido para o c�digo de usu�rio na execu��o desta fun��o
     */
    
    /**
     * \brief Devolve a conex�o "Dispatcher" do barramento indicado.
     * 
     * \param busid Identificador do barramento ao qual a conex�o est� associada. OpenBusContext
     * n�o possui ownership dessa conex�o e o mesmo n�o � transferido para o c�digo de usu�rio na
     * execu��o desta fun��o
     * 
     * \return Conex�o "Dispatcher" do barramento indicado, ou 'null' caso n�o haja nenhuma conex�o
     *         "Dispatcher" associada ao barramento indicado.
     */
    Connection * clearDispatcher(const char *busid);
    
    /** 
     * ORB utilizado pela conex�o. 
     */
    CORBA::ORB * orb() const { return _orb; }
  private:
    /**
    * OpenBusContext deve ser adquirido atrav�s de:
    *   orb->resolve_initial_references(OPENBUS_CONTEXT_ID)
    */
    OpenBusContext(CORBA::ORB *, IOP::Codec *, 
      PortableInterceptor::SlotId slotId_joinedCallChain, 
      PortableInterceptor::SlotId slotId_signedCallChain, 
      PortableInterceptor::SlotId slotId_legacyCallChain,
      PortableInterceptor::SlotId slotId_requesterConnection,
      PortableInterceptor::SlotId slotId_receiveConnection);
    ~OpenBusContext();
    void orb(CORBA::ORB *o) { _orb = o; }
    typedef std::map<std::string, Connection*> BusidConnection;
    Mutex _mutex;
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
    friend CORBA::ORB *openbus::ORBInitializer(int argc, char **argv);
  };
}

#endif
