// -*- coding: iso-8859-1-unix -*-
#include "openbus/detail/interceptors/client.hpp"
#include "openbus/detail/interceptors/orb_initializer.hpp"
#include "openbus/Connection.hpp"
#include "openbus/openbus_context.hpp"
#include "openbus/detail/any.hpp"
#include "openbus/log.hpp"

#include <tao/AnyTypeCode/AnyTypeCode_Adapter_Impl.h>
#include <tao/AnyTypeCode/ExceptionA.h>
#include <tao/AnyTypeCode/Any_Dual_Impl_T.h>
#include <boost/scoped_ptr.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <openssl/sha.h>
#include <cstddef>
#include <cstring>
#include <sstream>

const std::size_t LRUSize(128);

namespace TAO
{
  template<>
  inline void
  Any_Dual_Impl_T<CORBA::Exception>::value (const CORBA::Exception & val)
  {
    this->value_ = val._tao_duplicate ();
  }

  template<>
  inline CORBA::Boolean
  Any_Dual_Impl_T<CORBA::Exception>::marshal_value (TAO_OutputCDR &cdr)
  {
    try
    {
      this->value_->_tao_encode (cdr);
      return true;
    }
    catch (const CORBA::Exception &)
    {
    }
    return false;
  }

  template<>
  inline CORBA::Boolean
  Any_Dual_Impl_T<CORBA::Exception>::demarshal_value (TAO_InputCDR &cdr)
  {
    try
    {
      this->value_->_tao_decode (cdr);
      return true;
    }
    catch (const CORBA::Exception &)
    {
    }
    return false;
  }

  // This should never get called since we don't have extraction operators
  // for CORBA::Exception, but it is here to sidestep the constructor call
  // in the unspecialized version that causes a problem with compilers that
  // require explicit instantiation
  template<>
  inline CORBA::Boolean
  Any_Dual_Impl_T<CORBA::Exception>::extract(
    const CORBA::Any &,
    _tao_destructor,
    CORBA::TypeCode_ptr,
    const CORBA::Exception *&)
  {
    return false;
  }
}

