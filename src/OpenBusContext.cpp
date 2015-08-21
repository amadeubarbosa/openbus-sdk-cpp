// -*- coding: iso-8859-1-unix -*-

#include "openbus/idl.hpp"
#include "openbus/openbus_context.hpp"
#include "openbus/detail/login_cache.hpp"
#include "openbus/log.hpp"
#include "openbus/detail/any.hpp"
#include "openbus/detail/openssl/public_key.hpp"

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

OpenBusContext::OpenBusContext(CORBA::ORB_ptr orb, 
                               interceptors::ORBInitializer *orb_init)
  : _orb_init(orb_init), _orb(orb), _defaultConnection(0),
    _callDispatchCallback(0)
{
  log_scope l(log().general_logger(), debug_level, 
              "OpenBusContext::OpenBusContext");
}

std::auto_ptr<Connection> OpenBusContext::connect_by_address_impl(
  const std::string &host,
  unsigned short port,
  EVP_PKEY *access_key,
  bool legacy_support)

{
  log_scope l(log().general_logger(), debug_level, 
              "OpenBusContext::connectByAddress");
  l.vlog("connectByAddress para host %s:%hi", host.c_str(), port);
  std::auto_ptr<Connection> conn(
    new Connection(host, port, _orb, _orb_init, *this, access_key,
                   legacy_support));
  l.vlog("connection: %p", conn.get());
  return conn;
}


std::auto_ptr<Connection> OpenBusContext::connect_by_reference_impl(
  CORBA::Object_var ref,
  EVP_PKEY *access_key,
  bool legacy_support)
{
  log_scope l(log().general_logger(), debug_level, 
              "OpenBusContext::connectByReference");
  std::auto_ptr<Connection> conn(
    new Connection(ref, _orb, _orb_init, *this, access_key, legacy_support));
  l.vlog("connection: %p", conn.get());
  return conn;
} 

std::auto_ptr<Connection> OpenBusContext::create_connection_impl(
  const std::string &host,
  unsigned short port,
  EVP_PKEY *access_key,
  bool legacy_support)

{
  return connect_by_address_impl(host, port, access_key, legacy_support);
}

Connection *OpenBusContext::setDefaultConnection(Connection *conn)
{
  boost::lock_guard<boost::mutex> l(_mutex);
  Connection *old(_defaultConnection);
  _defaultConnection = conn;
  return old;
}

Connection *OpenBusContext::getDefaultConnection() const 
{
  boost::lock_guard<boost::mutex> l(_mutex);
  return _defaultConnection;
}

Connection *OpenBusContext::setCurrentConnection(Connection *conn) 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::setCurrentConnection");
  l.vlog("connection:%p", conn); 
  unsigned char buf[sizeof(Connection *)];
  memcpy(buf, &conn, sizeof(Connection *));
  idl::core::OctetSeq seq(sizeof(Connection *), sizeof(Connection *), buf);
  CORBA::Any any;
  any <<= seq;
  Connection *old(getCurrentConnection());
  _orb_init->pi_current->set_slot(_orb_init->current_connection, any);
  return old;
}

Connection *OpenBusContext::getCurrentConnection() const 
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::getCurrentConnection");
  CORBA::Any_var any(
    _orb_init->pi_current->get_slot(_orb_init->current_connection));
  
  Connection *conn(0);
  idl::core::OctetSeq seq(extract<idl::core::OctetSeq>(any));  
  if (seq.length() == 0)
  {
    return getDefaultConnection();
  }
  assert(seq.length() == sizeof(Connection *));
  std::memcpy(&conn, seq.get_buffer(), sizeof(Connection *));
  return conn;
}

CallerChain OpenBusContext::extract_call_chain(
  idl::creden::SignedData signed_chain,
  Connection *conn)
{
  CORBA::Any_var any(
    _orb_init->codec->decode_value(
      CORBA::OctetSeq(signed_chain.encoded.maximum(),
                      signed_chain.encoded.length(),
                      const_cast<unsigned char *>
                      (signed_chain.encoded.get_buffer())),
      idl::access::_tc_CallChain));
    idl::access::CallChain chain(extract<idl::access::CallChain>(any));
  return CallerChain(
    chain,
    chain.bus.in(),
    conn->login()->entity.in(),
    signed_chain);
}

