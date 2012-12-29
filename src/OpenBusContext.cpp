// -*- coding: iso-8859-1 -*-
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/log.hpp"
#include "openbus/util/AutoLock_impl.hpp"

namespace openbus 
{
OpenBusContext::OpenBusContext(CORBA::ORB *o, IOP::Codec *c, PortableInterceptor::SlotId s1, 
                               PortableInterceptor::SlotId s2, PortableInterceptor::SlotId s3,
                               PortableInterceptor::SlotId s4, PortableInterceptor::SlotId s5) 
  : _orb(o), _codec(c), _slotId_joinedCallChain(s1), _slotId_signedCallChain(s2), 
    _slotId_legacyCallChain(s3), _slotId_requesterConnection(s4), _slotId_receiveConnection(s5),
    _defaultConnection(0), _callDispatchCallback(0)
{
  log_scope l(log.general_logger(), debug_level, "OpenBusContext::OpenBusContext");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent));
}

OpenBusContext::~OpenBusContext() 
{ 
}

std::auto_ptr<Connection> OpenBusContext::createConnection(const std::string host, short port, 
                                                           std::vector<std::string> props)
{
  log_scope l(log.general_logger(), debug_level, "OpenBusContext::createConnection");
  l.vlog("createConnection para host %s:%hi", host.c_str(), port);
  std::auto_ptr<Connection> conn(new Connection(host, port, _orb, _codec, _slotId_joinedCallChain, 
                                                _slotId_signedCallChain, _slotId_legacyCallChain, 
                                                _slotId_receiveConnection, this, props));
  l.vlog("connection: %p", conn.get());
  return conn;
}

Connection *OpenBusContext::setDefaultConnection(Connection *c)
{
  AutoLock _(&_mutex);
  Connection *old = _defaultConnection;
  _defaultConnection = c;
  return old;
}

Connection * OpenBusContext::getDefaultConnection() const 
{
  AutoLock _(&_mutex);
  return _defaultConnection;
}

Connection *OpenBusContext::setCurrentConnection(Connection *c) 
{
  log_scope l(log.general_logger(), info_level, "OpenBusContext::setCurrentConnection");
  l.vlog("connection:%p", c); 
  size_t size = sizeof(Connection *);
  unsigned char buf[size];
  memcpy(buf, &c, size);
  idl::OctetSeq connectionAddrOctetSeq(size, size, buf);
  CORBA::Any connectionAddrAny;
  connectionAddrAny <<= connectionAddrOctetSeq;
  Connection *old = getCurrentConnection();
  _piCurrent->set_slot(_slotId_requesterConnection, connectionAddrAny);
  return old;
}

Connection *OpenBusContext::getCurrentConnection() const 
{
  log_scope l(log.general_logger(), info_level, "OpenBusContext::getCurrentConnection");
  CORBA::Any_var connectionAddrAny = _piCurrent->get_slot(_slotId_requesterConnection);
  idl::OctetSeq connectionAddrOctetSeq;
  Connection *c = 0;
  if (*connectionAddrAny >>= connectionAddrOctetSeq)
  {
    assert(connectionAddrOctetSeq.length() == sizeof(Connection *));
    std::memcpy(&c, connectionAddrOctetSeq.get_buffer(), sizeof(Connection *));
  } else 
  {
    return getDefaultConnection();
  }
  return c;
}

CallerChain OpenBusContext::getCallerChain() 
{
  log_scope l(log.general_logger(), info_level, "OpenBusContext::getCallerChain");
  Connection *c = getDispatchConnection();
  if (!c)
  {
    return CallerChain();
  }

  CORBA::Any *sigCallChainAny = _piCurrent->get_slot(_slotId_signedCallChain);
  idl_ac::CallChain callChain;
  idl_cr::SignedCallChain sigCallChain;
  if (*sigCallChainAny >>= sigCallChain) 
  {
    CORBA::Any_var callChainAny = _codec->decode_value(sigCallChain.encoded,
                                                       idl_ac::_tc_CallChain);
    *callChainAny >>= callChain;
    return CallerChain(c->busid(), *c->login(), callChain.originators, callChain.caller, 
                       sigCallChain);
  } 
  else 
  {
    CORBA::Any_var legacyChainAny = _piCurrent->get_slot(_slotId_legacyCallChain);
    if (legacyChainAny >>= callChain)
    {
      return CallerChain(c->busid(), *c->login(), callChain.originators, callChain.caller);
    }
    else 
    {
      return CallerChain();
    }
  }
  return CallerChain();
}

void OpenBusContext::joinChain(CallerChain const &chain) 
{
  log_scope l(log.general_logger(), info_level, "OpenBusContext::joinChain");
  CORBA::Any sigCallChainAny;
  sigCallChainAny <<= *(chain.signedCallChain());
  _piCurrent->set_slot(_slotId_joinedCallChain, sigCallChainAny);
}

void OpenBusContext::exitChain() 
{
  log_scope l(log.general_logger(), info_level, "OpenBusContext::exitChain");
  CORBA::Any any;
  _piCurrent->set_slot(_slotId_joinedCallChain, any);    
}

CallerChain OpenBusContext::getJoinedChain() 
{
  log_scope l(log.general_logger(), info_level, "OpenBusContext::getJoinedChain");
  Connection *c = getDispatchConnection();
  if (!c)
  {
    return CallerChain();
  }
  CORBA::Any_var sigCallChainAny=_piCurrent->get_slot(_slotId_joinedCallChain);
  idl_cr::SignedCallChain sigCallChain;
  if (*sigCallChainAny >>= sigCallChain) 
  {
    CORBA::Any_var callChainAny = _codec->decode_value(sigCallChain.encoded, idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain) 
    {
      return CallerChain(c->busid(), *c->login(), callChain.originators, callChain.caller, 
                         sigCallChain);
    } 
    else
    { 
      return CallerChain();
    }
  } 
  else 
  {
    return CallerChain();
  }
}

void OpenBusContext::onCallDispatch(CallDispatchCallback c) 
{
  AutoLock ctx_mutex(&_mutex);
  _callDispatchCallback = c;
}

OpenBusContext::CallDispatchCallback OpenBusContext::onCallDispatch() const 
{
  AutoLock ctx_mutex(&_mutex);
  return _callDispatchCallback;
}

idl_or::OfferRegistry_ptr OpenBusContext::getOfferRegistry() const
{
  Connection *c = getCurrentConnection();
  if (c) 
  {
    return c->getOfferRegistry();
  }
  return 0; 
}

idl_ac::LoginRegistry_ptr OpenBusContext::getLoginRegistry() const
{
  Connection *c = getCurrentConnection();
  if (c)
  {
    return c->getLoginRegistry();
  }
  return 0;
}

Connection *OpenBusContext::getDispatchConnection()
{
  log_scope l(log.general_logger(), info_level, "OpenBusContext::getDispatchConnection");
  CORBA::Any_var connectionAddrAny = _piCurrent->get_slot(_slotId_receiveConnection);
  idl::OctetSeq connectionAddrOctetSeq;
  Connection *c = 0;
  if (*connectionAddrAny >>= connectionAddrOctetSeq)
  {
    assert(connectionAddrOctetSeq.length() == sizeof(Connection*));
    std::memcpy(&c, connectionAddrOctetSeq.get_buffer(), sizeof(Connection*));
    l.vlog("Connection.busid: %s", c->busid().c_str());
  } 
  return c;
}
}