namespace openbus 
{
  
namespace interceptors 
{ 

hash_value session_key(PortableInterceptor::ClientRequestInfo_ptr r) 
{
  hash_value profile_data_hash;
  CORBA::OctetSeq profile(
    r->effective_profile()->profile_data);
  SHA256(profile.get_buffer(), profile.length(), profile_data_hash.c_array());
  return profile_data_hash;
}

Connection &ClientInterceptor::get_current_connection(PortableInterceptor::ClientRequestInfo &r)
{
  log_scope l(log().general_logger(),info_level,
              "ClientInterceptor::get_current_connection");
  Connection *conn(0);
  CORBA::Any_var any(r.get_slot(_orb_init->current_connection));
  idl::core::OctetSeq seq(extract<idl::core::OctetSeq>(any));
  if (seq.length() > 0)
  {
    assert(seq.length() == sizeof(conn));
    std::memcpy(&conn, seq.get_buffer(), sizeof(conn));
  }
  assert(_bus_ctx);
  if(!conn) 
  {
    if (!(conn = _bus_ctx->getDefaultConnection())) 
    {
      l.log("throw NoLoginCode");
      throw CORBA::NO_PERMISSION(idl::access::NoLoginCode, CORBA::COMPLETED_NO);
    }
  }
  assert(conn);
  l.vlog("connection:%p", conn);
  return *conn;
}

CallerChain ClientInterceptor::get_joined_chain(
  PortableInterceptor::ClientRequestInfo_ptr r,
  Connection &conn)
{
  CORBA::Any_var any(r->get_slot(_orb_init->joined_call_chain));

  idl::creden::SignedData signed_chain(extract<idl::creden::SignedData>(any));
  if (signed_chain.encoded.length())
  {
    any = _orb_init->codec->decode_value(
      CORBA::OctetSeq(signed_chain.encoded.maximum(),
                      signed_chain.encoded.length(),
                      const_cast<unsigned char *>
                      (signed_chain.encoded.get_buffer())),
      idl::access::_tc_CallChain);
    idl::access::CallChain chain(extract<idl::access::CallChain>(any));
    return CallerChain(chain, chain.bus.in(), chain.target.in(), signed_chain);
  }
  else
  {
    idl::legacy::creden::SignedCallChain legacy_signed_chain(
      extract<idl::legacy::creden::SignedCallChain>(any));
    if (legacy_signed_chain.encoded.length())
    {
      any = _orb_init->codec->decode_value(
        CORBA::OctetSeq(legacy_signed_chain.encoded.maximum(),
                        legacy_signed_chain.encoded.length(),
                        const_cast<unsigned char *>
                        (legacy_signed_chain.encoded.get_buffer())),
        idl::legacy::access::_tc_CallChain);
      idl::legacy::access::CallChain chain(extract<idl::legacy::access::CallChain>(any));
      return CallerChain(chain, conn.busid(), chain.target.in(), legacy_signed_chain);
    }
  }
  return CallerChain();
}

bool ClientInterceptor::ignore_request(PortableInterceptor::ClientRequestInfo &r)
{
  CORBA::Any_var any(r.get_slot(_orb_init->ignore_interceptor));
  CORBA::Boolean ignore(false);
  any >>= CORBA::Any::to_boolean(ignore);
  return !!ignore;
}

bool ClientInterceptor::ignore_invalid_login(PortableInterceptor::ClientRequestInfo &r)
{
  CORBA::Any_var any(r.get_slot(_orb_init->ignore_invalid_login));
  CORBA::Boolean ignore(false);
  any >>= CORBA::Any::to_boolean(ignore);
  return !!ignore;
}

idl::legacy::creden::SignedCallChain ClientInterceptor::get_signed_chain(
  Connection &conn,
  hash_value &hash,
  const std::string &target,
  idl::legacy::creden::CredentialData type)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::get_signed_chain");
  idl::legacy::creden::SignedCallChain chain;
  bool cached(false);
  {
    boost::lock_guard<boost::mutex> lock(_mutex);
    cached = _legacy_callChainLRUCache.fetch(hash, chain);
  }
  if (!cached) 
  {
    try
    {
      chain = *conn._legacy_access_control->signChainFor(target.c_str());
    }
    catch (const CORBA::SystemException &)
    {
      l.vlog("throw CORBA::NO_PERMISSION, minor=UnavailableBusCode, busid=%s", 
             conn.busid().c_str());
      throw CORBA::NO_PERMISSION(idl::legacy::access::UnavailableBusCode,
                                 CORBA::COMPLETED_NO);
    }
    catch (const idl::legacy::access::InvalidLogins &)
    {
      Connection::profile2login_LRUCache::Key_List entries(
        conn._profile2login.get_all_keys());
      for (Connection::profile2login_LRUCache::Key_List::iterator it(
             entries.begin()); it != entries.end(); ++it)
      {        
        if (target == conn._profile2login.fetch(*it))
        {
          conn._profile2login.remove(*it);
        }
      }
      l.log("throw CORBA::NO_PERMISSION, minor=InvalidTaretCode");
      throw CORBA::NO_PERMISSION(idl::legacy::access::InvalidTargetCode,CORBA::COMPLETED_NO);
    }

    {
      boost::lock_guard<boost::mutex> lock(_mutex);
      _legacy_callChainLRUCache.insert(hash, chain);
    }
  }
  return chain;
}

idl::creden::SignedData ClientInterceptor::get_signed_chain(
  Connection &conn,
  hash_value &hash,
  const std::string &target,
  idl::creden::CredentialData type)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::get_signed_chain");
  idl::creden::SignedData chain;  
  bool cached(false);
  {
    boost::lock_guard<boost::mutex> lock(_mutex);
    cached = _callChainLRUCache.fetch(hash, chain);
  }
  if (!cached) 
  {
    try
    {
      chain = *conn.access_control()->signChainFor(target.c_str());
    }
    catch (const CORBA::SystemException &)
    {
      l.vlog("throw CORBA::NO_PERMISSION, minor=UnavailableBusCode, busid=%s", 
             conn.busid().c_str());
      throw CORBA::NO_PERMISSION(idl::access::UnavailableBusCode,
                                 CORBA::COMPLETED_NO);
    }

    {
      boost::lock_guard<boost::mutex> lock(_mutex);
      _callChainLRUCache.insert(hash, chain);
    }
  }
  return chain;
}

Connection::SecretSession * ClientInterceptor::get_session(
  PI::ClientRequestInfo_ptr r,
  Connection &conn)
{
  Connection::SecretSession *session(0);
  std::string remote_id;
  boost::lock_guard<boost::mutex> lock(_mutex);
  conn._profile2login.fetch(session_key(r), remote_id);
  if (!remote_id.empty())
  {
    session = conn._login2session.fetch_ptr(remote_id);
  }
  return session;
}

