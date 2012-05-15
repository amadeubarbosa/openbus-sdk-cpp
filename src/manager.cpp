#include <manager.h>

namespace openbus {
  ConnectionManager::ConnectionManager() : _defaultConnection(0), _userDefaultConnection(0) { 
    MICOMT::Thread::create_key(_threadConnectionKey);
    MICOMT::Thread::create_key(_threadConnectionDispatcherKey);
  }
  
  ConnectionManager::~ConnectionManager() { 
    MICOMT::Thread::delete_key(_threadConnectionKey);
    MICOMT::Thread::delete_key(_threadConnectionDispatcherKey);
  }
  
  Connection* ConnectionManager::createConnection(const char* host, short port) {
    return new Connection(host, port, _orb, _orbInitializer, this);
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
    } else return 0;    
  }
  
  #ifdef OPENBUS_SDK_MULTITHREAD
  void ConnectionManager::setThreadRequester(Connection* c) { 
    MICOMT::Thread::set_specific(_threadConnectionKey, c);
  }

  Connection* ConnectionManager::getThreadRequester() { 
    return (Connection*) MICOMT::Thread::get_specific(_threadConnectionKey);
  }
  #endif
}
