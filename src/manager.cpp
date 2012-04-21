#include <manager.h>

namespace openbus {
  ConnectionManager::ConnectionManager() : _defaultConnection(0) { }
  ConnectionManager::~ConnectionManager() { }
  
  Connection* ConnectionManager::createConnection(const char* host, short port) {
    return new Connection(host, port, orb, _orbInitializer, this);
  }
  
  void ConnectionManager::setupBusDispatcher(Connection* c) {
    _busidConnection[std::string(c->busid())] = c;
  }

  Connection* ConnectionManager::getBusDispatcher(const char* busid) {
    BusidConnection::iterator it = _busidConnection.find(std::string(busid));
    if (it != _busidConnection.end()) return it->second;
    else return 0;
  }

  Connection* ConnectionManager::removeBusDispatcher(const char* busid) {
    BusidConnection::iterator it = _busidConnection.find(std::string(busid));
    if (it != _busidConnection.end()) {
      //[doubt] posso fazer isso?
      _busidConnection.erase(it);
      return it->second;
    } 
    else return 0;    
  }
}