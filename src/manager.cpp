#include <manager.h>

#include <log.h>

namespace openbus {
  ConnectionManager::ConnectionManager() : _defaultConnection(0) {
    log_scope function_scope(log.general_logger(), debug_level
                             , "openbus::ConnectionManager::ConnectionManager");
    #ifdef OPENBUS_SDK_MULTITHREAD
    MICOMT::Thread::create_key(_threadConnectionKey);
    MICOMT::Thread::create_key(_threadConnectionDispatcherKey);
    #else
    _threadConnection = 0;
    _receiveRequestInterceptorConnection = 0;
    #endif
  }

  ConnectionManager::~ConnectionManager() { 
    #ifdef OPENBUS_SDK_MULTITHREAD
    MICOMT::Thread::delete_key(_threadConnectionKey);
    MICOMT::Thread::delete_key(_threadConnectionDispatcherKey);
    #endif
  }

  std::auto_ptr<Connection> ConnectionManager::createConnection(const char* host, short port) {
    int p = port;
    log_scope function_scope(log.general_logger(), debug_level
                             , "openbus::ConnectionManager::createConnection");
    function_scope.vlog("createConnection para host %s:%d", host, p);
    return std::auto_ptr<Connection> (new Connection(host, port, _orb, _orbInitializer, this));
  }

  void ConnectionManager::setDispatcher(Connection* c) {
    log_scope function_scope(log.general_logger(), info_level
                             , "openbus::ConnectionManager::setDispatcher");
    function_scope.vlog("setDispatcher para o barramento %s", c->busid());
    _busidConnection[std::string(c->busid())] = c;
  }

  Connection* ConnectionManager::getDispatcher(const char* busid) {
    log_scope function_scope(log.general_logger(), info_level
                             , "openbus::ConnectionManager::getDispatcher");
    function_scope.vlog("getDispatcher do barramento %s", busid);
    BusidConnection::iterator it = _busidConnection.find(std::string(busid));
    if (it != _busidConnection.end()) return it->second;
    else return 0;
  }

  Connection* ConnectionManager::clearDispatcher(const char* busid) {
    BusidConnection::iterator it = _busidConnection.find(std::string(busid));
    if (it != _busidConnection.end()) {
      Connection* c = it->second;
      _busidConnection.erase(it);
      return c;
    } else return 0;    
  }

  void ConnectionManager::setRequester(Connection* c) { 
    #ifdef OPENBUS_SDK_MULTITHREAD
    MICOMT::Thread::set_specific(_threadConnectionKey, c);
    #else
    _threadConnection = c;
    #endif
  }

  Connection* ConnectionManager::getRequester() { 
    #ifdef OPENBUS_SDK_MULTITHREAD
    return (Connection*) MICOMT::Thread::get_specific(_threadConnectionKey);
    #else
    return _threadConnection;
    #endif
  }
}
