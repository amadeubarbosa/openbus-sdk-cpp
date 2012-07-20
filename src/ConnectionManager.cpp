#include "openbus/ConnectionManager.h"
#include "openbus/log.h"
#ifdef OPENBUS_SDK_MULTITHREAD
#include "openbus/util/AutoLock_impl.h"
#endif

namespace openbus {
ConnectionManager::ConnectionManager(
  CORBA::ORB *o, IOP::Codec *c, 
  PortableInterceptor::SlotId s1, 
  PortableInterceptor::SlotId s2, 
  PortableInterceptor::SlotId s3,
  PortableInterceptor::SlotId s4) 
  : _orb(o), _codec(c), _slotId_joinedCallChain(s1), _slotId_signedCallChain(s2), 
  _slotId_legacyCallChain(s3), _slotId_connectionAddr(s4), _defaultConnection(0)
{
  log_scope l(log.general_logger(), debug_level, "ConnectionManager::ConnectionManager");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent));
}

ConnectionManager::~ConnectionManager() { }

std::auto_ptr<Connection> ConnectionManager::createConnection(const char *host, short port, 
  std::vector<std::string> props)
{
  log_scope l(log.general_logger(), debug_level, "ConnectionManager::createConnection");
  l.vlog("createConnection para host %s:%hi", host, port);
  return std::auto_ptr<Connection> (new Connection(host, port, _orb, _codec, 
    _slotId_joinedCallChain, _slotId_signedCallChain, _slotId_legacyCallChain, this, props));
}

void ConnectionManager::setRequester(Connection *c) {
  log_scope l(log.general_logger(), info_level, "ConnectionManager::setRequester");
  l.vlog("connection:%p", c);
  size_t size = sizeof(Connection*);
  unsigned char buf[size];
  memcpy(buf, &c, size);
  idl::OctetSeq connectionAddrOctetSeq(size, size, buf);
  CORBA::Any connectionAddrAny;
  connectionAddrAny <<= connectionAddrOctetSeq;
  _piCurrent->set_slot(_slotId_connectionAddr, connectionAddrAny);
}

Connection *ConnectionManager::getRequester() const { 
  log_scope l(log.general_logger(), info_level, "ConnectionManager::getRequester");
  CORBA::Any_var connectionAddrAny=_piCurrent->get_slot(_slotId_connectionAddr);
  idl::OctetSeq connectionAddrOctetSeq;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) {
    assert(connectionAddrOctetSeq.length() == sizeof(Connection*));
    Connection *c;
    std::memcpy(&c, connectionAddrOctetSeq.get_buffer(), sizeof(Connection*));
    return c;
  } else return 0;
}

void ConnectionManager::setDispatcher(Connection &c) {
  log_scope l(log.general_logger(), info_level, "ConnectionManager::setDispatcher");
  AutoLock m(&_mutex);
  if (c._busid) _busidConnection[std::string(c._busid)] = &c;
}

Connection *ConnectionManager::getDispatcher(const char *busid) {
  log_scope l(log.general_logger(), info_level, "ConnectionManager::getDispatcher");
  l.vlog("getDispatcher do barramento %s", busid);
  if (!busid) return 0;
  AutoLock m(&_mutex);
  BusidConnection::const_iterator it = _busidConnection.find(std::string(busid));
  if (it != _busidConnection.end()) return it->second;
  else return 0;
}

Connection *ConnectionManager::clearDispatcher(const char *busid) {
  log_scope l(log.general_logger(), info_level, "ConnectionManager::clearDispatcher");
  l.vlog("clearDispatcher para a conexão [busid:%s]", busid);
  if (!busid) return 0;
  AutoLock m(&_mutex);
  BusidConnection::iterator it = _busidConnection.find(std::string(busid));
  if (it != _busidConnection.end()) {
    Connection *c = it->second;
    _busidConnection.erase(it);
    return c;
  } else return 0;    
}
}
