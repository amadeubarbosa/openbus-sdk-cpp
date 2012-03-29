#ifndef _TECGRAF_MULTIPLEXER_H_
#define _TECGRAF_MULTIPLEXER_H_

#include <CORBA.h>

#include "connection.h"
#include "interceptors/clientInterceptor_impl.h"

#define CONNECTION_MULTIPLEXER_ID "ConnectionMultiplexer"

/* forward declarations */
namespace openbus {
  class Connection;
}

namespace openbus {
  namespace multiplexed {
    typedef std::list<Connection*> Connections;
    typedef std::map<std::string, Connection*> BusidConnection;

    class ConnectionMultiplexer : public CORBA::LocalObject {
    public:
      ConnectionMultiplexer();
      ~ConnectionMultiplexer() {}
      Connections getConnections() { return _connections; }
      void setCurrentConnection(Connection*);
      Connection* getCurrentConnection();
      void setIncomingConnection(const char* busid, Connection*);
      Connection* getIncomingConnection(const char* busid);
    private:
      void addConnection(Connection*);
      void removeConnection(Connection*);
      Connections _connections;
      BusidConnection _busidConnection;
      Connection* _outConnection;
      friend class openbus::Connection;
    };
  }
}

#endif