template <typename C>
void ClientInterceptor::fill_credential(
  PI::ClientRequestInfo_ptr r,
  Connection &conn,
  typename boost::mpl::if_<
  typename boost::is_same<C, idl::creden::CredentialData>::type,
  idl::access::LoginInfo,
  idl::legacy::access::LoginInfo>::type &login,
  CallerChain &chain,
  Connection::SecretSession *session)
{
  C credential;
  credential.bus = conn._busid.c_str();
  credential.login = login.id;
  if (session == 0)
  {
    credential.ticket = 0;
    credential.session = 0;
    std::memset(credential.hash, '\0', idl::core::HashValueSize);
    std::memset(credential.chain.signature, '\0', idl::core::EncryptedBlockSize);
  }  
  else
  {
    credential.session = session->id;
    credential.ticket = ++session->ticket;

    hash_value hash(::openbus::hash<C>(
                      r->operation(),
                      credential.ticket,
                      session->secret));
    std::copy(hash.cbegin(), hash.cend(), credential.hash);
        
    if (session->remote_id == std::string(idl::core::BusLogin)) 
    {
      if (chain == CallerChain())
      {
        std::memset(credential.chain.signature, '\0', idl::core::EncryptedBlockSize);
      }
      else
      {
        credential.chain = chain.signed_chain(C());
      }
    }
    else
    {
      std::string login_id(login.id.in());
      hash_value hash;
      {
        std::size_t size(login_id.size() + session->remote_id.size() 
                         + idl::core::EncryptedBlockSize);
        boost::scoped_array<unsigned char> buf (new unsigned char[size]());
        std::size_t pos(0);
        std::memcpy(buf.get(), login_id.c_str(), login_id.size());
        pos += login_id.size();
        std::memcpy(buf.get() + pos, session->remote_id.c_str(), 
                    session->remote_id.size());
        pos += session->remote_id.size();
        if (chain != CallerChain())
        {
          std::memcpy(buf.get() + pos, 
                      chain.signed_chain(C()).signature, 
                      idl::core::EncryptedBlockSize);
        } 
        else
        {
          std::memset(buf.get() + pos, '\0', idl::core::EncryptedBlockSize);
        }
        SHA256(buf.get(), size, hash.c_array());
      }
      std::string target;
      if (boost::is_same<C, idl::creden::CredentialData>::value)
      {
        target = session->entity;
      }
      else
      {
        target = session->remote_id;
      }
      credential.chain = get_signed_chain(conn, hash, target, C());
    }
  }
      
  IOP::ServiceContext sctx;
  sctx.context_id =
    (boost::is_same<C, idl::creden::CredentialData>::value) ?
    idl::creden::CredentialContextId :
    idl::legacy::creden::CredentialContextId;
  CORBA::Any any;
  any <<= credential;
  CORBA::OctetSeq_var o(_orb_init->codec->encode_value(any));
  sctx.context_data = o;

  r->add_request_service_context(sctx, true);
}
  
void ClientInterceptor::attach_credential(
  PI::ClientRequestInfo_ptr r,
  Connection &conn,
  idl::access::LoginInfo &login)
{
  idl::legacy::access::LoginInfo legacy_login;
  legacy_login.id = login.id;
  legacy_login.entity = login.entity;

  Connection::SecretSession *session(get_session(r, conn));
  CallerChain chain(get_joined_chain(r, conn));
  if (session == 0)
  {
    fill_credential<idl::creden::CredentialData>(
      r, conn, login, chain, session);
    fill_credential<idl::legacy::creden::CredentialData>(
      r, conn, legacy_login, chain, session);
    return;
  }
  
  if (session->is_legacy || chain.is_legacy())
    fill_credential<idl::legacy::creden::CredentialData>(
      r, conn, legacy_login, chain, session);
  else
    fill_credential<idl::creden::CredentialData>(
      r, conn, login, chain, session);
}

boost::uuids::uuid ClientInterceptor::get_request_id(
  PortableInterceptor::ClientRequestInfo_ptr r)
{
  CORBA::Any_var any(r->get_slot(_orb_init->request_id));
  const char *tmp;
  if (*any >>= tmp)
  {
    return boost::uuids::string_generator()(tmp);
  }
  else
  {
    return boost::uuids::nil_generator()();
  }
}