CallerChain OpenBusContext::extract_legacy_call_chain(
  idl::legacy::creden::SignedCallChain signed_chain,
  Connection *conn)
{
  CORBA::Any_var any(
    _orb_init->codec->decode_value(
      CORBA::OctetSeq(signed_chain.encoded.maximum(),
                      signed_chain.encoded.length(),
                      const_cast<unsigned char *>
                      (signed_chain.encoded.get_buffer())),
      idl::legacy::access::_tc_CallChain));
    idl::legacy::access::CallChain chain(extract<idl::legacy::access::CallChain>(any));
  return CallerChain(
    chain,
    conn->busid(),
    conn->login()->entity.in(),
    signed_chain);
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
    _orb_init->pi_current->get_slot(_orb_init->signed_call_chain));
    
  idl::creden::SignedData signed_chain(extract<idl::creden::SignedData>(any));
  if (signed_chain.encoded.length() == 0)
  {    
    idl::legacy::creden::SignedCallChain legacy_signed_chain(
      extract<idl::legacy::creden::SignedCallChain>(any));
    if (legacy_signed_chain.encoded.length() != 0)
      return extract_legacy_call_chain(legacy_signed_chain, conn);
  }
  else
    return extract_call_chain(signed_chain, conn);

  return CallerChain();
}

void OpenBusContext::joinChain(CallerChain const &chain) 
{
  log_scope l(log().general_logger(), info_level, "OpenBusContext::joinChain");
  CallerChain caller_chain(chain == CallerChain() ? getCallerChain() : chain);
  if (caller_chain == CallerChain())
  {
    return;
  }
  CORBA::Any sig_any;
  if (caller_chain._signedCallChain.encoded.length() > 0)
    sig_any <<= caller_chain._signedCallChain;
  else
    sig_any <<= caller_chain._legacy_signedCallChain;    
  _orb_init->pi_current->set_slot(_orb_init->joined_call_chain, sig_any);
}

void OpenBusContext::exitChain() 
{
  log_scope l(log().general_logger(), info_level, "OpenBusContext::exitChain");
  CORBA::Any any;
  _orb_init->pi_current->set_slot(_orb_init->joined_call_chain, any);    
}

CallerChain OpenBusContext::getJoinedChain() const
{
  log_scope l(log().general_logger(), debug_level, 
              "OpenBusContext::getJoinedChain");
  Connection *conn(getCurrentConnection());
  if (!conn)
  {
    return CallerChain();
  }
  CORBA::Any_var any(_orb_init->pi_current->get_slot(_orb_init->joined_call_chain));

  idl::creden::SignedData signed_chain(extract<idl::creden::SignedData>(any));
  if (signed_chain.encoded.length() == 0)
  {
    return CallerChain();
  }
  try
  {
    any =
      _orb_init->codec->decode_value(
        CORBA::OctetSeq(signed_chain.encoded.maximum(),
                        signed_chain.encoded.length(),
                        const_cast<unsigned char *>
                        (signed_chain.encoded.get_buffer())),
        idl::access::_tc_CallChain);
  }
  catch (const CORBA::Exception &)
  {
    return CallerChain();
  }
  idl::access::CallChain chain(extract<idl::access::CallChain>(any));

  return CallerChain(
    chain, chain.bus.in(), conn->login()->entity.in(), 
    signed_chain);
}

CallerChain OpenBusContext::makeChainFor(const std::string &entity) const
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::makeChainFor");
  Connection *conn(getCurrentConnection());
  if (!conn)
  {
    return CallerChain();
  }
  CallerChain joined_chain(getJoinedChain());
  idl::creden::SignedData_var signed_chain(
    conn->access_control()->signChainFor(entity.c_str()));
  CORBA::Any_var any(_orb_init->codec->decode_value(
                       CORBA::OctetSeq(signed_chain->encoded.maximum(),
                                       signed_chain->encoded.length(),
                                       signed_chain->encoded.get_buffer()),
                       idl::access::_tc_CallChain));
  idl::access::CallChain chain(extract<idl::access::CallChain>(any));
  return CallerChain(chain, chain.bus.in(), chain.target.in(),
                     *signed_chain);
}

