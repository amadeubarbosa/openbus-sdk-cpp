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
    // [todo] 
    #ifdef OPENBUS_SDK_MULTITHREAD
    void setThreadRequester(Connection*) { }
    Connection* getThreadRequester() { return 0; }
    #endif
    void setupBusDispatcher(Connection*);
    Connection* getBusDispatcher(const char* busid);
    Connection* removeBusDispatcher(const char* busid);
    CORBA::ORB* orb() const { return _orb; }
  private:
    void orb(CORBA::ORB* o) { _orb = o; }
    CORBA::ORB* _orb;
    interceptors::ORBInitializer* _orbInitializer;
    BusidConnection _busidConnection;
    Connection* _defaultConnection;
    friend class openbus::Connection;
    friend CORBA::ORB* openbus::initORB(int argc, char** argv) throw(CORBA::Exception);
  };
}

#endif