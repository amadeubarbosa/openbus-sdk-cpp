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

CallerChain OpenBusContext::getCallerChain() {
  log_scope l(log.general_logger(), info_level, "OpenBusContext::getCallerChain");
  CORBA::Any_var connectionAddrAny = _piCurrent->get_slot(_slotId_receiveConnection);
  idl::OctetSeq connectionAddrOctetSeq;
  Connection *c = 0;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) {
    assert(connectionAddrOctetSeq.length() == sizeof(Connection*));
    std::memcpy(&c, connectionAddrOctetSeq.get_buffer(), sizeof(Connection*));
    l.vlog("Connection.busid: %s", c->busid());
    // if (c != this) return CallerChain();
  } else return CallerChain();

  CORBA::Any *sigCallChainAny = _piCurrent->get_slot(_slotId_signedCallChain);
  idl_ac::CallChain callChain;
  idl_cr::SignedCallChain sigCallChain;
  if (*sigCallChainAny >>= sigCallChain) {
    CORBA::Any_var callChainAny = _codec->decode_value(sigCallChain.encoded,
                                                       idl_ac::_tc_CallChain);
    *callChainAny >>= callChain;
    return CallerChain(c->_busid, callChain.originators, callChain.caller, sigCallChain);
  } else {
    CORBA::Any_var legacyChainAny = _piCurrent->get_slot(_slotId_legacyCallChain);
    if (legacyChainAny >>= callChain)
      return CallerChain(0, callChain.originators, callChain.caller);
    else return CallerChain();
  }
  return CallerChain();
}

void OpenBusContext::joinChain(CallerChain const& chain) {
  log_scope l(log.general_logger(), info_level, "OpenBusContext::joinChain");
  CORBA::Any sigCallChainAny;
  sigCallChainAny <<= *(chain.signedCallChain());
  _piCurrent->set_slot(_slotId_joinedCallChain, sigCallChainAny);
}

void OpenBusContext::exitChain() {
  log_scope l(log.general_logger(), info_level, "OpenBusContext::exitChain");
  CORBA::Any any;
  _piCurrent->set_slot(_slotId_joinedCallChain, any);    
}

CallerChain OpenBusContext::getJoinedChain() {
  log_scope l(log.general_logger(), info_level, "OpenBusContext::getJoinedChain");
  CORBA::Any_var sigCallChainAny=_piCurrent->get_slot(_slotId_joinedCallChain);
  idl_cr::SignedCallChain sigCallChain;
  if (*sigCallChainAny >>= sigCallChain) {
    CORBA::Any_var callChainAny = _codec->decode_value(sigCallChain.encoded, idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain) return CallerChain(callChain.target, callChain.originators, 
      callChain.caller, sigCallChain);
    else return CallerChain();
  } else return CallerChain();
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
