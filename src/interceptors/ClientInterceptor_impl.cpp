// -*- coding: iso-8859-1-unix -*-
#include "openbus/interceptors/ClientInterceptor_impl.hpp"
#include "openbus/interceptors/ORBInitializer_impl.hpp"
#include "openbus/Connection.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/any.hpp"
#include "stubs/core.h"
#include "stubs/credential_v1_5.h"
#include "openbus/log.hpp"

#include <boost/scoped_ptr.hpp>
#include <openssl/sha.h>
#include <cstddef>
#include <cstring>
#include <sstream>

const std::size_t LRUSize(128);

namespace openbus 
{
namespace interceptors 
{ 

hash_value session_key(PI::ClientRequestInfo &r) 
{
  hash_value profile_data_hash;
  ::IOP::TaggedProfile::_profile_data_seq profile(
    r.effective_profile()->profile_data);
  SHA256(profile.get_buffer(), profile.length(), profile_data_hash.c_array());
  return profile_data_hash;
}

Connection &ClientInterceptor::get_current_connection(PI::ClientRequestInfo &r)
{
  log_scope l(log().general_logger(),info_level,
              "ClientInterceptor::get_current_connection");
  Connection *conn(0);
  CORBA::Any_var any(r.get_slot(_orb_info->slot.current_connection));
  idl::OctetSeq seq(extract<idl::OctetSeq>(any));
  if (seq.length() > 0)
  {
    assert(seq.length() == sizeof(conn));
    std::memcpy(&conn, seq.get_buffer(), sizeof(conn));
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

CallerChain ClientInterceptor::get_joined_chain(Connection &conn, 
                                                PI::ClientRequestInfo &r)
{
  CORBA::Any_var any(r.get_slot(_orb_info->slot.joined_call_chain));
  idl_cr::SignedCallChain signed_chain(extract<idl_cr::SignedCallChain>(any));
  if (signed_chain.encoded.length() == 0)
  {
    return CallerChain();
  }
  any = _codec->decode_value(signed_chain.encoded,
                                       idl_ac::_tc_CallChain);
  idl_ac::CallChain chain(extract<idl_ac::CallChain>(any));
  return CallerChain(conn.busid(), chain.target.in(),
                     chain.originators, chain.caller, signed_chain);
}

bool ClientInterceptor::ignore_request(PI::ClientRequestInfo &r)
{
  CORBA::Any_var any(r.get_slot(_orb_info->slot.ignore_interceptor));
  CORBA::Boolean ignore(false);
  any >>= CORBA::Any::to_boolean(ignore);
  return !!ignore;
}

bool ClientInterceptor::ignore_invalid_login(PI::ClientRequestInfo &r)
{
  CORBA::Any_var any(r.get_slot(_orb_info->slot.ignore_invalid_login));
  CORBA::Boolean ignore(false);
  any >>= CORBA::Any::to_boolean(ignore);
  return !!ignore;
}

idl_cr::SignedCallChain ClientInterceptor::get_signed_chain(
  Connection &conn, hash_value &hash, const std::string &remote_id)
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
      Connection::profile2login_LRUCache::Key_List entries(
        conn._profile2login.get_all_keys());
      for (Connection::profile2login_LRUCache::Key_List::iterator it(
             entries.begin()); it != entries.end(); ++it)
      {        
        if (remote_id == conn._profile2login.fetch(*it))
        {
          conn._profile2login.remove(*it);
        }
      }
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
  PI::ClientRequestInfo &r, Connection &conn, const idl_ac::LoginInfo &curr_login)
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

  Connection::SecretSession session;
  if (!remote_id.empty())
  {
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::lock_guard<boost::mutex> lock(_mutex);
#endif
    conn._login2session.fetch(remote_id, session);
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
      credential.chain = get_signed_chain(conn, hash, remote_id);
    } 
  }
      
  IOP::ServiceContext sctx;
  sctx.context_id = idl_cr::CredentialContextId;
  CORBA::Any any;
  any <<= credential;
  CORBA::OctetSeq_var o(_codec->encode_value(any));
  sctx.context_data = o;

  r.add_request_service_context(sctx, true);
}

void ClientInterceptor::build_legacy_credential(
  PI::ClientRequestInfo &r, 
  Connection &conn,
  const idl_ac::LoginInfo &curr_login)
{
  legacy::v1_5::Credential credential;
  credential.identifier = curr_login.id;
  credential.owner = curr_login.entity;
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
      credential.delegate = caller_chain.originators()[0].entity;
    }
    else
    {
      credential.delegate = caller_chain.caller().entity; 
    }
  }
  IOP::ServiceContext sctx;
  sctx.context_id = 1234;
  CORBA::Any any;
  any <<= credential;
  CORBA::OctetSeq_var o(_codec->encode_value(any));
  sctx.context_data = o;
  r.add_request_service_context(sctx, true);
}

ClientInterceptor::ClientInterceptor(boost::shared_ptr<orb_info> p)
  : _orb_info(p), _codec(_orb_info->codec), _callChainLRUCache(LRUSize)
{ 
  log_scope l(log().general_logger(), info_level, 
              "ClientInterceptor::ClientInterceptor");
}

