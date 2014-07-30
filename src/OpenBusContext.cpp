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

CallerChain OpenBusContext::makeChainFor(const char *loginId)
{
  log_scope l(log().general_logger(), info_level, 
                                                "OpenBusContext::makeChainFor");
  Connection *conn = getCurrentConnection();
  if (conn) {
    std::string busid = conn->busid();
    idl_cr::SignedCallChain_var sigCallChain;
    try
    {
      sigCallChain = conn->access_control()->signChainFor(loginId);
    }
    catch (const CORBA::SystemException &)
    {
      l.vlog("throw CORBA::NO_PERMISSION, minor=UnavailableBusCode, busid=%s", 
                                                         conn->busid().c_str());
      throw CORBA::NO_PERMISSION(idl_ac::UnavailableBusCode,
                                                           CORBA::COMPLETED_NO);
    }
    catch (const idl_ac::InvalidLogins &)
    {
      l.log("throw CORBA::NO_PERMISSION, minor=InvalidTargetCode");
      throw CORBA::NO_PERMISSION(idl_ac::InvalidTargetCode,
                                                           CORBA::COMPLETED_NO);
    }
    CORBA::Any_var callChainAny = _orb_info->codec->decode_value(
                              sigCallChain.in().encoded, idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain)
    {
      return CallerChain(
        busid, callChain.target.in(), callChain.originators, callChain.caller,
                                                                  sigCallChain);
    }
  }
  return CallerChain();
}

CORBA::OctetSeq OpenBusContext::encodeChain(const CallerChain chain)
{
  log_scope l(log().general_logger(), info_level,
              "OpenBusContext::encodeChain");
  CORBA::Any ctxIdAny;
  ctxIdAny <<= idl_cr::CredentialContextId;
  CORBA::OctetSeq_var ctxIdEnc = _orb_info->codec->encode_value(ctxIdAny);
  CORBA::Any chainAny;
  chainAny <<= (idl_cr::ExportedCallChain) { chain.busid().c_str(),
                                                   *(chain.signedCallChain()) };
  CORBA::OctetSeq_var chainEnc = _orb_info->codec->encode_value(chainAny);
  CORBA::OctetSeq encoded;
  encoded.length(ctxIdEnc.in().length() + chainEnc.in().length());
  CORBA::ULong i = 0;
  for (; i < ctxIdEnc.in().length(); i++)
  {
    encoded[i] = ctxIdEnc[i];
  }
  for (; i < encoded.length(); i++)
  {
    encoded[i] = chainEnc[i - ctxIdEnc.in().length()];
  }
  return encoded;
}

CallerChain OpenBusContext::decodeChain(const CORBA::OctetSeq encoded)
{
  log_scope l(log().general_logger(), info_level,
              "OpenBusContext::decodeChain");
  size_t const ctxIdSize = 8; // Endianness (1 byte) + padding (3) + ULong (4).
  if (encoded.length() >= ctxIdSize) {
    CORBA::OctetSeq ctxIdEnc;
    ctxIdEnc.length(ctxIdSize);
    for (CORBA::ULong i = 0; i < ctxIdEnc.length() ; i++)
    {
      ctxIdEnc[i] = encoded[i];
    }
    CORBA::Any_var ctxIdAny = _orb_info->codec->decode_value(ctxIdEnc,
                                                              CORBA::_tc_ulong);
    CORBA::ULong ctxId;
    if ((ctxIdAny >>= ctxId) && ctxId == idl_cr::CredentialContextId)
    {
      CORBA::OctetSeq chainEnc;
      chainEnc.length(encoded.length() - ctxIdEnc.length());
      for (CORBA::ULong i = ctxIdEnc.length(); i < encoded.length(); i++)
      {
        chainEnc[i - ctxIdSize] = encoded[i];
      }
      CORBA::Any_var impChainAny = _orb_info->codec->decode_value(chainEnc,
                                                 idl_cr::_tc_ExportedCallChain);
      idl_cr::ExportedCallChain impChain;
      if (impChainAny >>= impChain)
      {
        CORBA::Any_var callChainAny = _orb_info->codec->decode_value(
                           impChain.signedChain.encoded, idl_ac::_tc_CallChain);
        idl_ac::CallChain callChain;
        callChainAny >>= callChain;
        return CallerChain(impChain.bus.in(), callChain.target.in(),
                 callChain.originators, callChain.caller, impChain.signedChain);
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
