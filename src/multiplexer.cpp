#include <multiplexer.h>

namespace openbus {
  namespace multiplexed {
    ConnectionMultiplexer::ConnectionMultiplexer() : _outConnection(0) { }
    
    void ConnectionMultiplexer::setCurrentConnection(Connection* c) {
      _outConnection = c;
    }

    Connection* ConnectionMultiplexer::getCurrentConnection() {
      return _outConnection;
    }

    void ConnectionMultiplexer::setIncomingConnection(const char* busid, Connection* c) {
      _busidConnection[std::string(busid)] = c;
    }

    Connection* ConnectionMultiplexer::getIncomingConnection(const char* busid) {
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
