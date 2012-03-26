#include <multiplexer.h>

namespace openbus {
  namespace multiplexed {
    Connections ConnectionMultiplexer::getConnections() {
      return _connections;
    }

    void ConnectionMultiplexer::setCurrentConnection(Connection* c) {
      _outConnection = c;
    }

    Connection* ConnectionMultiplexer::getCurrentConnection() {
      return _outConnection;
    }

    void ConnectionMultiplexer::setIncommingConnection(const char* busid, Connection* c) {
      _busidConnection[std::string(busid)] = c;
    }

    Connection* ConnectionMultiplexer::getIncommingConnection(const char* busid) {
      BusidConnection::iterator it = _busidConnection.find(std::string(busid));
      if (it != _busidConnection.end())
        return it->second;
      else
        return 0;
    }

    void ConnectionMultiplexer::addConnection(Connection* c) {
      _connections.push_back(c);
    }

    void ConnectionMultiplexer::removeConnection(Connection* c) {
      _connections.remove(c);
    }
  }
}
