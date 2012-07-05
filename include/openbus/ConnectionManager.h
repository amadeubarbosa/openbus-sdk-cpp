/**
* API - SDK Openbus C++
* \file openbus/ConnectionManager.h
*/

#ifndef _TECGRAF_MANAGER_H_
#define _TECGRAF_MANAGER_H_

#include <CORBA.h>

namespace openbus {
  class ConnectionManager;
}

#include "openbus/interceptors/ORBInitializer_impl.h"
#include "openbus/Connection.h"

#define CONNECTION_MANAGER_ID "OpenbusConnectionManager"

/**
* \brief openbus
*/
namespace openbus {
  /* exceptions */
  struct NotLoggedIn  { const char *name() const { return "openbus::NotLoggedIn"; } };
  /**/
  
 /**
 * \brief Interface com operações para gerenciar o acesso a barramentos OpenBus através de um ORB 
 *  CORBA.
 */
  class ConnectionManager : public CORBA::LocalObject {
  public:
    /**
	  * Cria uma conexão para um barramento a partir de um endereço de rede IP e uma porta.
	  * 
	  * @param[in] host Endereço de rede IP onde o barramento está executando.
	  * @param[in] port Porta do processo do barramento no endereço indicado.
	  * 
	  * @throw CORBA::Exception
	  *
	  * @return Conexão ao barramento referenciado.
	  */
    std::auto_ptr<Connection> createConnection(const char *host, short port);
    
    /**
 	  * Define a conexão a ser utilizada nas chamadas realizadas e no despacho de chamadas recebidas 
 	  * sempre que não houver uma conexão específica definida. Sempre que não houver uma conexão 
 	  * associada tanto as chamadas realizadas como as chamadas recebidas são negadas com a exceção 
 	  * CORBA::NO_PERMISSION.
 	  *
 	  * @param[in] conn Conexão a ser definida como conexão default.
  	*/
    void setDefaultConnection(Connection *conn) { _defaultConnection = conn; }
    
    /**
	  * Obtém a conexão a ser utilizada nas chamadas realizadas e no despacho de chamadas recebidas 
	  * sempre que não houver uma conexão específica definida.
	  * 
	  * @return Conexão definida como conexão default.
	  */
    Connection *getDefaultConnection() const { return _defaultConnection; }
    
    /**
	  * Define a conexão com o barramento a ser utilizada em todas as chamadas feitas pela thread 
	  * corrente. Quando a conexão passada é zero a thread passa a ficar sem nenhuma conexão 
	  * associada.
	  * 
	  * @throw CORBA::Exception
	  *
	  * @param[in] Conexão a barramento a ser associada a thread corrente.
	  */
    void setRequester(Connection*);
    
    /**
	  * Devolve a conexão com o barramento associada a thread corrente, ou zero caso não haja 
	  * nenhuma conexão associada à thread.
	  * 
	  * @throw CORBA::Exception
	  *
	  * @return Conexão a barramento associada a thread corrente.
  	*/
    Connection *getRequester() const;
    
	  /**
	  * Define que conexão deve ser utilizada para receber chamadas oriundas do barramento ao qual 
	  * está conectada e autenticada, denominada conexão de despacho.
	  * 
	  * @param[in] Conexão a barramento a ser associada a thread corrente.
    *
	  * @throw NotLoggedIn A conexão não está autenticada ou é nula.
  	*/
    void setDispatcher(Connection&);
    
	  /**
	  * Devolve a conexão de despacho associada ao barramento indicado, se houver. dado barramento, 
	  * ou zero caso não haja nenhuma conexão associada ao barramento.
	  * 
	  * @param[in] busid Identificador do barramento ao qual a conexão está associada.
	  * @return Conexão a barramento associada ao barramento.
	  */
    Connection *getDispatcher(const char *busid);
    
	  /**
	  * Remove a conexão de despacho associada ao barramento indicado, se houver.
	  * 
	  * @return Conexão a barramento associada ao barramento ou zero se não houver nenhuma conexão
	  * associada.
	  */
    Connection *clearDispatcher(const char *busid);
    
	  /** 
	  * ORB utilizado pela conexão. 
	  */
    CORBA::ORB *orb() const { return _orb; }
  private:
    /**
    * ConnectionManager deve ser adquirido através de:
    *   orb->resolve_initial_references(CONNECTION_MANAGER_ID)
    */
    ConnectionManager(CORBA::ORB*, IOP::Codec*, 
      PortableInterceptor::SlotId slotId_joinedCallChain, 
      PortableInterceptor::SlotId slotId_signedCallChain, 
      PortableInterceptor::SlotId slotId_connectionAddr,
      PortableInterceptor::SlotId slotId_legacyCallChain);
    ~ConnectionManager();
    void orb(CORBA::ORB *o) { _orb = o; }
    typedef std::map<std::string, Connection*> BusidConnection;
    MICOMT::Mutex _mutex;
    CORBA::ORB *_orb;
    PortableInterceptor::Current_var _piCurrent;
    IOP::Codec *_codec;
    PortableInterceptor::SlotId _slotId_joinedCallChain; 
    PortableInterceptor::SlotId _slotId_signedCallChain;
    PortableInterceptor::SlotId _slotId_legacyCallChain;
    PortableInterceptor::SlotId _slotId_connectionAddr;
    Connection *_defaultConnection;
    BusidConnection _busidConnection;
    friend CORBA::ORB *openbus::ORBInitializer(int argc, char **argv);
  };
}

#endif
