// -*- coding: iso-8859-1-unix -*-
#include "openbus/OpenBusContext.hpp"
#include "openbus/log.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

namespace openbus 
{  
const std::size_t magic_tag_size(4);
  
InvalidEncodedStream::InvalidEncodedStream()
{
}

InvalidEncodedStream::InvalidEncodedStream(const std::string &msg)
  : msg_(msg)
{
}

InvalidEncodedStream::~InvalidEncodedStream() throw()
{
}

bool CallerChain::is_legacy() const
{
  idl::EncryptedBlock null_block;
  memset(null_block, 0, idl::EncryptedBlockSize);
  return (std::memcmp(&null_block, _signedCallChain.signature,
                      idl::EncryptedBlockSize) == 0);
}
  
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
      CORBA::Any_var any(
        _orb_info->codec->decode_value(
          sig->encoded, idl_ac::_tc_CallChain));
      const idl_ac::CallChain *chain;
      *any >>= chain;
      return CallerChain(
        conn->busid(), conn->login()->entity.in(), chain->originators, 
        chain->caller, *sig);
    } 
  }
  return CallerChain();
}

void OpenBusContext::joinChain(CallerChain const &chain) 
{
  log_scope l(log().general_logger(), info_level, "OpenBusContext::joinChain");
  CORBA::Any sig_any;
  sig_any <<= *(chain.signedCallChain());
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
      CORBA::Any_var any(
        _orb_info->codec->decode_value(sig->encoded, idl_ac::_tc_CallChain));
      const idl_ac::CallChain *chain;
      if (*any >>= chain) 
      {
        return CallerChain(
          conn->busid(), conn->login()->entity.in(), chain->originators, 
          chain->caller, *sig);
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
    CORBA::Any_var any(_orb_info->codec->decode_value(sig->encoded,
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
  
  idl_data_export::ExportedVersionSeq exported_version_seq;
  exported_version_seq.length(1);
  
  idl_data_export::LegacyExportedCallChain exported_legacy_chain;
  exported_legacy_chain.bus = chain.busid().c_str();
  exported_legacy_chain.target = chain.target().c_str();
  exported_legacy_chain.caller = chain.caller();
  if (chain.originators().length() > 0)
  {
    exported_legacy_chain.delegate =
      chain.originators()[static_cast<CORBA::ULong>(0)].entity;
  }      

  CORBA::Any any;
  any <<= exported_legacy_chain;
  CORBA::OctetSeq_var exported_legacy_chain_cdr(
    _orb_info->codec->encode_value(any));

  idl_data_export::ExportedVersion legacy_exported_version;
  legacy_exported_version.version = idl_data_export::LegacyVersion;
  legacy_exported_version.encoded = *(exported_legacy_chain_cdr);

  if (chain.is_legacy())
  {
    exported_version_seq[static_cast<CORBA::ULong>(0)]
      = legacy_exported_version;
    return encode_exported_versions(exported_version_seq,
                                    idl_data_export::MagicTag_CallChain);
  }

  {
    exported_version_seq.length(2);

    idl_data_export::ExportedCallChain exported_chain;
    exported_chain.bus = chain.busid().c_str();
    exported_chain.signedChain = *(chain.signedCallChain());

    CORBA::Any any;
    any <<= exported_chain;
    CORBA::OctetSeq_var exported_chain_cdr(_orb_info->codec->encode_value(any));
 
    idl_data_export::ExportedVersion exported_version;
    exported_version.version = idl_data_export::CurrentVersion;
    exported_version.encoded = *(exported_chain_cdr);
    
    exported_version_seq[static_cast<CORBA::ULong>(0)] = exported_version;
    exported_version_seq[static_cast<CORBA::ULong>(1)]
      = legacy_exported_version;
  }
  return encode_exported_versions(exported_version_seq,
                                  idl_data_export::MagicTag_CallChain);
}


  CallerChain OpenBusContext::decodeChain(const CORBA::OctetSeq &encoded) const
{
  log_scope l(log().general_logger(), info_level,
              "OpenBusContext::decodeChain");
  try
  {
    idl_data_export::ExportedVersionSeq_var seq;
    std::string tag(decode_exported_versions(encoded, seq));

    if (idl_data_export::MagicTag_CallChain != tag)
    {
      throw InvalidEncodedStream(
        "Stream de bytes não corresponde ao tipo de dado esperado.");
    }

    for (CORBA::ULong i(0); i < seq->length(); ++i)
    {
      if (idl_data_export::CurrentVersion == seq[i].version)
      {
        const idl_data_export::ExportedCallChain *exported_chain;
        CORBA::Any_var exported_chain_any(
          _orb_info->codec->decode_value(
            seq[i].encoded, idl_data_export::_tc_ExportedCallChain));
        *exported_chain_any >>= exported_chain;
        const idl_ac::CallChain *call_chain;
        CORBA::Any_var call_chain_any = _orb_info->codec->decode_value(
          exported_chain->signedChain.encoded, idl_ac::_tc_CallChain);
        *call_chain_any >>= call_chain;
        return CallerChain(
          exported_chain->bus.in(), call_chain->target.in(),
          call_chain->originators, call_chain->caller,
          exported_chain->signedChain);      
      }
      if (idl_data_export::LegacyVersion == seq[i].version)
      {
        const idl_data_export::LegacyExportedCallChain *exported_chain;
        CORBA::Any_var any(
          _orb_info->codec->decode_value(
            seq[i].encoded, idl_data_export::_tc_LegacyExportedCallChain));
        *any >>= exported_chain;
        idl_ac::LoginInfoSeq originators;
        if (!std::string(exported_chain->delegate).empty())
        {
          originators.length(1);
          idl_ac::LoginInfo delegate;        
          delegate.id = "<unknown>";
          delegate.entity = exported_chain->delegate; 
          originators[static_cast<CORBA::ULong>(0)] = delegate;
        }
        idl_cr::SignedCallChain legacy_signed_chain;
        std::memset(legacy_signed_chain.signature, 0, idl::EncryptedBlockSize);
        idl_ac::CallChain legacy_call_chain;
        legacy_call_chain.target = exported_chain->target;
        legacy_call_chain.originators = originators;
        legacy_call_chain.caller = exported_chain->caller;
        CORBA::Any legacy_call_chain_any;
        legacy_call_chain_any <<= legacy_call_chain;
        CORBA::OctetSeq_var legacy_call_chain_cdr(
          _orb_info->codec->encode_value(legacy_call_chain_any));
        legacy_signed_chain.encoded = *(legacy_call_chain_cdr);

        return CallerChain(
          exported_chain->bus.in(), exported_chain->target.in(), originators,
          exported_chain->caller, legacy_signed_chain);
      }
    }
    throw InvalidEncodedStream("Versão de cadeia incompatível.");
  }
  catch (const CORBA::SystemException &e)
  {
    // Mico doest not implement CORBA::Exception::_rep_id().
    throw InvalidEncodedStream(
      boost::str(boost::format("Falha inesperada ao decodificar a cadeia. \
                               Exceção recebida '%1%' com minor code '%2'")
                               % e._repoid() % e.minor()));
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

CORBA::OctetSeq OpenBusContext::encode_exported_versions(
  idl_data_export::ExportedVersionSeq exported_version_seq,
  const std::string &tag)
{
  CORBA::Any any;
  any <<= exported_version_seq;  
  CORBA::OctetSeq_var exported_version_seq_cdr(
    _orb_info->codec->encode_value(any));
  
  CORBA::OctetSeq ret;
  std::size_t size(exported_version_seq_cdr->length() + tag.size());
  ret.length(size);

  std::memcpy(ret.get_buffer(), tag.c_str(), tag.size());
  std::memcpy(ret.get_buffer() + tag.size(),
              exported_version_seq_cdr->get_buffer(),
              size);
  return ret;
}

std::string OpenBusContext::decode_exported_versions(
  const CORBA::OctetSeq &stream,
  idl_data_export::ExportedVersionSeq_out exported_version_seq) const
{
  if (stream.length() < magic_tag_size )
  {
    throw IOP::Codec::FormatMismatch();
  }
  
  char tag[magic_tag_size + 1];
  std::memcpy(tag, stream.get_buffer(), magic_tag_size);
  tag[magic_tag_size] = '\0';
  
  CORBA::OctetSeq seq;
  seq.length(stream.length() - magic_tag_size);
  std::memcpy(seq.get_buffer(), stream.get_buffer() + magic_tag_size,
              seq.length());
  try
  {
    CORBA::Any_var any(_orb_info->codec->decode_value(
                         seq, idl_data_export::_tc_ExportedVersionSeq));
    const idl_data_export::ExportedVersionSeq *tmp;
    any >>= tmp;
    idl_data_export::ExportedVersionSeq_var ret(
      new idl_data_export::ExportedVersionSeq(*tmp));
    exported_version_seq = ret._retn();
  }
  catch (const CORBA::SystemException &e)
  {
    // Mico does not implement CORBA::Exception::_rep_id().
    throw InvalidEncodedStream(
      boost::str(
        boost::format("Falha ao extrair ExportedVersionSeq. Exceção lançada: \
        '%1%' com minor code '%2%'.") % e._repoid() % e.minor()));
  }
  return tag;
}
}