ClientInterceptor::ClientInterceptor(ORBInitializer *orb_init)
  : _orb_init(orb_init),
    _callChainLRUCache(LRUSize),
    _legacy_callChainLRUCache(LRUSize),
    _bus_ctx(0)
{ 
  log_scope l(log().general_logger(), info_level, 
              "ClientInterceptor::ClientInterceptor");
}

void ClientInterceptor::send_request(PortableInterceptor::ClientRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::send_request");
  l.level_vlog(debug_level, "operation: %s", r->operation());
  if (ignore_request(*r))
  {
    return;
  }
  Connection &conn(get_current_connection(*r));
  idl::access::LoginInfo curr_login(conn.get_login());
  if (std::string(curr_login.id.in()).empty())
  {
    l.log("throw NoLoginCode");
    throw CORBA::NO_PERMISSION(idl::access::NoLoginCode, CORBA::COMPLETED_NO);
  }
  l.vlog("login: %s", curr_login.id.in());

  attach_credential(r, conn, curr_login);
  
  CORBA::Any any;
  boost::uuids::uuid request_id = boost::uuids::random_generator()();
  any <<= boost::uuids::to_string(request_id);
  {
    boost::lock_guard<boost::mutex> l(_mutex);
    _request_id2conn[request_id] = &conn;
  }
  _orb_init->pi_current->set_slot(_orb_init->request_id, any);  
}

