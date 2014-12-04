// -*- coding: iso-8859-1-unix -*-
#include "openbus/interceptors/ClientInterceptor_impl.hpp"
#include "openbus/interceptors/ORBInitializer_impl.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "coreC.h"
#include "credential_v1_5C.h"
#include "openbus/log.hpp"

//??
#include <tao/AnyTypeCode/AnyTypeCode_Adapter_Impl.h>
#include <tao/AnyTypeCode/ExceptionA.h>
#include <tao/AnyTypeCode/Any_Dual_Impl_T.h>
#include <boost/scoped_ptr.hpp>
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

hash_value session_key(PortableInterceptor::ClientRequestInfo &r) 
{
  hash_value profile_data_hash;
  CORBA::OctetSeq profile(
    r.effective_profile()->profile_data);
  SHA256(profile.get_buffer(), profile.length(), profile_data_hash.c_array());
  return profile_data_hash;
}

Connection &ClientInterceptor::get_current_connection(
  PortableInterceptor::ClientRequestInfo &r)
{
  log_scope l(log().general_logger(),info_level,
              "ClientInterceptor::get_current_connection");
  Connection *conn(0);
  CORBA::Any_var connectionAddrAny(r.get_slot(_orb_info->slot.requester_conn));
  const idl::OctetSeq *connectionAddrOctetSeq;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) 
  {
    assert(connectionAddrOctetSeq->length() == sizeof(conn));
    std::memcpy(&conn, connectionAddrOctetSeq->get_buffer(), sizeof(conn));
  }
  assert(_openbus_ctx);
  if(!conn) 
  {
    if (!(conn = _openbus_ctx->getDefaultConnection())) 
    {
      l.log("throw NoLoginCode");
      throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
    }
  }
  assert(conn);
  l.vlog("connection:%p", conn);
  return *conn;
}

CallerChain ClientInterceptor::get_joined_chain(
  Connection &conn,
  PortableInterceptor::ClientRequestInfo &r)
{
  CORBA::Any_var any(r.get_slot(_orb_info->slot.joined_call_chain));
  const idl_cr::SignedCallChain *signed_chain;
  if (*any >>= signed_chain) 
  {
    try
    {      
      CORBA::Any_var any(_orb_info->codec->decode_value(
        CORBA::OctetSeq(signed_chain->encoded.maximum(),
                        signed_chain->encoded.length(),
                        const_cast<unsigned char *>
                        (signed_chain->encoded.get_buffer())),
        idl_ac::_tc_CallChain));
      const idl_ac::CallChain *chain;
      if (*any >>= chain)
      {
        return CallerChain(conn.busid(), conn.login()->entity.in(), 
                           chain->originators, chain->caller, *signed_chain);
      }
    }
    catch (const IOP::Codec::FormatMismatch &)
    {
      /* empty */
    }
  }
  return CallerChain();
}

bool ClientInterceptor::ignore_request(
  PortableInterceptor::ClientRequestInfo &r)
{
  CORBA::Any_var any(r.get_slot(_orb_info->slot.ignore_interceptor));
  CORBA::Boolean ignore(false);
  any >>= CORBA::Any::to_boolean(ignore);
  return !!ignore;
}

idl_cr::SignedCallChain ClientInterceptor::get_signed_chain(
  Connection &conn,
  hash_value &hash,
  const std::string &remote_id)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::get_signed_chain");
  idl_cr::SignedCallChain chain;
  bool cached(false);
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);
#endif
    cached = _callChainLRUCache.fetch(hash, chain);
  }
  if (!cached) 
  {
    try
    {
      chain = *conn.access_control()->signChainFor(remote_id.c_str());
    }
    catch (const CORBA::SystemException &)
    {
      l.vlog("throw CORBA::NO_PERMISSION, minor=UnavailableBusCode, busid=%s", 
             conn.busid().c_str());
      throw CORBA::NO_PERMISSION(idl_ac::UnavailableBusCode,
                                 CORBA::COMPLETED_NO);
    }
    catch (const idl_ac::InvalidLogins &)
    {
      l.log("throw CORBA::NO_PERMISSION, minor=InvalidTaretCode");
      throw CORBA::NO_PERMISSION(idl_ac::InvalidTargetCode,CORBA::COMPLETED_NO);
    }

    {
#ifdef OPENBUS_SDK_MULTITHREAD
      boost::lock_guard<boost::mutex> lock(_mutex);
#endif
      _callChainLRUCache.insert(hash, chain);
    }
  }
  return chain;
}

