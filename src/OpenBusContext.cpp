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
  _piCurrent->set_slot(_orb_info->slot.requester_conn, any);
  return old;
}

Connection *OpenBusContext::getCurrentConnection() const 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::getCurrentConnection");
  CORBA::Any_var any(
    _piCurrent->get_slot(_orb_info->slot.requester_conn));
  const idl::OctetSeq *seq;
  Connection *conn(0);
  if (*any >>= seq)
  {
    assert(seq->length() == sizeof(Connection *));
    std::memcpy(&conn, seq->get_buffer(), sizeof(Connection *));
  } 
  else 
  {
    return getDefaultConnection();
  }
  return conn;
}

CallerChain OpenBusContext::getCallerChain() 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::getCallerChain");
  if (Connection *conn = getDispatchConnection())
  {
    CORBA::Any_var sig_any(
      _piCurrent->get_slot(_orb_info->slot.signed_call_chain));    
    const idl_cr::SignedCallChain *sig;
    if (*sig_any >>= sig) 
    {
      try
      {
        CORBA::Any_var any(
          _orb_info->codec->decode_value(
            CORBA::OctetSeq(sig->encoded.maximum(),
                            sig->encoded.length(),
                            const_cast<unsigned char *>
                            (sig->encoded.get_buffer())),
            idl_ac::_tc_CallChain));
        const idl_ac::CallChain *chain;
        *any >>= chain;
        return CallerChain(
          conn->busid(), conn->login()->entity.in(), chain->originators, 
          chain->caller, *sig);
      }
      catch (const IOP::Codec::FormatMismatch &)
      {
        /* empty */
      }
    } 
  }
  return CallerChain();
}

void OpenBusContext::joinChain(CallerChain const &chain) 
{
  log_scope l(log().general_logger(), info_level, "OpenBusContext::joinChain");
  CORBA::Any sig_any;
  sig_any <<= chain.signedCallChain();
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
  if (Connection *conn = getDispatchConnection())
  {
    CORBA::Any_var sig_any(
      _piCurrent->get_slot(_orb_info->slot.joined_call_chain));
    const idl_cr::SignedCallChain *sig;
    if (*sig_any >>= sig) 
    {
      try
      {
        CORBA::Any_var any(
          _orb_info->codec->decode_value(
            CORBA::OctetSeq(sig->encoded.maximum(),
                            sig->encoded.length(),
                            const_cast<unsigned char *>
                            (sig->encoded.get_buffer())),
            idl_ac::_tc_CallChain));
        const idl_ac::CallChain *chain;
        if (*any >>= chain) 
        {
          return CallerChain(
            conn->busid(), conn->login()->entity.in(), chain->originators, 
            chain->caller, *sig);
        }
      }
      catch (const IOP::Codec::FormatMismatch &)
      {
        /* empty */
      }
    }
  }
  return CallerChain();
}

CallerChain OpenBusContext::makeChainFor(const std::string loginId)
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::makeChainFor");
  Connection *conn(getCurrentConnection());
  if (conn)
  {
    idl_cr::SignedCallChain_var sig;
    try
    {
      sig = conn->access_control()->signChainFor(loginId.c_str());
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
    CORBA::Any_var any(_orb_info->codec->decode_value(
                         CORBA::OctetSeq(sig->encoded.maximum(),
                                         sig->encoded.length(),
                                         sig->encoded.get_buffer()),
                         idl_ac::_tc_CallChain));
    const idl_ac::CallChain *chain;
    if (*any >>= chain)
    {
      return CallerChain(conn->busid(), chain->target.in(),
                         chain->originators, chain->caller, sig);
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
  CORBA::OctetSeq_var ctxIdEnc(_orb_info->codec->encode_value(ctxIdAny));
  CORBA::Any chainAny;

  //MSVC 2008
  idl_cr::ExportedCallChain exported_chain;
  exported_chain.bus = chain.busid().c_str();
  exported_chain.signedChain = chain.signedCallChain();
  chainAny <<= exported_chain;

  CORBA::OctetSeq_var chainEnc(_orb_info->codec->encode_value(chainAny));
  CORBA::OctetSeq encoded;
  encoded.length(ctxIdEnc->length() + chainEnc->length());
  CORBA::ULong i(0);
  for (; i < ctxIdEnc->length(); ++i)
  {
    encoded[i] = ctxIdEnc[i];
  }
  for (; i < encoded.length(); ++i)
  {
    encoded[i] = chainEnc[i - ctxIdEnc->length()];
  }
  return encoded;
}

CallerChain OpenBusContext::decodeChain(const CORBA::OctetSeq encoded)
{
  log_scope l(log().general_logger(), info_level,
              "OpenBusContext::decodeChain");
  // Endianness (1 byte) + padding (3) + ULong (4).
  CORBA::ULong const ctxIdSize(8);
  if (encoded.length() >= ctxIdSize)
  {
    CORBA::OctetSeq ctxIdEnc;
    ctxIdEnc.length(ctxIdSize);
    for (CORBA::ULong i = 0; i < ctxIdEnc.length() ; ++i)
    {
      ctxIdEnc[i] = encoded[i];
    }
    CORBA::Any_var ctxIdAny(_orb_info->codec->decode_value(ctxIdEnc,
                                                           CORBA::_tc_ulong));
    CORBA::ULong ctxId;
    if ((ctxIdAny >>= ctxId) && ctxId == idl_cr::CredentialContextId)
    {
      CORBA::OctetSeq chainEnc;
      chainEnc.length(encoded.length() - ctxIdEnc.length());
      for (CORBA::ULong i = ctxIdEnc.length(); i < encoded.length(); ++i)
      {
        chainEnc[i - ctxIdSize] = encoded[i];
      }
      CORBA::Any_var impChainAny(
        _orb_info->codec->decode_value(chainEnc,
                                       idl_cr::_tc_ExportedCallChain));
      const idl_cr::ExportedCallChain *impChain;
      if (*impChainAny >>= impChain)
      {
        CORBA::Any_var callChainAny(
          _orb_info->codec->decode_value(
            CORBA::OctetSeq(impChain->signedChain.encoded.maximum(),
                            impChain->signedChain.encoded.length(),
                            const_cast<unsigned char *>
                            (impChain->signedChain.encoded.get_buffer())),
            idl_ac::_tc_CallChain));
        const idl_ac::CallChain *callChain;
        *callChainAny >>= callChain;
        return CallerChain(impChain->bus.in(), callChain->target.in(),
                           callChain->originators, callChain->caller,
                           impChain->signedChain);
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

Connection *OpenBusContext::getDispatchConnection()
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::getDispatchConnection");
  CORBA::Any_var any(
    _piCurrent->get_slot(_orb_info->slot.receive_conn));
  const idl::OctetSeq *seq;
  Connection *conn(0);
  if (*any >>= seq)
  {
    assert(seq->length() == sizeof(Connection*));
    std::memcpy(&conn, seq->get_buffer(), sizeof(Connection*));
    l.vlog("Connection.busid: %s", conn->busid().c_str());
  } 
  return conn;
}
}
