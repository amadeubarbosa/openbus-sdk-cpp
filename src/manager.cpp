#include <manager.h>
#include <log.h>

namespace openbus {
ConnectionManager::ConnectionManager(CORBA::ORB* o, interceptors::ORBInitializer* i) 
 : _orb(o), _orbInitializer(i), _defaultConnection(0) 
{
  log_scope l(log.general_logger(), debug_level, "ConnectionManager::ConnectionManager");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent));
}

ConnectionManager::~ConnectionManager() { }

std::auto_ptr<Connection> ConnectionManager::createConnection(const char* host, short port) 
  throw (CORBA::Exception)
{
  log_scope l(log.general_logger(), debug_level, "ConnectionManager::createConnection");
  l.vlog("createConnection para host %s:%hi", host, port);
  return std::auto_ptr<Connection> (new Connection(host, port, _orb, _orbInitializer, this));
}

void ConnectionManager::setRequester(Connection* c) throw (CORBA::Exception) {
  log_scope l(log.general_logger(), info_level, "ConnectionManager::setRequester");
  l.vlog("connection:%p", c);
  size_t size = sizeof(Connection*);
  unsigned char buf[size];
  memcpy(buf, &c, size);
  idl::OctetSeq connectionAddrOctetSeq(size, size, buf);
  CORBA::Any connectionAddrAny;
  connectionAddrAny <<= connectionAddrOctetSeq;
  _piCurrent->set_slot(_orbInitializer->slotId_connectionAddr(), connectionAddrAny);
}

Connection* ConnectionManager::getRequester() const throw (CORBA::Exception) { 
  log_scope l(log.general_logger(), info_level, "ConnectionManager::getRequester");
  CORBA::Any_var connectionAddrAny=_piCurrent->get_slot(_orbInitializer->slotId_connectionAddr());
  idl::OctetSeq connectionAddrOctetSeq;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) {
    assert(connectionAddrOctetSeq.length() == sizeof(Connection*));
    Connection* c;
    std::memcpy(&c, connectionAddrOctetSeq.get_buffer(), sizeof(Connection*));
    return c;
  } else return 0;
}

void ConnectionManager::setDispatcher(Connection& c) throw (NotLoggedIn) {
  log_scope l(log.general_logger(), info_level, "ConnectionManager::setDispatcher");
  AutoLock m(&_mutex);
  if (c.busid()) _busidConnection[std::string(c.busid())] = &c;
  else throw NotLoggedIn();
}

Connection* ConnectionManager::getDispatcher(const char* busid) {
  log_scope l(log.general_logger(), info_level, "ConnectionManager::getDispatcher");
  l.vlog("getDispatcher do barramento %s", busid);
  if (!busid) return 0;
  AutoLock m(&_mutex);
  BusidConnection::iterator it = _busidConnection.find(std::string(busid));
  if (it != _busidConnection.end()) return it->second;
  else return 0;
}

Connection* ConnectionManager::clearDispatcher(const char* busid) {
  log_scope l(log.general_logger(), info_level, "ConnectionManager::clearDispatcher");
  l.vlog("clearDispatcher para a conexão [busid:%s]", busid);
  if (!busid) return 0;
  AutoLock m(&_mutex);
  BusidConnection::iterator it = _busidConnection.find(std::string(busid));
  if (it != _busidConnection.end()) {
    Connection* c = it->second;
    _busidConnection.erase(it);
    return c;
  } else return 0;    
}
}
