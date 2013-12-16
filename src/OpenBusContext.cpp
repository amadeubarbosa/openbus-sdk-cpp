// -*- coding: iso-8859-1-unix -*-
#include "openbus/OpenBusContext.hpp"
#include "openbus/log.hpp"

namespace openbus 
{
  OpenBusContext::OpenBusContext(CORBA::ORB_ptr orb, 
                                 boost::shared_ptr<interceptors::orb_info> i)
  : _orb(orb), _orb_info(i), _defaultConnection(0), _callDispatchCallback(0)
{
  log_scope l(log().general_logger(), debug_level, 
              "OpenBusContext::OpenBusContext");
  CORBA::Object_var init_ref = _orb->resolve_initial_references("PICurrent");
  _piCurrent = PortableInterceptor::Current::_narrow(init_ref);
  assert(!CORBA::is_nil(_piCurrent));
}

std::auto_ptr<Connection> OpenBusContext::createConnection(
  const std::string host, unsigned short port, 
  const Connection::ConnectionProperties &props)
{
  log_scope l(log().general_logger(), debug_level, 
              "OpenBusContext::createConnection");
  l.vlog("createConnection para host %s:%hi", host.c_str(), port);
  std::auto_ptr<Connection> conn (
    new Connection(host, port, _orb, _orb_info, *this, props));
  l.vlog("connection: %p", conn.get());
  return conn;
}

Connection *OpenBusContext::setDefaultConnection(Connection *c)
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> l(_mutex);
#endif
  Connection *old = _defaultConnection;
  _defaultConnection = c;
  return old;
}

Connection *OpenBusContext::getDefaultConnection() const 
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> l(_mutex);
#endif
  return _defaultConnection;
}

Connection *OpenBusContext::setCurrentConnection(Connection *c) 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::setCurrentConnection");
  l.vlog("connection:%p", c); 
  size_t const size = sizeof(Connection *);
  unsigned char buf[size];
  memcpy(buf, &c, size);
  idl::OctetSeq connectionAddrOctetSeq(size, size, buf);
  CORBA::Any connectionAddrAny;
  connectionAddrAny <<= connectionAddrOctetSeq;
  Connection *old = getCurrentConnection();
  _piCurrent->set_slot(_orb_info->slot.requester_conn, connectionAddrAny);
  return old;
}

Connection *OpenBusContext::getCurrentConnection() const 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::getCurrentConnection");
  CORBA::Any_var connectionAddrAny = 
    _piCurrent->get_slot(_orb_info->slot.requester_conn);
  idl::OctetSeq connectionAddrOctetSeq;
  Connection *c = 0;
  if (connectionAddrAny >>= connectionAddrOctetSeq)
  {
    assert(connectionAddrOctetSeq.length() == sizeof(Connection *));
    std::memcpy(&c, connectionAddrOctetSeq.get_buffer(), sizeof(Connection *));
  } 
  else 
  {
    return getDefaultConnection();
  }
  return c;
}

CallerChain OpenBusContext::getCallerChain() 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::getCallerChain");
  if (Connection *c = getDispatchConnection())
  {
    CORBA::Any_var sigCallChainAny = 
      _piCurrent->get_slot(_orb_info->slot.signed_call_chain);
    idl_ac::CallChain callChain;
    idl_cr::SignedCallChain sigCallChain;
    if (sigCallChainAny >>= sigCallChain) 
    {
      CORBA::Any_var callChainAny = _orb_info->codec->decode_value(sigCallChain.encoded,
                                                         idl_ac::_tc_CallChain);
      callChainAny >>= callChain;
      return CallerChain(
        c->busid(), c->login()->entity.in(), callChain.originators, 
        callChain.caller, sigCallChain);
    } 
  }
  return CallerChain();
}

void OpenBusContext::joinChain(CallerChain const &chain) 
{
  log_scope l(log().general_logger(), info_level, "OpenBusContext::joinChain");
  CORBA::Any sigCallChainAny;
  sigCallChainAny <<= *(chain.signedCallChain());
  _piCurrent->set_slot(_orb_info->slot.joined_call_chain, sigCallChainAny);
}

void OpenBusContext::exitChain() 
{
  log_scope l(log().general_logger(), info_level, "OpenBusContext::exitChain");
  CORBA::Any any;
  _piCurrent->set_slot(_orb_info->slot.joined_call_chain, any);    
}

CallerChain OpenBusContext::getJoinedChain() 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::getJoinedChain");
  if (Connection *c = getDispatchConnection())
  {
    CORBA::Any_var sigCallChainAny = 
      _piCurrent->get_slot(_orb_info->slot.joined_call_chain);
    idl_cr::SignedCallChain sigCallChain;
    if (sigCallChainAny >>= sigCallChain) 
    {
      CORBA::Any_var callChainAny = _orb_info->codec->decode_value(sigCallChain.encoded, 
                                                         idl_ac::_tc_CallChain);
      idl_ac::CallChain callChain;
      if (callChainAny >>= callChain) 
      {
        return CallerChain(
          c->busid(), c->login()->entity.in(), callChain.originators, 
          callChain.caller, sigCallChain);
      }
    }
  }
  return CallerChain();
}

void OpenBusContext::onCallDispatch(CallDispatchCallback c) 
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> l(_mutex);
#endif
  _callDispatchCallback = c;
}

OpenBusContext::CallDispatchCallback OpenBusContext::onCallDispatch() const 
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> l(_mutex);
#endif
  return _callDispatchCallback;
}

idl_or::OfferRegistry_ptr OpenBusContext::getOfferRegistry() const
{
  idl_or::OfferRegistry_var ret;
  Connection *conn = getCurrentConnection();
  if (conn)
  {
    ret = conn->getOfferRegistry();
  }
  return ret._retn();
}

idl_ac::LoginRegistry_ptr OpenBusContext::getLoginRegistry() const
{
  idl_ac::LoginRegistry_var ret;
  Connection *conn = getCurrentConnection();
  if(conn)
  {
    ret = conn->getLoginRegistry();
  }
  return ret._retn();
}

Connection *OpenBusContext::getDispatchConnection()
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::getDispatchConnection");
  CORBA::Any_var connectionAddrAny = 
    _piCurrent->get_slot(_orb_info->slot.receive_conn);
  idl::OctetSeq connectionAddrOctetSeq;
  Connection *c = 0;
  if (connectionAddrAny >>= connectionAddrOctetSeq)
  {
    assert(connectionAddrOctetSeq.length() == sizeof(Connection*));
    std::memcpy(&c, connectionAddrOctetSeq.get_buffer(), sizeof(Connection*));
    l.vlog("Connection.busid: %s", c->busid().c_str());
  } 
  return c;
}
}