void ClientInterceptor::send_request(PI::ClientRequestInfo_ptr r)
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
  if (caller_chain.is_legacy())
  {
    if (conn._legacyEnabled)
    {
      build_legacy_credential(*r, conn, curr_login);
      return;
    }
    throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
  }
  build_credential(*r, conn, curr_login);
}

void ClientInterceptor::receive_exception(PI::ClientRequestInfo_ptr r)
{
  log_scope l(log().general_logger(), debug_level, 
              "ClientInterceptor::receive_exception");
  l.level_vlog(debug_level, "operation: %s", r->operation()); 
  l.level_vlog(debug_level, "exception: %s", r->received_exception_id()); 

  if (std::string(r->received_exception_id())
      != "IDL:omg.org/CORBA/NO_PERMISSION:1.0")
  {
    return;
  }

  CORBA::SystemException &ex(
    *CORBA::SystemException::_decode(*r->received_exception()));
  if (ex.completed() != CORBA::COMPLETED_NO) 
  {
    return;
  }

  l.level_vlog(debug_level, "minor: %d", ex.minor());
  Connection &conn(get_current_connection(*r));
  if (ex.minor() == idl_ac::InvalidCredentialCode) 
  {
    l.level_vlog(debug_level, "creating credential session");
    IOP::ServiceContext_var sctx(
      r->get_reply_service_context(idl_cr::CredentialContextId));
    idl_cr::CredentialReset credential_reset;
    try 
    {
      CORBA::Any_var any(
        _codec->decode_value(sctx->context_data, idl_cr::_tc_CredentialReset));
      credential_reset = extract<idl_cr::CredentialReset>(any);
    }
    catch (const CORBA::Exception &) 
    {
      throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode,CORBA::COMPLETED_NO);
    }
        
    CORBA::OctetSeq secret(conn._key.decrypt(credential_reset.challenge,
                                             idl::EncryptedBlockSize));

    Connection::SecretSession session;
    session.id = credential_reset.session;
    session.remote_id = credential_reset.target.in();
    std::copy(secret.get_buffer(), secret.get_buffer() + secret_size, 
              session.secret.c_array());
    session.ticket = 0;
    {
#ifdef OPENBUS_SDK_MULTITHREAD
      boost::lock_guard<boost::mutex> lock(_mutex);
#endif
      conn._profile2login.insert(session_key(*r), session.remote_id);
      conn._login2session.insert(session.remote_id, session);
    }
    l.log("Retransmissao da requisicao...");
    throw PI::ForwardRequest(r->target(), false);
  } 
  else if (ex.minor() == idl_ac::InvalidLoginCode) 
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
      boost::lock_guard<boost::mutex> conn_lock(conn._mutex);
#endif
      invalid_login = *conn._loginInfo;
    }
    try
    { 
      interceptors::ignore_invalid_login i(_orb_info);
      if (_openbus_ctx->getLoginRegistry())
      {
        validity = _openbus_ctx->getLoginRegistry()->getLoginValidity(
          invalid_login.id);
      }
      else
      {
        l.log("Nao foi possivel obter uma referencia remota para LoginRegistry. Retrasmissao da requisicao.");
        throw PI::ForwardRequest(r->target(), false);
      }
    }
    catch (const CORBA::NO_PERMISSION &e)
    {
      if (idl_ac::InvalidLoginCode == e.minor())
      {
        idl_ac::LoginInfo curr_login;
        {
#ifdef OPENBUS_SDK_MULTITHREAD
          boost::lock_guard<boost::mutex> conn_lock(conn._mutex);
#endif
          curr_login = *conn._loginInfo;
        }
        if (std::string(curr_login.id.in())
            == std::string(invalid_login.id.in()))
        {
          conn._logout(true);
          idl_ac::LoginInfo *obj(new idl_ac::LoginInfo);
          obj->id = invalid_login.id;
          obj->entity = invalid_login.entity;
          {
#ifdef OPENBUS_SDK_MULTITHREAD
            boost::lock_guard<boost::mutex> conn_lock(conn._mutex);
#endif            
          conn._invalid_login.reset(obj);
          }
        }
        l.log("Excecao InvalidLogin ao chamar getLoginValidity(). Retransmissa da requisicao.");
        throw PI::ForwardRequest(r->target(), false);
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
      throw PI::ForwardRequest(r->target(), false);
    }
  }
  else if (idl_ac::NoLoginCode == ex.minor() ||
           idl_ac::UnavailableBusCode == ex.minor() ||
           idl_ac::InvalidTargetCode == ex.minor() ||
           idl_ac::InvalidRemoteCode == ex.minor())
  {
    throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, CORBA::COMPLETED_NO);
  }
  else
  {
    throw CORBA::NO_PERMISSION(ex.minor(), CORBA::COMPLETED_NO);
  }
}

void ClientInterceptor::send_poll(PI::ClientRequestInfo_ptr) 
{ 
}

void ClientInterceptor::receive_reply(PI::ClientRequestInfo_ptr) 
{ 
}

void ClientInterceptor::receive_other(PI::ClientRequestInfo_ptr) 
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
