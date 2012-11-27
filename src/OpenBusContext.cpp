#include "openbus/OpenBusContext.h"
#include "openbus/log.h"
#include "openbus/util/AutoLock_impl.h"

namespace openbus {
OpenBusContext::OpenBusContext(CORBA::ORB *o, IOP::Codec *c, 
                                     PortableInterceptor::SlotId s1, 
                                     PortableInterceptor::SlotId s2, 
                                     PortableInterceptor::SlotId s3,
                                     PortableInterceptor::SlotId s4,
                                     PortableInterceptor::SlotId s5) 
  : _orb(o), _codec(c), _slotId_joinedCallChain(s1), _slotId_signedCallChain(s2), 
    _slotId_legacyCallChain(s3), _slotId_requesterConnection(s4), _slotId_receiveConnection(s5),
  _defaultConnection(0), _callDispatchCallback(0)
{
  log_scope l(log.general_logger(), debug_level, "OpenBusContext::OpenBusContext");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent));
}

OpenBusContext::~OpenBusContext() { }

std::auto_ptr<Connection> OpenBusContext::createConnection(const char *host, short port, 
                                                              std::vector<std::string> props)
{
  log_scope l(log.general_logger(), debug_level, "OpenBusContext::createConnection");
  l.vlog("createConnection para host %s:%hi", host, port);
  std::auto_ptr<Connection> conn(new Connection(host, port, _orb, _codec, _slotId_joinedCallChain, 
    _slotId_signedCallChain, _slotId_legacyCallChain, _slotId_receiveConnection, this, props));
  l.vlog("connection: %p", conn.get());
  return conn;
}

Connection * OpenBusContext::getDefaultConnection() const 
{
  AutoLock _(&_mutex);
  return _defaultConnection;
}

void OpenBusContext::setRequester(Connection *c) {
  log_scope l(log.general_logger(), info_level, "OpenBusContext::setRequester");
  l.vlog("connection:%p", c);
  size_t size = sizeof(Connection *);
  unsigned char buf[size];
  memcpy(buf, &c, size);
  idl::OctetSeq connectionAddrOctetSeq(size, size, buf);
  CORBA::Any connectionAddrAny;
  connectionAddrAny <<= connectionAddrOctetSeq;
  _piCurrent->set_slot(_slotId_requesterConnection, connectionAddrAny);
}

Connection * OpenBusContext::getRequester() const { 
  log_scope l(log.general_logger(), info_level, "OpenBusContext::getRequester");
  CORBA::Any_var connectionAddrAny=_piCurrent->get_slot(_slotId_requesterConnection);
  idl::OctetSeq connectionAddrOctetSeq;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) {
    assert(connectionAddrOctetSeq.length() == sizeof(Connection *));
    Connection *c;
    std::memcpy(&c, connectionAddrOctetSeq.get_buffer(), sizeof(Connection *));
    return c;
  } else return 0;
}

void OpenBusContext::setDispatcher(Connection &c) {
  log_scope l(log.general_logger(), info_level, "OpenBusContext::setDispatcher");
  AutoLock m(&_mutex);
  if (c._busid) _busidConnection[std::string(c._busid)] = &c;
}

Connection * OpenBusContext::getDispatcher(const char *busid) {
  log_scope l(log.general_logger(), info_level, "OpenBusContext::getDispatcher");
  l.vlog("getDispatcher do barramento %s", busid);
  if (!busid) return 0;
  AutoLock m(&_mutex);
  BusidConnection::const_iterator it = _busidConnection.find(std::string(busid));
  if (it != _busidConnection.end()) return it->second;
  else return 0;
}

Connection * OpenBusContext::clearDispatcher(const char *busid) {
  log_scope l(log.general_logger(), info_level, "OpenBusContext::clearDispatcher");
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

void OpenBusContext::onCallDispatch(CallDispatchCallback c) {
  AutoLock ctx_mutex(&_mutex);
  _callDispatchCallback = c;
}

OpenBusContext::CallDispatchCallback OpenBusContext::onCallDispatch() const {
  AutoLock ctx_mutex(&_mutex);
  return _callDispatchCallback;
}
}
