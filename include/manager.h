#ifndef _TECGRAF_MANAGER_H_
#define _TECGRAF_MANAGER_H_

#include <CORBA.h>

#include "connection.h"
#include "interceptors/orbInitializer_impl.h"

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

namespace openbus {
  typedef std::map<std::string, Connection*> BusidConnection;
  
  class ConnectionManager : public CORBA::LocalObject {
  public:
    ConnectionManager();
    ~ConnectionManager();
    Connection* createConnection(const char* host, short port);
    void setDefaultConnection(Connection* c) { _defaultConnection = c; }
    Connection* getDefaultConnection() { return _defaultConnection; }
    void setupBusDispatcher(Connection*);
    Connection* getBusDispatcher(const char* busid);
    Connection* removeBusDispatcher(const char* busid);
    #ifdef OPENBUS_SDK_MULTITHREAD
    void setThreadRequester(Connection*);
    Connection* getThreadRequester();
    #endif
    CORBA::ORB* orb() const { return _orb; }
  private:
    #ifdef OPENBUS_SDK_MULTITHREAD
    MICOMT::Thread::ThreadKey _threadConnectionKey;
    MICOMT::Thread::ThreadKey _threadConnectionDispatcherKey;
    #endif
    void orb(CORBA::ORB* o) { _orb = o; }
    CORBA::ORB* _orb;
    interceptors::ORBInitializer* _orbInitializer;
    BusidConnection _busidConnection;
    Connection* _defaultConnection;
    Connection* _userDefaultConnection;
    friend class openbus::Connection;
    friend CORBA::ORB* openbus::initORB(int argc, char** argv) throw(CORBA::Exception);
    friend class openbus::interceptors::ClientInterceptor;
    friend class openbus::interceptors::ServerInterceptor;
  };
}

#endif