void ClientInterceptor::build_credential(
  PortableInterceptor::ClientRequestInfo &r,
  Connection &conn)
{
  idl_cr::CredentialData credential;
  credential.bus = CORBA::string_dup(conn._busid.c_str());
  credential.login = CORBA::string_dup(conn._login()->id);

  Connection::SecretSession session;
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);
#endif
    conn._profile2session.fetch(session_key(r), session);
  }

  if (session == Connection::SecretSession()) 
  {
    credential.ticket = 0;
    credential.session = 0;
    std::memset(credential.hash, '\0', idl::HashValueSize);
    std::memset(credential.chain.signature, '\0', idl::EncryptedBlockSize);
  }
  else
  {
    credential.session = session.id;
    credential.ticket = ++session.ticket;

    hash_value hash(::openbus::hash(r.operation(), credential.ticket, 
                                    session.secret));
    std::copy(hash.cbegin(), hash.cend(), credential.hash);
        
    CallerChain caller_chain(get_joined_chain(conn, r));
    if (session.remote_id == std::string(idl::BusLogin)) 
    {
      if (caller_chain == CallerChain())
      {
        std::memset(credential.chain.signature, '\0', idl::EncryptedBlockSize);
      }
      else
      {
        credential.chain = caller_chain.signedCallChain();
      }
    }
    else
    {
      std::string login(conn._login()->id.in());
      hash_value hash;
      {
        std::size_t size(login.size() + session.remote_id.size() 
                         + idl::EncryptedBlockSize);
        boost::scoped_array<unsigned char> buf (new unsigned char[size]());
        std::size_t pos(0);
        std::memcpy(buf.get(), login.c_str(), login.size());
        pos += login.size();
        std::memcpy(buf.get() + pos, session.remote_id.c_str(), 
                    session.remote_id.size());
        pos += session.remote_id.size();
        if (caller_chain != CallerChain())
        {
          std::memcpy(buf.get() + pos, 
                      caller_chain.signedCallChain().signature, 
                      idl::EncryptedBlockSize);
        } 
        else
        {
          std::memset(buf.get() + pos, '\0', idl::EncryptedBlockSize);
        }
        SHA256(buf.get(), size, hash.c_array());
      }
      credential.chain = get_signed_chain(conn, hash, session.remote_id);
    } 
  }
      
  IOP::ServiceContext sctx;
  sctx.context_id = idl_cr::CredentialContextId;
  CORBA::Any any;
  any <<= credential;
  CORBA::OctetSeq_var o(_orb_info->codec->encode_value(any));
  sctx.context_data = o;

  r.add_request_service_context(sctx, true);
}

void ClientInterceptor::build_legacy_credential(
  PortableInterceptor::ClientRequestInfo &r, 
  Connection &conn)
{
  legacy::v1_5::Credential credential;
  credential.identifier = CORBA::string_dup(conn._login()->id);
  credential.owner = CORBA::string_dup(conn._login()->entity);
  CallerChain caller_chain(get_joined_chain(conn, r));
  if (caller_chain == CallerChain())
  {
    credential.delegate = "";
  }
  else
  {
    if (conn._legacyDelegate == Connection::ORIGINATOR 
        && caller_chain._originators.length())
    {
      credential.delegate = 
        CORBA::string_dup(caller_chain._originators[0].entity);
    }
    else
    {
      credential.delegate = 
        CORBA::string_dup(caller_chain._caller.entity); 
    }
  }
  IOP::ServiceContext sctx;
  sctx.context_id = 1234;
  CORBA::Any any;
  any <<= credential;
  CORBA::OctetSeq_var o(_orb_info->codec->encode_value(any));
  sctx.context_data = o;
  r.add_request_service_context(sctx, true);
}

ClientInterceptor::ClientInterceptor(boost::shared_ptr<orb_info> p)
  : _orb_info(p), _callChainLRUCache(LRUSize)
{ 
  log_scope l(log().general_logger(), info_level, 
              "ClientInterceptor::ClientInterceptor");
}