CallerChain OpenBusContext::importChain(
  const CORBA::OctetSeq &token,
  const std::string &domain) const
{
  log_scope l(log().general_logger(), info_level, 
              "OpenBusContext::importChain");
  Connection *conn(getCurrentConnection());
  if (!conn)
  {
    return CallerChain();
  }
  idl::core::OctetSeq_var o(conn->access_control()->certificate());
  openssl::pX509 crt(openssl::byteSeq2x509(o->get_buffer(), o->length()));
  openssl::pkey pub_key(X509_get_pubkey(crt.get()));
  PublicKey bus_key(pub_key);
  idl::core::OctetSeq encrypted(bus_key.encrypt(token.get_buffer(), token.length()));
  idl::creden::SignedData_var signed_chain(
    conn->access_control()->signChainByToken(encrypted, domain.c_str()));
  CORBA::Any_var any(_orb_init->codec->decode_value(
                       CORBA::OctetSeq(signed_chain->encoded.maximum(),
                                       signed_chain->encoded.length(),
                                       signed_chain->encoded.get_buffer()),
                       idl::access::_tc_CallChain));
  idl::access::CallChain chain(extract<idl::access::CallChain>(any));
  return CallerChain(chain, chain.bus.in(), chain.target.in(),
                     *signed_chain);
}

CORBA::OctetSeq OpenBusContext::encodeChain(const CallerChain chain)
{
  log_scope l(log().general_logger(), info_level,
              "OpenBusContext::encodeChain");
  
  idl::data_export::VersionedDataSeq exported_version_seq;
  exported_version_seq.length(1);
  
  {
    exported_version_seq.length(2);

    idl::data_export::ExportedCallChain exported_chain = chain._signedCallChain;

    CORBA::Any any;
    any <<= exported_chain;
    CORBA::OctetSeq_var exported_chain_cdr(_orb_init->codec->encode_value(any));
 
    idl::data_export::VersionedData exported_version;
    exported_version.version = idl::data_export::ExportVersion;
    exported_version.encoded = idl::core::OctetSeq(
      exported_chain_cdr->maximum(),
      exported_chain_cdr->length(),
      const_cast<unsigned char *>
      (exported_chain_cdr->get_buffer()));
    
    exported_version_seq[static_cast<CORBA::ULong>(0)] = exported_version;
  }
  return encode_exported_versions(exported_version_seq,
                                  idl::data_export::MagicTag_CallChain);
}


CallerChain OpenBusContext::decodeChain(const CORBA::OctetSeq &encoded) const
{
  log_scope l(log().general_logger(), debug_level,
              "OpenBusContext::decodeChain");
  try
  {
    idl::data_export::VersionedDataSeq_var seq;
    std::string tag(decode_exported_versions(encoded, seq));

    if (idl::data_export::MagicTag_CallChain != tag)
    {
      l.log("InvalidEncodedStream: Stream de bytes n�o corresponde ao tipo de dado esperado.");
      throw InvalidEncodedStream(
        "Stream de bytes n�o corresponde ao tipo de dado esperado.");
    }

    for (CORBA::ULong i(0); i < seq->length(); ++i)
    {
      if (idl::data_export::ExportVersion == seq[i].version)
      {
        l.log("Decodificando 'CurrentVersion'.");
        CORBA::Any_var exported_chain_any(
          _orb_init->codec->decode_value(
            CORBA::OctetSeq(seq[i].encoded.maximum(),
                            seq[i].encoded.length(),
                            const_cast<unsigned char*>
                            (seq[i].encoded.get_buffer())),
            idl::data_export::_tc_ExportedCallChain));
        idl::data_export::ExportedCallChain exported_chain(
          extract<idl::data_export::ExportedCallChain>(exported_chain_any));
        CORBA::Any_var call_chain_any(
          _orb_init->codec->decode_value(
            CORBA::OctetSeq(exported_chain.encoded.maximum(),
                            exported_chain.encoded.length(),
                            const_cast<unsigned char *>
                            (exported_chain.encoded.get_buffer())),
            idl::access::_tc_CallChain));
        idl::access::CallChain call_chain(
          extract<idl::access::CallChain>(call_chain_any));
        return CallerChain(
          call_chain, call_chain.bus.in(), call_chain.target.in(),
          exported_chain);      
      }
      else if (idl::legacy::data_export::CurrentVersion == seq[i].version)
      {
        l.log("Decodificando versao legada.");
        CORBA::Any_var exported_chain_any(
          _orb_init->codec->decode_value(
            CORBA::OctetSeq(seq[i].encoded.maximum(),
                            seq[i].encoded.length(),
                            const_cast<unsigned char*>
                            (seq[i].encoded.get_buffer())),
            idl::legacy::data_export::_tc_ExportedCallChain));
        idl::legacy::data_export::ExportedCallChain exported_chain(
          extract<idl::legacy::data_export::ExportedCallChain>(exported_chain_any));
        CORBA::Any_var call_chain_any(
          _orb_init->codec->decode_value(
            CORBA::OctetSeq(exported_chain.signedChain.encoded.maximum(),
                            exported_chain.signedChain.encoded.length(),
                            const_cast<unsigned char *>
                            (exported_chain.signedChain.encoded.get_buffer())),
            idl::legacy::access::_tc_CallChain));
        idl::legacy::access::CallChain call_chain(
          extract<idl::legacy::access::CallChain>(call_chain_any));
        return CallerChain(
          call_chain, exported_chain.bus.in(), call_chain.target.in(),
          exported_chain.signedChain);      
      }
        
    }
    throw InvalidEncodedStream("Vers�o de cadeia incompat�vel.");
  }
  catch (const CORBA::SystemException &e)
  {
    throw InvalidEncodedStream(
      boost::str(boost::format("Falha inesperada ao decodificar a cadeia. \
                               Exce��o recebida '%1%' com minor code '%2'")
                               % e._rep_id() % e.minor()));
  }
  return CallerChain();
}

