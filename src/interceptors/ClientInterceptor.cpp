// -*- coding: iso-8859-1-unix -*-
#include "openbus/detail/interceptors/ClientInterceptor.hpp"
#include "openbus/detail/interceptors/ORBInitializer.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/detail/any.hpp"
#include "openbus_core-2.1C.h"
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

hash_value session_key(PortableInterceptor::ClientRequestInfo &r) 
{
  hash_value profile_data_hash;
  CORBA::OctetSeq profile(
    r.effective_profile()->profile_data);
  SHA256(profile.get_buffer(), profile.length(), profile_data_hash.c_array());
  return profile_data_hash;
}

Connection &ClientInterceptor::get_current_connection(PortableInterceptor::ClientRequestInfo &r)
{
  log_scope l(log().general_logger(),info_level,
              "ClientInterceptor::get_current_connection");
  Connection *conn(0);
  CORBA::Any_var any(r.get_slot(_orb_init->current_connection));
  idl::OctetSeq seq(extract<idl::OctetSeq>(any));
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
      throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
    }
  }
  assert(conn);
  l.vlog("connection:%p", conn);
  return *conn;
}

CallerChain ClientInterceptor::get_joined_chain(Connection &conn, 
                                                PortableInterceptor::ClientRequestInfo &r)
{
  CORBA::Any_var any(r.get_slot(_orb_init->joined_call_chain));
  idl_cr::SignedData signed_chain(extract<idl_cr::SignedData>(any));
  if (signed_chain.encoded.length() == 0)
  {
    return CallerChain();
  }
  any = _orb_init->codec->decode_value(
    CORBA::OctetSeq(signed_chain.encoded.maximum(),
                    signed_chain.encoded.length(),
                    const_cast<unsigned char *>
                    (signed_chain.encoded.get_buffer())),
    idl_ac::_tc_CallChain);
  // any = _orb_init->codec->decode_value(signed_chain.encoded,
  //                                      idl_ac::_tc_CallChain);
  idl_ac::CallChain chain(extract<idl_ac::CallChain>(any));
  return CallerChain(conn.busid(), chain.target.in(),
                     chain.originators, chain.caller, signed_chain);
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

idl_cr::SignedData ClientInterceptor::get_signed_chain(
  Connection &conn, hash_value &hash, const std::string &entity)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::get_signed_chain");
  idl_cr::SignedData chain;
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
      chain = *conn.access_control()->signChainFor(entity.c_str());
    }
    catch (const CORBA::SystemException &)
    {
      l.vlog("throw CORBA::NO_PERMISSION, minor=UnavailableBusCode, busid=%s", 
             conn.busid().c_str());
      throw CORBA::NO_PERMISSION(idl_ac::UnavailableBusCode,
                                 CORBA::COMPLETED_NO);
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
  PortableInterceptor::ClientRequestInfo &r, Connection &conn, const idl_ac::LoginInfo &curr_login)
{
  idl_cr::CredentialData credential;
  credential.bus = conn._busid.c_str();
  credential.login = curr_login.id;

  std::string remote_id;
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);
#endif
    conn._profile2login.fetch(session_key(r), remote_id);
  }

  Connection::SecretSession *session(0);
  if (!remote_id.empty())
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);
#endif
    session = conn._login2session.fetch_ptr(remote_id);
  }

  if (session == 0) 
  {
    credential.ticket = 0;
    credential.session = 0;
    std::memset(credential.hash, '\0', idl::HashValueSize);
    std::memset(credential.chain.signature, '\0', idl::EncryptedBlockSize);
  }
  else
  {
    credential.session = session->id;
    credential.ticket = ++session->ticket;

    hash_value hash(::openbus::hash(r.operation(), credential.ticket, 
                                    session->secret));
    std::copy(hash.cbegin(), hash.cend(), credential.hash);
        
    CallerChain caller_chain(get_joined_chain(conn, r));
    if (remote_id == std::string(idl::BusLogin)) 
    {
      if (caller_chain == CallerChain())
      {
        std::memset(credential.chain.signature, '\0', idl::EncryptedBlockSize);
      }
      else
      {
        credential.chain = caller_chain._signedCallChain;
      }
    }
    else
    {
      std::string login(curr_login.id.in());
      hash_value hash;
      {
        std::size_t size(login.size() + remote_id.size() 
                         + idl::EncryptedBlockSize);
        boost::scoped_array<unsigned char> buf (new unsigned char[size]());
        std::size_t pos(0);
        std::memcpy(buf.get(), login.c_str(), login.size());
        pos += login.size();
        std::memcpy(buf.get() + pos, remote_id.c_str(), 
                    remote_id.size());
        pos += remote_id.size();
        if (caller_chain != CallerChain())
        {
          std::memcpy(buf.get() + pos, 
                      caller_chain._signedCallChain.signature, 
                      idl::EncryptedBlockSize);
        } 
        else
        {
          std::memset(buf.get() + pos, '\0', idl::EncryptedBlockSize);
        }
        SHA256(buf.get(), size, hash.c_array());
      }
      credential.chain = get_signed_chain(conn, hash, session->entity);
    } 
  }
      
  IOP::ServiceContext sctx;
  sctx.context_id = idl_cr::CredentialContextId;
  CORBA::Any any;
  any <<= credential;
  CORBA::OctetSeq_var o(_orb_init->codec->encode_value(any));
  sctx.context_data = o;

  r.add_request_service_context(sctx, true);
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
  idl_ac::LoginInfo curr_login(conn.get_login());
  if (std::string(curr_login.id.in()).empty())
  {
    l.log("throw NoLoginCode");
    throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
  }
  l.vlog("login: %s", curr_login.id.in());
  CallerChain caller_chain(get_joined_chain(conn, *r));
  build_credential(*r, conn, curr_login);
  CORBA::Any any;
  boost::uuids::uuid request_id = boost::uuids::random_generator()();
  any <<= boost::uuids::to_string(request_id);
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> l(_mutex);
#endif
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
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> l(_mutex);
#endif
    conn = _request_id2conn.find(request_id)->second;
    _request_id2conn.erase(request_id);
  }
  if (ex->minor() == idl_ac::InvalidCredentialCode) 
  {
    l.level_vlog(debug_level, "creating credential session");
    IOP::ServiceContext_var sctx(
      r->get_reply_service_context(idl_cr::CredentialContextId));
    idl_cr::CredentialReset credential_reset;
    try 
    {
      CORBA::Any_var any(
        _orb_init->codec->decode_value(sctx->context_data, idl_cr::_tc_CredentialReset));
      credential_reset = extract<idl_cr::CredentialReset>(any);
    }
    catch (const CORBA::Exception &) 
    {
      throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode,CORBA::COMPLETED_NO);
    }
        
    idl::OctetSeq secret(conn->_key.decrypt(credential_reset.challenge,
                                           idl::EncryptedBlockSize));

    Connection::SecretSession session;
    session.id = credential_reset.session;
    session.remote_id = credential_reset.target.in();
    session.entity = credential_reset.entity.in();
    std::copy(secret.get_buffer(), secret.get_buffer() + secret_size, 
              session.secret.c_array());
    session.ticket = 0;
    {
#ifdef OPENBUS_SDK_MULTITHREAD
      boost::lock_guard<boost::mutex> lock(_mutex);
#endif
      conn->_profile2login.insert(session_key(*r), session.remote_id);
      conn->_login2session.insert(session.remote_id, session);
    }
    l.log("Retransmissao da requisicao...");
    throw PortableInterceptor::ForwardRequest(r->target());
  } 
  else if (ex->minor() == idl_ac::InvalidLoginCode) 
  {
    if (ignore_invalid_login(*r))
    {
      l.log("Ignorando excecao InvalidLogin.");
      throw CORBA::NO_PERMISSION(idl_ac::InvalidLoginCode, CORBA::COMPLETED_NO);
    }
    std::size_t validity(0);
    idl_ac::LoginInfo invalid_login;
    {
#ifdef OPENBUS_SDK_MULTITHREAD
      boost::lock_guard<boost::mutex> conn_lock(conn->_mutex);
#endif
      invalid_login = *conn->_loginInfo;
    }
    try
    { 
      interceptors::ignore_invalid_login i(_orb_init);
      if (_bus_ctx->getLoginRegistry())
      {
        validity = _bus_ctx->getLoginRegistry()->getLoginValidity(
          invalid_login.id);
      }
      else
      {
        l.log("Nao foi possivel obter uma referencia remota para LoginRegistry. Retrasmissao da requisicao.");
        throw PortableInterceptor::ForwardRequest(r->target());
      }
    }
    catch (const CORBA::NO_PERMISSION &e)
    {
      if (idl_ac::InvalidLoginCode == e.minor())
      {
        idl_ac::LoginInfo curr_login;
        {
#ifdef OPENBUS_SDK_MULTITHREAD
          boost::lock_guard<boost::mutex> conn_lock(conn->_mutex);
#endif
          curr_login = *conn->_loginInfo;
        }
        if (std::string(curr_login.id.in())
            == std::string(invalid_login.id.in()))
        {
          conn->_logout(true);
          idl_ac::LoginInfo *obj(new idl_ac::LoginInfo);
          obj->id = invalid_login.id;
          obj->entity = invalid_login.entity;
          {
#ifdef OPENBUS_SDK_MULTITHREAD
            boost::lock_guard<boost::mutex> conn_lock(conn->_mutex);
#endif            
          conn->_invalid_login.reset(obj);
          }
        }
        l.log("Excecao InvalidLogin ao chamar getLoginValidity(). Retransmissa da requisicao.");
        throw PortableInterceptor::ForwardRequest(r->target());
      }
      else
      {
        throw CORBA::NO_PERMISSION(idl_ac::UnavailableBusCode,
                                     CORBA::COMPLETED_NO);
      }
    }
    catch (const CORBA::Exception &)
    {
      throw CORBA::NO_PERMISSION(idl_ac::UnavailableBusCode,
                                 CORBA::COMPLETED_NO);
    }
    if (validity > 0)
    {
      throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode,
                                 CORBA::COMPLETED_NO);
    }
    else
    {
      l.log("getLoginValidity < 0. Retransmissa da requisicao.");
      throw PortableInterceptor::ForwardRequest(r->target());
    }
  }
  else if (idl_ac::NoLoginCode == ex->minor() ||
           idl_ac::UnavailableBusCode == ex->minor() ||
           idl_ac::InvalidTargetCode == ex->minor() ||
           idl_ac::InvalidRemoteCode == ex->minor())
  {
    throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, CORBA::COMPLETED_NO);
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
