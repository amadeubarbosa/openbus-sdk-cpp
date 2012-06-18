#ifndef _TECGRAF_MANAGER_H_
#define _TECGRAF_MANAGER_H_

#include <CORBA.h>
#include <stdexcept>

#include <connection.h>
#include <interceptors/orbInitializer_impl.h>

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
  struct NotLoggedIn  { const char* name() const { return "openbus::NotLoggedIn"; } };

  typedef std::map<std::string, Connection*> BusidConnection;
  
  class ConnectionManager : public CORBA::LocalObject {
  public:
    std::auto_ptr<Connection> createConnection(const char* host, short port) 
      throw (CORBA::Exception); 
    void setDefaultConnection(Connection* c) { _defaultConnection = c; }
    Connection* getDefaultConnection() const { return _defaultConnection; }
    void setRequester(Connection*) throw (CORBA::Exception);
    Connection* getRequester() throw (CORBA::Exception);
    void setDispatcher(Connection*) throw (NotLoggedIn);
    Connection* getDispatcher(const char* busid);
    Connection* clearDispatcher(const char* busid);
    CORBA::ORB* orb() const { return _orb; }
  private:
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