void ClientInterceptor::send_request(
  PortableInterceptor::ClientRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::send_request");
  l.level_vlog(debug_level, "operation: %s", r->operation());
  if (ignore_request(*r))
  {
    return;
  }
  Connection &conn(get_current_connection(*r));
  if (!conn._login()) 
  {
    l.log("throw NoLoginCode");
    throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
  }
  l.vlog("login: %s", conn._login()->id.in());
  build_credential(*r, conn);
  if (conn._legacyEnabled)
  {
    build_legacy_credential(*r, conn);
  }
}

void ClientInterceptor::receive_exception(
  PortableInterceptor::ClientRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::receive_exception");
  l.level_vlog(debug_level, "operation: %s", r->operation()); 
  l.level_vlog(debug_level, "exception: %s", r->received_exception_id()); 

  CORBA::Any_var any = r->received_exception();

  TAO::Any_Dual_Impl_T<CORBA::Exception> *any_impl =
    dynamic_cast <TAO::Any_Dual_Impl_T<CORBA::Exception> *> (any->impl());
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
  Connection &conn(get_current_connection(*r));
  if (ex->minor() == idl_ac::InvalidCredentialCode) 
  {
    l.level_vlog(debug_level, "creating credential session");
    IOP::ServiceContext_var sctx;
    sctx = r->get_reply_service_context(idl_cr::CredentialContextId);
    CORBA::Any_var any;
    const idl_cr::CredentialReset *credential_reset;
    try 
    {
      any = _orb_info->codec->decode_value(sctx->context_data,
                                           idl_cr::_tc_CredentialReset);
      *any >>= credential_reset;
    }
    catch (const CORBA::Exception &) 
    {
      throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode,CORBA::COMPLETED_NO);
    }
        
    idl::OctetSeq secret(conn._key.decrypt(credential_reset->challenge,
                                           idl::EncryptedBlockSize));

    Connection::SecretSession session;
    session.id = credential_reset->session;
    session.remote_id = credential_reset->target.in();
    std::copy(secret.get_buffer(), secret.get_buffer() + secret_size, 
              session.secret.c_array());
    session.ticket = 0;
    {
#ifdef OPENBUS_SDK_MULTITHREAD
      boost::lock_guard<boost::mutex> lock(_mutex);
#endif
      conn._profile2session.insert(session_key(*r), session);
    }
    l.log("Retransmissao da requisicao...");
    throw PortableInterceptor::ForwardRequest(r->target());
  } 
  else if (ex->minor() == idl_ac::NoCredentialCode) 
  {
    throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, CORBA::COMPLETED_NO);
  }
  else if (ex->minor() == idl_ac::InvalidLoginCode) 
  {
    idl_ac::LoginInfo oldLogin;
    {
#ifdef OPENBUS_SDK_MULTITHREAD
      boost::lock_guard<boost::mutex> conn_lock(conn._mutex);
#endif
      oldLogin = *conn._loginInfo;
      conn._state = Connection::INVALID;
    }

    try 
    {
      if (conn.onInvalidLogin()) 
      {
        conn.onInvalidLogin()(conn, oldLogin);
      }
    } 
    catch (...) 
    {
      l.level_log(warning_level, 
                  "Falha na execucao da callback OnInvalidLogin.");
    }

    if (conn._state == Connection::LOGGED)
    {
      throw PortableInterceptor::ForwardRequest(r->target()); 
    }
    else if (conn._state == Connection::UNLOGGED) 
    {
      l.log("Connection::UNLOGGED: throw NoLoginCode");
      throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
    } 
    else if (conn._state == Connection::INVALID) 
    {
      if (!std::strcmp(conn._login()->id, oldLogin.id)) 
      {
        conn._logout();
        l.log("Connection::INVALID: throw NoLoginCode");
        throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
      }
      else
      {
        throw PortableInterceptor::ForwardRequest(r->target());
      }
    }
  }
}

void ClientInterceptor::send_poll(PortableInterceptor::ClientRequestInfo_ptr) 
{ 
}

void ClientInterceptor::receive_reply(
  PortableInterceptor::ClientRequestInfo_ptr)
{ 
}

void ClientInterceptor::receive_other(
  PortableInterceptor::ClientRequestInfo_ptr)
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
