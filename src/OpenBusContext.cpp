// -*- coding: iso-8859-1-unix -*-
#include "openbus/OpenBusContext.hpp"
#include "openbus/log.hpp"
#include "openbus/any.hpp"

namespace openbus 
{
OpenBusContext::OpenBusContext(CORBA::ORB_ptr orb, 
                               boost::shared_ptr<interceptors::orb_info> i)
  : _orb(orb), _orb_info(i), _codec(_orb_info->codec), _defaultConnection(0),
    _callDispatchCallback(0)
{
  log_scope l(log().general_logger(), debug_level, 
              "OpenBusContext::OpenBusContext");
  CORBA::Object_var init_ref(_orb->resolve_initial_references("PICurrent"));
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
  std::auto_ptr<Connection> conn(
    new Connection(host, port, _orb, _orb_info, *this, props));
  l.vlog("connection: %p", conn.get());
  return conn;
}

Connection *OpenBusContext::setDefaultConnection(Connection *conn)
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> l(_mutex);
#endif
  Connection *old(_defaultConnection);
  _defaultConnection = conn;
  return old;
}

Connection *OpenBusContext::getDefaultConnection() const 
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> l(_mutex);
#endif
  return _defaultConnection;
}

Connection *OpenBusContext::setCurrentConnection(Connection *conn) 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::setCurrentConnection");
  l.vlog("connection:%p", conn); 
  unsigned char buf[sizeof(Connection *)];
  memcpy(buf, &conn, sizeof(Connection *));
  idl::OctetSeq seq(sizeof(Connection *), sizeof(Connection *), buf);
  CORBA::Any any;
  any <<= seq;
  Connection *old(getCurrentConnection());
  _piCurrent->set_slot(_orb_info->slot.current_connection, any);
  return old;
}

Connection *OpenBusContext::getCurrentConnection() const 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::getCurrentConnection");
  CORBA::Any_var any(
    _piCurrent->get_slot(_orb_info->slot.current_connection));
  
  Connection *conn(0);
  idl::OctetSeq seq(extract<idl::OctetSeq>(any));  
  if (seq.length() == 0)
  {
    return getDefaultConnection();
  }
  assert(seq.length() == sizeof(Connection *));
  std::memcpy(&conn, seq.get_buffer(), sizeof(Connection *));
  return conn;
}

CallerChain OpenBusContext::getCallerChain() 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::getCallerChain");
  Connection *conn(getCurrentConnection());
  if (!conn)
  {
    return CallerChain();
  }
    
  CORBA::Any_var any(
    _piCurrent->get_slot(_orb_info->slot.signed_call_chain));
    
  idl_cr::SignedCallChain signed_chain(extract<idl_cr::SignedCallChain>(any));
    
  any = _codec->decode_value(signed_chain.encoded, idl_ac::_tc_CallChain);
  idl_ac::CallChain chain(extract<idl_ac::CallChain>(any));
    
  return CallerChain(
    conn->busid(), conn->login()->entity.in(), chain.originators, 
    chain.caller, signed_chain);
}

void OpenBusContext::joinChain(CallerChain const &chain) 
{
  log_scope l(log().general_logger(), info_level, "OpenBusContext::joinChain");
  CallerChain caller_chain(chain);
  CORBA::Any sig_any;
  if (caller_chain != CallerChain())
  {
    sig_any <<= *(caller_chain.signedCallChain());
  }
  _piCurrent->set_slot(_orb_info->slot.joined_call_chain, sig_any);
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
  Connection *conn(getCurrentConnection());
  if (!conn)
  {
    return CallerChain();
  }
  CORBA::Any_var any(_piCurrent->get_slot(_orb_info->slot.joined_call_chain));

  idl_cr::SignedCallChain signed_chain(extract<idl_cr::SignedCallChain>(any));
  if (signed_chain.encoded.length() == 0)
  {
    return CallerChain();
  }
    
  any = _codec->decode_value(signed_chain.encoded, idl_ac::_tc_CallChain);
  idl_ac::CallChain chain(extract<idl_ac::CallChain>(any));

  return CallerChain(
    conn->busid(), conn->login()->entity.in(), chain.originators, 
    chain.caller, signed_chain);
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
  Connection *conn(getCurrentConnection());
  if (conn)
  {
    ret = conn->getOfferRegistry();
  }
  return ret._retn();
}

idl_ac::LoginRegistry_ptr OpenBusContext::getLoginRegistry() const
{
  idl_ac::LoginRegistry_var ret;
  Connection *conn(getCurrentConnection());
  if(conn)
  {
    ret = conn->getLoginRegistry();
  }
  return ret._retn();
}
}