CORBA::OctetSeq OpenBusContext::encodeSharedAuthSecret(
  const SharedAuthSecret &secret)
{
  log_scope l(log().general_logger(), info_level,
              "OpenBusContext::encodeSharedAuth");
  idl::data_export::VersionedDataSeq exported_version_seq;
  exported_version_seq.length(2);

  idl::data_export::ExportedSharedAuth shared_auth;
  shared_auth.bus = secret.busid().c_str();
  shared_auth.attempt = secret.login_process_;
  shared_auth.secret = secret.secret_;

  CORBA::Any any;
  any <<= shared_auth;
  CORBA::OctetSeq_var shared_auth_cdr(_orb_init->codec->encode_value(any));  
  
  idl::data_export::VersionedData exported_curr_version;
  exported_curr_version.version = idl::data_export::ExportVersion;
  exported_curr_version.encoded = idl::core::OctetSeq(
    shared_auth_cdr->maximum(),
    shared_auth_cdr->length(),
    const_cast<unsigned char *>
    (shared_auth_cdr->get_buffer()));
  
  exported_version_seq[0u] = exported_curr_version;

  {
    idl::legacy::data_export::ExportedSharedAuth shared_auth;
    shared_auth.bus = secret.busid().c_str();
    shared_auth.attempt = secret.legacy_login_process_;
    shared_auth.secret = idl::legacy::core::OctetSeq(
      secret.secret_.maximum(),
      secret.secret_.length(),
      const_cast<unsigned char *>
      (secret.secret_.get_buffer()));

    CORBA::Any any;
    any <<= shared_auth;
    CORBA::OctetSeq_var shared_auth_cdr(_orb_init->codec->encode_value(any));  
  
    idl::data_export::VersionedData exported_curr_version;
    exported_curr_version.version = idl::legacy::data_export::CurrentVersion;
    exported_curr_version.encoded = idl::core::OctetSeq(
      shared_auth_cdr->maximum(),
      shared_auth_cdr->length(),
      const_cast<unsigned char *>
      (shared_auth_cdr->get_buffer()));
  
    exported_version_seq[1u] = exported_curr_version;
  }
  
  return encode_exported_versions(exported_version_seq,
                                  idl::data_export::MagicTag_SharedAuth);
}
  
