/**
* API - SDK Openbus C++
* \file manager.h
*/

#ifndef _TECGRAF_MANAGER_H_
#define _TECGRAF_MANAGER_H_

#include <CORBA.h>
#include <stdexcept>

#include <connection.h>
#include <interceptors/orbInitializer_impl.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include "util/autolock_impl.h"
#endif

#define CONNECTION_MANAGER_ID "OpenbusConnectionManager"

/* forward declarations */
namespace openbus {
  class Connection;
  namespace interceptors {
	class ClientInterceptor;
	class ServerInterceptor;
    class ORBInitializer;
  }
}

/**
* \brief openbus
*/
namespace openbus {
  struct NotLoggedIn  { const char* name() const { return "openbus::NotLoggedIn"; } };

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
    std::auto_ptr<Connection> createConnection(const char* host, short port) 
      throw (CORBA::Exception); 
      
    /**
 	  * Define a conexão a ser utilizada nas chamadas realizadas e no despacho de chamadas recebidas 
 	  * sempre que não houver uma conexão específica definida. Sempre que não houver uma conexão 
 	  * associada tanto as chamadas realizadas como as chamadas recebidas são negadas com a exceção 
 	  * CORBA::NO_PERMISSION.
 	  *
 	  * @param[in] conn Conexão a ser definida como conexão default.
  	*/
    void setDefaultConnection(Connection* conn) { _defaultConnection = conn; }
    
    /**
	  * Obtém a conexão a ser utilizada nas chamadas realizadas e no despacho de chamadas recebidas 
	  * sempre que não houver uma conexão específica definida.
	  * 
	  * @return Conexão definida como conexão default.
	  */
    Connection* getDefaultConnection() const { return _defaultConnection; }
    
    /**
	  * Define a conexão com o barramento a ser utilizada em todas as chamadas feitas pela thread 
	  * corrente. Quando a conexão passada é zero a thread passa a ficar sem nenhuma conexão 
	  * associada.
	  * 
	  * @throw CORBA::Exception
	  *
	  * @param[in] Conexão a barramento a ser associada a thread corrente.
	  */
    void setRequester(Connection*) throw (CORBA::Exception);
    
    /**
	  * Devolve a conexão com o barramento associada a thread corrente, ou zero caso não haja 
	  * nenhuma conexão associada à thread.
	  * 
	  * @throw CORBA::Exception
	  *
	  * @return Conexão a barramento associada a thread corrente.
  	*/
    Connection* getRequester() const throw (CORBA::Exception);
    
	  /**
	  * Define que conexão deve ser utilizada para receber chamadas oriundas do barramento ao qual 
	  * está conectada e autenticada, denominada conexão de despacho.
	  * 
	  * @param[in] Conexão a barramento a ser associada a thread corrente.
    *
	  * @throw NotLoggedIn A conexão não está autenticada ou é nula.
  	*/
    void setDispatcher(Connection&) throw (NotLoggedIn);
    
	  /**
	  * Devolve a conexão de despacho associada ao barramento indicado, se houver. dado barramento, 
	  * ou zero caso não haja nenhuma conexão associada ao barramento.
	  * 
	  * @param[in] busid Identificador do barramento ao qual a conexão está associada.
	  * @return Conexão a barramento associada ao barramento.
	  */
    Connection* getDispatcher(const char* busid);
    
	  /**
	  * Remove a conexão de despacho associada ao barramento indicado, se houver.
	  * 
	  * @return Conexão a barramento associada ao barramento ou zero se não houver nenhuma conexão
	  * associada.
	  */
    Connection* clearDispatcher(const char* busid);
    
	  /** 
	  * ORB utilizado pela conexão. 
	  */
    CORBA::ORB* orb() const { return _orb; }
  private:
    MICOMT::Mutex _mutex;
    typedef std::map<std::string, Connection*> BusidConnection;
    ConnectionManager(CORBA::ORB*, interceptors::ORBInitializer*);
    ~ConnectionManager();
    void orb(CORBA::ORB* o) { _orb = o; }
    CORBA::ORB* _orb;
    PortableInterceptor::Current_var _piCurrent;
    interceptors::ORBInitializer* _orbInitializer;
    BusidConnection _busidConnection;
    Connection* _defaultConnection;
    friend class openbus::Connection;
    friend CORBA::ORB* openbus::initORB(int argc, char** argv) throw(CORBA::Exception);
    friend class openbus::interceptors::ClientInterceptor;
    friend class openbus::interceptors::ServerInterceptor;
  };
}

#endif
