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
   * \brief Gerencia conexões de acesso a barramentos OpenBus através de um ORB.
   *
   * Conexões representam formas diferentes de acesso ao barramento. O OpenBusContext permite
   * criar essas conexões e gerenciá-las, indicando quais são utilizadas em cada chamada. As
   * conexões são usadas basicamente de duas formas no tratamento das chamadas: 
   * - para realizar uma chamada remota (cliente), neste caso a conexão é denominada "Requester".
   * - para validar uma chamada recebida (servidor), neste caso a conexão é denominada "Dispatcher".
   */
  class OpenBusContext : public CORBA::LocalObject {
  public:
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
     * @throw InvalidBusAddress Os parâmetros 'host' e 'port' não são válidos.
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
     * como é feito através das operações 'setRequester' e 'setDispatcher'.
     * 
     * @param[in] conn Conexão a ser definida como conexão padrão. O 'ownership' da conexão não é
     * transferida para o OpenBusContext, e a conexão deve ser removida do OpenBusContext
     * antes de destruida
     */
    void setDefaultConnection(Connection *conn) { _defaultConnection = conn; }
    
    /**
     * \brief Devolve a conexão padrão.
     * 
     * Veja operação 'setDefaultConnection'.
     * 
     * \return Conexão definida como conexão padrão. OpenBusContext não possui ownership dessa
     * conexão e o mesmo não é transferido para o código de usuário na execução desta função
     */
    Connection * getDefaultConnection() const { return _defaultConnection; }
    
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
    void setRequester(Connection *conn);
    
    /**
     * \brief Devolve a conexão associada ao contexto corrente.
     * 
     * @throw CORBA::Exception
     *
     * @return Conexão a barramento associada a thread corrente. OpenBusContext não possui
     * ownership dessa conexão e o mesmo não é transferido para o código de usuário na execução
     * desta função
     */
    Connection *getRequester() const;
    
    /**
     * \brief Define uma a conexão como "Dispatcher" de barramento.
     * 
     * Define a conexão como "Dispatcher" do barramento ao qual ela está conectada, de forma que
     * todas as chamadas originadas por entidades conectadas a este barramento serão validadas com
     * essa conexão. Só pode haver uma conexão "Dispatcher" para cada barramento, portanto se já
     * houver outra conexão "Dispatcher" para o mesmo barramento essa será substituída pela nova
     * conexão.
     * 
     * @param[in] conn Conexão a ser definida como "Dispatcher". O 'ownership' da conexão não é
     * transferida para o OpenBusContext, e a conexão deve ser removida do OpenBusContext
     * antes de destruida
     */
    void setDispatcher(Connection &conn);
    
    /**
     * \brief Devolve a conexão "Dispatcher" do barramento indicado.
     * 
     * @param[in] busid Identificador do barramento ao qual a conexão está
     * associada. OpenBusContext não possui ownership dessa conexão e o mesmo não é transferido
     * para o código de usuário na execução desta função
     * 
     * @return Conexão "Dispatcher" do barramento indicado, ou 'null' caso não haja nenhuma conexão
     *         "Dispatcher" associada ao barramento indicado. OpenBusContext não possui ownership
     *         dessa conexão e o mesmo não é transferido para o código de usuário na execução desta
     *         função
     */
    Connection * getDispatcher(const char *busid);
    
    /**
     * \brief Remove a conexão "Dispatcher" associada ao barramento indicado.
     * 
     * \param busid Identificador do barramento ao qual a conexão está associada.
     * 
     * \return Conexão "Dispatcher" associada ao barramento ou 'null' se não houver nenhuma conexão
     *         associada. OpenBusContext não possui ownership dessa conexão e o mesmo não é
     *         transferido para o código de usuário na execução desta função
     */
    
    /**
     * \brief Devolve a conexão "Dispatcher" do barramento indicado.
     * 
     * \param busid Identificador do barramento ao qual a conexão está associada. OpenBusContext
     * não possui ownership dessa conexão e o mesmo não é transferido para o código de usuário na
     * execução desta função
     * 
     * \return Conexão "Dispatcher" do barramento indicado, ou 'null' caso não haja nenhuma conexão
     *         "Dispatcher" associada ao barramento indicado.
     */
    Connection * clearDispatcher(const char *busid);
    
    /** 
     * ORB utilizado pela conexão. 
     */
    CORBA::ORB * orb() const { return _orb; }
  private:
    /**
    * OpenBusContext deve ser adquirido através de:
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