SharedAuthSecret OpenBusContext::decodeSharedAuthSecret(
  const CORBA::OctetSeq &encoded)
{
  log_scope l(log().general_logger(), info_level,
              "OpenBusContext::decodeSharedAuth");
  idl::data_export::VersionedDataSeq_var seq;
  std::string tag(decode_exported_versions(encoded, seq));

  if (idl::data_export::MagicTag_SharedAuth != tag)
  {
    throw InvalidEncodedStream(
      "Stream de bytes n�o corresponde ao tipo de dado esperado.");
  }
  
  SharedAuthSecret secret;
  bool found(false);
  for (CORBA::ULong i(0); i < seq->length(); ++i)
  {
    if (idl::data_export::ExportVersion == seq[i].version)
    {
      CORBA::Any_var any(_orb_init->codec->decode_value(
                           CORBA::OctetSeq(seq[i].encoded.maximum(),
                                           seq[i].encoded.length(),
                                           const_cast<unsigned char*>
                                           (seq[i].encoded.get_buffer())),
                           idl::data_export::_tc_ExportedSharedAuth));
      idl::data_export::ExportedSharedAuth exported_shared_auth(
        extract<idl::data_export::ExportedSharedAuth>(any));
      secret.busid_ = exported_shared_auth.bus.in();
      secret.login_process_ = exported_shared_auth.attempt;
      secret.secret_ = exported_shared_auth.secret;
      found = true;
    }
    else if (idl::legacy::data_export::CurrentVersion == seq[i].version)
    {
      CORBA::Any_var any(_orb_init->codec->decode_value(
                           CORBA::OctetSeq(seq[i].encoded.maximum(),
                                           seq[i].encoded.length(),
                                           const_cast<unsigned char*>
                                           (seq[i].encoded.get_buffer())),
                           idl::legacy::data_export::_tc_ExportedSharedAuth));
      idl::legacy::data_export::ExportedSharedAuth exported_shared_auth(
        extract<idl::legacy::data_export::ExportedSharedAuth>(any));
      secret.busid_ = exported_shared_auth.bus.in();
      secret.legacy_login_process_ = exported_shared_auth.attempt;
      secret.secret_ = idl::core::OctetSeq(
        exported_shared_auth.secret.maximum(),
        exported_shared_auth.secret.length(),
        exported_shared_auth.secret.get_buffer());
      found = true;
    }
  }
  if (!found)
  {
    throw InvalidEncodedStream(
      "Stream de bytes n�o corresponde ao tipo de dado esperado.");
  }
  return secret;
}

void OpenBusContext::onCallDispatch(CallDispatchCallback c) 
{
  boost::lock_guard<boost::mutex> l(_mutex);
  _callDispatchCallback = c;
}

OpenBusContext::CallDispatchCallback OpenBusContext::onCallDispatch() const 
{
  boost::lock_guard<boost::mutex> l(_mutex);
  return _callDispatchCallback;
}

idl::offers::OfferRegistry_ptr OpenBusContext::getOfferRegistry() const
{
  idl::offers::OfferRegistry_var ret;
  Connection *conn(getCurrentConnection());
  if (conn)
  {
    ret = conn->getOfferRegistry();
  }
  return ret._retn();
}

idl::access::LoginRegistry_ptr OpenBusContext::getLoginRegistry() const
{
  idl::access::LoginRegistry_var ret;
  Connection *conn(getCurrentConnection());
  if(conn)
  {
    ret = conn->getLoginRegistry();
  }
  return ret._retn();
}

CORBA::OctetSeq OpenBusContext::encode_exported_versions(
  idl::data_export::VersionedDataSeq exported_version_seq,
  const std::string &tag)
{
  CORBA::Any any;
  any <<= exported_version_seq;  
  CORBA::OctetSeq_var exported_version_seq_cdr(
    _orb_init->codec->encode_value(any));
  
  CORBA::OctetSeq ret;
  std::size_t exported_version_seq_size(exported_version_seq_cdr->length());
  std::size_t size(exported_version_seq_size + tag.size());
  ret.length(static_cast<CORBA::ULong>(size));

  std::memcpy(ret.get_buffer(), tag.c_str(), tag.size());
  std::memcpy(ret.get_buffer() + tag.size(),
              exported_version_seq_cdr->get_buffer(),
              exported_version_seq_size);
  return ret;
}

std::string OpenBusContext::decode_exported_versions(
  const CORBA::OctetSeq &stream,
  idl::data_export::VersionedDataSeq_out exported_version_seq) const
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
    CORBA::Any_var any(_orb_init->codec->decode_value(
                         seq, idl::data_export::_tc_VersionedDataSeq));
    const idl::data_export::VersionedDataSeq *tmp;
    *any >>= tmp;
    idl::data_export::VersionedDataSeq_var ret(
      new idl::data_export::VersionedDataSeq(*tmp));
    exported_version_seq = ret._retn();
  }
  catch (const CORBA::SystemException &e)
  {
    throw InvalidEncodedStream(
      boost::str(
        boost::format("Falha ao extrair VersionedDataSeq. Exce��o lan�ada: \
        '%1%' com minor code '%2%'.") % e._rep_id() % e.minor()));
  }
  return tag;
}
}
