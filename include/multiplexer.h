#ifndef  _TECGRAF_MULTIPLEXER_H_
#define _TECGRAF_MULTIPLEXER_H_

#include "connection.h"
#include "interceptors/clientInterceptor_impl.h"

namespace openbus {
  namespace multiplexed {
    typedef std::list<Connection*> Connections;
    typedef std::map<std::string, Connection*> BusidConnection;

    class ConnectionMultiplexer {
    public:
      ConnectionMultiplexer() {}
      ~ConnectionMultiplexer() {}
      Connections getConnections();
      void setCurrentConnection(Connection*);
      Connection* getCurrentConnection();
      void setIncommingConnection(const char* busid, Connection*);
      Connection* getIncommingConnection(const char* busid);
    private:
      void addConnection(Connection*);
      void removeConnection(Connection*);
      Connections _connections;
      BusidConnection _busidConnection;
      Connection* _outConnection;
      friend class Connection;
    };
  }
}

#endif
