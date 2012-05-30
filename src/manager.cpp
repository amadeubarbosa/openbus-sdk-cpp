#include <manager.h>
#include <log.h>

namespace openbus {
  ConnectionManager::ConnectionManager(CORBA::ORB* o, interceptors::ORBInitializer* i) 
  : _orb(o), _orbInitializer(i), _defaultConnection(0) {
    log_scope l(log.general_logger(), debug_level, "openbus::ConnectionManager::ConnectionManager");
    CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
    assert(!CORBA::is_nil(init_ref));
    _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  }

  ConnectionManager::~ConnectionManager() { }

  std::auto_ptr<Connection> ConnectionManager::createConnection(const char* host, short port) {
    int p = port;
    log_scope l(log.general_logger(), debug_level, "openbus::ConnectionManager::createConnection");
    l.vlog("createConnection para host %s:%d", host, p);
    return std::auto_ptr<Connection> (new Connection(host, port, _orb, _orbInitializer, this));
  }

  void ConnectionManager::setDispatcher(Connection* c) {
    log_scope l(log.general_logger(), info_level, "openbus::ConnectionManager::setDispatcher");
    _busidConnection[std::string(c->busid())] = c;
  }

  Connection* ConnectionManager::getDispatcher(const char* busid) {
    log_scope l(log.general_logger(), info_level, "openbus::ConnectionManager::getDispatcher");
    l.vlog("getDispatcher do barramento %s", busid);
    BusidConnection::iterator it = _busidConnection.find(std::string(busid));
    if (it != _busidConnection.end()) return it->second;
    else return 0;
  }

  Connection* ConnectionManager::clearDispatcher(const char* busid) {
    log_scope l(log.general_logger(), info_level, "openbus::ConnectionManager::clearDispatcher");
    l.vlog("clearDispatcher para a conex�o [busid:%s]", busid);
    BusidConnection::iterator it = _busidConnection.find(std::string(busid));
    if (it != _busidConnection.end()) {
      Connection* c = it->second;
      _busidConnection.erase(it);
      return c;
    } else return 0;    
  }

  void ConnectionManager::setRequester(Connection* c) {
    log_scope l(log.general_logger(), info_level, "openbus::ConnectionManager::setRequester");
    size_t size = sizeof(Connection*);
    unsigned char buf[size];
    memcpy(buf, &c, size);
    idl::OctetSeq_var connectionAddrOctetSeq = new idl::OctetSeq(size, size, buf);
    CORBA::Any connectionAddrAny;
    connectionAddrAny <<= *(connectionAddrOctetSeq);
    _piCurrent->set_slot(_orbInitializer->slotId_connectionAddr(), connectionAddrAny);
  }

  Connection* ConnectionManager::getRequester() { 
    log_scope l(log.general_logger(), info_level, "openbus::ConnectionManager::getRequester");
    CORBA::Any_var connectionAddrAny=_piCurrent->get_slot(_orbInitializer->slotId_connectionAddr());
    idl::OctetSeq connectionAddrOctetSeq;
    if (*connectionAddrAny >>= connectionAddrOctetSeq) {
      assert(connectionAddrOctetSeq.length() == sizeof(Connection*));
      Connection* c;
      std::memcpy(&c, connectionAddrOctetSeq.get_buffer(), sizeof(Connection*));
      return c;
    } else return 0;
  }
}