void ClientInterceptor::receive_exception(PortableInterceptor::ClientRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::receive_exception");
  l.level_vlog(debug_level, "operation: %s", r->operation()); 
  l.level_vlog(debug_level, "exception: %s", r->received_exception_id()); 

  if (ignore_request(*r))
  {
    throw;
  }

  CORBA::Any_var any = r->received_exception();

  TAO::Any_Dual_Impl_T<CORBA::Exception> *any_impl =
    static_cast <TAO::Any_Dual_Impl_T<CORBA::Exception> *> (any->impl());
  if (!any_impl)
  {
    return;
  }
  const CORBA::Exception *p =
    static_cast<const CORBA::Exception *>(any_impl->value());
  const CORBA::NO_PERMISSION *ex = static_cast<const CORBA::NO_PERMISSION *>(p);
  if (ex->completed() != CORBA::COMPLETED_NO) 
  {
    return;
  }

  l.level_vlog(debug_level, "minor: %d", ex->minor());
  boost::uuids::uuid request_id(get_request_id(r));
  if (request_id.is_nil())
  {
    throw;
  }
  Connection *conn(0);
  {
    boost::lock_guard<boost::mutex> l(_mutex);
    conn = _request_id2conn.find(request_id)->second;
    _request_id2conn.erase(request_id);
  }
  if (ex->minor() == idl::access::InvalidCredentialCode) 
  {
    l.level_vlog(debug_level, "creating credential session");
    idl::creden::CredentialReset credential_reset;
    Connection::SecretSession session;
    try 
    {
      l.log("Verificando CredentialReset");
      IOP::ServiceContext_var sctx(
        r->get_reply_service_context(idl::creden::CredentialContextId));
      CORBA::Any_var any(
        _orb_init->codec->decode_value(sctx->context_data, idl::creden::_tc_CredentialReset));
      credential_reset = extract<idl::creden::CredentialReset>(any);
      idl::core::OctetSeq secret(conn->_key.decrypt(credential_reset.challenge,
                                              idl::core::EncryptedBlockSize));

      session.id = credential_reset.session;
      session.remote_id = credential_reset.target.in();
      session.entity = credential_reset.entity.in();
      std::copy(secret.get_buffer(), secret.get_buffer() + secret_size, 
                session.secret.c_array());
      session.ticket = 0;
    }
    catch (const CORBA::Exception &) 
    {
      l.log("Verificando CredentialReset legado");
      idl::legacy::creden::CredentialReset credential_reset;
      try 
      {
        IOP::ServiceContext_var sctx(
          r->get_reply_service_context(idl::legacy::creden::CredentialContextId));
        CORBA::Any_var any(
          _orb_init->codec->decode_value(sctx->context_data,
                                         idl::legacy::creden::_tc_CredentialReset));
        credential_reset = extract<idl::legacy::creden::CredentialReset>(any);
        idl::core::OctetSeq secret(conn->_key.decrypt(credential_reset.challenge,
                                                idl::core::EncryptedBlockSize));

        session.id = credential_reset.session;
        session.remote_id = credential_reset.target.in();
        std::copy(secret.get_buffer(), secret.get_buffer() + secret_size, 
                  session.secret.c_array());
        session.ticket = 0;
        session.is_legacy = true;
        l.log("Credential Reset legado obtido");
      }
      catch (const CORBA::Exception &) 
      {      
        l.log("Lancando InvalidRemoteCode");
        throw CORBA::NO_PERMISSION(idl::access::InvalidRemoteCode,
                                   CORBA::COMPLETED_NO);
      }
    }
        
    {
      boost::lock_guard<boost::mutex> lock(_mutex);
      conn->_profile2login.insert(session_key(r), session.remote_id);
      conn->_login2session.insert(session.remote_id, session);
    }
    l.log("Retransmissao da requisicao...");
    throw PortableInterceptor::ForwardRequest(r->target());
  } 
  else if (ex->minor() == idl::access::InvalidLoginCode) 
  {
    if (ignore_invalid_login(*r))
    {
      l.log("Ignorando excecao InvalidLogin.");
      throw CORBA::NO_PERMISSION(idl::access::InvalidLoginCode, CORBA::COMPLETED_NO);
    }
    std::size_t validity(0);
    idl::access::LoginInfo invalid_login;
    {
      boost::lock_guard<boost::mutex> conn_lock(conn->_mutex);
      invalid_login = *conn->_loginInfo;
    }
    try
    { 
      interceptors::ignore_invalid_login i(_orb_init);
      idl::access::LoginRegistry_ptr login_registry(_bus_ctx->getLoginRegistry());
      if (login_registry)
      {
        validity = login_registry->getLoginValidity(invalid_login.id);
      }
      else
      {
        l.log("Nao foi possivel obter uma referencia remota para LoginRegistry. Retrasmissao da requisicao.");
        throw PortableInterceptor::ForwardRequest(r->target());
      }
    }
    catch (const CORBA::NO_PERMISSION &e)
    {
      if (idl::access::InvalidLoginCode != e.minor())
      {
        throw CORBA::NO_PERMISSION(idl::access::UnavailableBusCode,
                                   CORBA::COMPLETED_NO);
      }
      
      idl::access::LoginInfo curr_login;
      {
        boost::lock_guard<boost::mutex> conn_lock(conn->_mutex);
        curr_login = *conn->_loginInfo;
      }
      if (std::string(curr_login.id.in())
          == std::string(invalid_login.id.in())) 
      {
        conn->_logout(true);
        idl::access::LoginInfo *obj(new idl::access::LoginInfo);
        obj->id = invalid_login.id;
        obj->entity = invalid_login.entity;
        {
          boost::lock_guard<boost::mutex> conn_lock(conn->_mutex);
          conn->_invalid_login.reset(obj);
        }
      }
      l.log("Excecao InvalidLogin ao chamar getLoginValidity(). Retransmissa da requisicao.");
      throw PortableInterceptor::ForwardRequest(r->target());
    }
    catch (const CORBA::Exception &)
    {
      throw CORBA::NO_PERMISSION(idl::access::UnavailableBusCode,
                                 CORBA::COMPLETED_NO);
    }
    if (validity > 0)
    {
      throw CORBA::NO_PERMISSION(idl::access::InvalidRemoteCode,
                                 CORBA::COMPLETED_NO);
    }
    else
    {
      l.log("getLoginValidity < 0. Retransmissa da requisicao.");
      throw PortableInterceptor::ForwardRequest(r->target());
    }
  }
  else if (idl::access::NoLoginCode == ex->minor() ||
           idl::access::UnavailableBusCode == ex->minor() ||
           idl::access::InvalidTargetCode == ex->minor() ||
           idl::access::InvalidRemoteCode == ex->minor())
  {
    throw CORBA::NO_PERMISSION(idl::access::InvalidRemoteCode, CORBA::COMPLETED_NO);
  }
  else
  {
    throw;
  }
}

void ClientInterceptor::send_poll(PortableInterceptor::ClientRequestInfo_ptr) 
{ 
}

void ClientInterceptor::receive_reply(
  PortableInterceptor::ClientRequestInfo_ptr r) 
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::receive_reply");
  if (ignore_request(*r))
  {
    return;
  }
  boost::uuids::uuid request_id(get_request_id(r));
  if (!request_id.is_nil())
  {
    _request_id2conn.erase(request_id);
  }
}

void ClientInterceptor::receive_other(PortableInterceptor::ClientRequestInfo_ptr) 
{  
}

char *ClientInterceptor::name() 
{ 
  return CORBA::string_dup("ClientInterceptor"); 
}

void ClientInterceptor::destroy() 
{ 
}

}}
