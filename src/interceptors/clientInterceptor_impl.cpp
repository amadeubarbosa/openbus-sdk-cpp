#include <openssl/sha.h>
#include <sstream>

#include <openbus/interceptors/clientInterceptor_impl.h>
#include <legacy/stubs/credential_v1_5.h>
#include <openbus/log.h>
#include <openbus/util/openssl.h>
#include <openbus/util/autolock_impl.h>

namespace openbus {
namespace interceptors {    

PortableInterceptor::SlotId ClientInterceptor::_slotId_ignoreInterceptor;

/* monta uma indentificador(chave) para uma requisição através de uma hash do profile. */
std::string getSessionKey(PortableInterceptor::ClientRequestInfo* r) {
  idl::HashValue profileDataHash;
  ::IOP::TaggedProfile::_profile_data_seq profile = r->effective_profile()->profile_data;
  SHA256(profile.get_buffer(), profile.length(), profileDataHash);
  std::string sprofileDataHash((const char*) profileDataHash, 32);
  return sprofileDataHash;
}

Connection& ClientInterceptor::getCurrentConnection(PortableInterceptor::ClientRequestInfo* r) {
  log_scope l(log.general_logger(),info_level,"ClientInterceptor::getCurrentConnection");
  Connection* conn = 0;
  CORBA::Any_var connectionAddrAny;
  connectionAddrAny = r->get_slot(_slotId_connectionAddr);
  idl::OctetSeq connectionAddrOctetSeq;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) {
    assert(connectionAddrOctetSeq.length() == sizeof(conn));
    std::memcpy(&conn, connectionAddrOctetSeq.get_buffer(), sizeof(conn));
  }
  assert(_manager);
  if(!conn)
    if (!(conn = _manager->getDefaultConnection()))
      throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
  assert(conn);
  l.vlog("connection:%p", conn);
  return *conn;
}

CallerChain* ClientInterceptor::getJoinedChain(PortableInterceptor::ClientRequestInfo* r) {
  CORBA::Any_var signedCallChainAny= r->get_slot(_slotId_joinedCallChain);
  idl_cr::SignedCallChain signedCallChain;
  if (*signedCallChainAny >>= signedCallChain) {
    CORBA::Any_var callChainAny = _cdrCodec->decode_value(signedCallChain.encoded,
      idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain) return new CallerChain(callChain.target, callChain.originators, 
      callChain.caller, signedCallChain);
    else return 0;
  } else return 0;
}

ClientInterceptor::ClientInterceptor(
  PortableInterceptor::SlotId slotId_connectionAddr,
  PortableInterceptor::SlotId slotId_joinedCallChain,
  PortableInterceptor::SlotId slotId_ignoreInterceptor,
  IOP::Codec* cdr_codec)
  : _cdrCodec(cdr_codec), _manager(0), _slotId_connectionAddr(slotId_connectionAddr), 
    _slotId_joinedCallChain(slotId_joinedCallChain)
{ 
  log_scope l(log.general_logger(), info_level,
    "ClientInterceptor::ClientInterceptor");
  _slotId_ignoreInterceptor = slotId_ignoreInterceptor;
  _sessionLRUCache = std::auto_ptr<SessionLRUCache> (new SessionLRUCache(LOGINCACHE_LRU_SIZE));
  _callChainLRUCache = std::auto_ptr<CallChainLRUCache>(new CallChainLRUCache(LOGINCACHE_LRU_SIZE));
}

ClientInterceptor::~ClientInterceptor() { }

void ClientInterceptor::send_request(PortableInterceptor::ClientRequestInfo* r)
  throw (CORBA::Exception)
{
  const char* operation = r->operation();
  log_scope l(log.general_logger(), debug_level, "ClientInterceptor::send_request");
  l.level_vlog(debug_level, "operation: %s", operation);

  /* esta chamada remota precisa ser interceptada? */
  if (!IgnoreInterceptor::status(r)) {
    Connection& conn = getCurrentConnection(r);
    AutoLock conn_mutex(&conn._mutex);
    if (conn.login()) {
      conn_mutex.unlock();
      CallerChain* callerChain = 0;
      IOP::ServiceContext serviceContext;
      serviceContext.context_id = idl_cr::CredentialContextId;

      /* montando uma credencial com os dados(busid e login) da conexão. */
      idl_cr::CredentialData credential;
      conn_mutex.lock();
      credential.bus = CORBA::string_dup(conn.busid());
      credential.login = CORBA::string_dup(conn.login()->id);
      conn_mutex.unlock();
      
      /* adquirindo uma chave para a sessão que corresponde a esta requisição. */
      std::string sessionKey = getSessionKey(r);
    
      SecretSession* session;
      AutoLock m(&_mutex);
      bool b = _sessionLRUCache->fetch(sessionKey, session);
      m.unlock();
      if (b) {
        /* recuperando uma sessão para esta requisição. */
        credential.session = session->id;
        session->ticket++;
        credential.ticket = session->ticket;
        int slen = 22 + strlen(operation);
        unsigned char* s = new unsigned char[slen];
        s[0] = idl::MajorVersion;
        s[1] = idl::MinorVersion;
        memcpy(s+2, session->secret, 16);
        memcpy(s+18, &credential.ticket, 4);
        memcpy(s+22, operation, strlen(operation));
        SHA256(s, slen, credential.hash);
        
        callerChain = getJoinedChain(r);
        if (strcmp(idl::BusLogin, session->remoteid)) {
          /* esta requisição não é para o barramento, então preciso assinar essa cadeia. */
          /* montando uma hash para consultar o cache de cadeias assinadas. */
          idl::HashValue hash;
          conn_mutex.lock();
          CORBA::String_var connId = CORBA::string_dup(conn.login()->id);
          conn_mutex.unlock();
          size_t sid = strlen(connId);
          size_t sremoteid = strlen(session->remoteid);
          slen = sid + sremoteid + 256;
          s = new unsigned char[slen];
          memcpy(s, connId, sid);
          memcpy(s+sid, session->remoteid, sremoteid);
          if (callerChain) memcpy(s+sid+sremoteid, callerChain->signedCallChain()->signature, 256);
          else memset(s+sid+sremoteid, '\0', 256);
          SHA256(s, slen, hash);
          std::string shash((const char*) hash, 32);
          idl_cr::SignedCallChain signedCallChain;
          AutoLock m(&_mutex);
          if (_callChainLRUCache->exists(shash)) {
            signedCallChain = _callChainLRUCache->fetch(shash);
            l.level_vlog(debug_level,"Recuperando signedCallChain. remoteid: %s",session->remoteid);
            credential.chain = signedCallChain;
          } else {
            m.unlock();
            credential.chain = *conn.access_control()->signChainFor(session->remoteid);
            m.lock();
            _callChainLRUCache->insert(shash, credential.chain);
          }
        } else
          if (callerChain) credential.chain = *callerChain->signedCallChain();
          else memset(credential.chain.signature, '\0', 256);
      } else {
        /* montando uma credencial com o propósito de requisitar o estabelecimento de uma 
        ** nova sessão. */
        credential.ticket = 0;
        credential.session = 0;
        memset(credential.hash, '\0', 32);
        memset(credential.chain.signature, '\0', 256);
      }
      
      /* anexando a credencial a esta requisição. */
      CORBA::Any any;
      any <<= credential;
      CORBA::OctetSeq_var o = _cdrCodec->encode_value(any);
      IOP::ServiceContext::_context_data_seq s(o->length(), o->length(), o->get_buffer(), 0);
      serviceContext.context_data = s;
      r->add_request_service_context(serviceContext, true);
      
      /* anexando uma credencial legacy a esta requisição. */
      IOP::ServiceContext legacyContext;
      legacyContext.context_id = 1234;
      legacy::v1_5::Credential legacyCredential;
      conn_mutex.lock();
      legacyCredential.identifier = CORBA::string_dup(conn.login()->id);
      legacyCredential.owner = CORBA::string_dup(conn.login()->entity);
      conn_mutex.unlock();
      if (callerChain && (callerChain->_originators.length() > 1))
        legacyCredential.delegate = CORBA::string_dup(callerChain->_originators[0].entity);
      else legacyCredential.delegate = "";
      CORBA::Any lany;
      lany <<= legacyCredential;
      o = _cdrCodec->encode_value(lany);
      IOP::ServiceContext::_context_data_seq ls(o->length(), o->length(), o->get_buffer(), 0);
      legacyContext.context_data = ls;
      r->add_request_service_context(legacyContext, true);
    } else throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);          
  }
}

void ClientInterceptor::receive_exception(PortableInterceptor::ClientRequestInfo* r)
  throw (CORBA::Exception, PortableInterceptor::ForwardRequest)
{
  const char* operation = r->operation();
  log_scope l(log.general_logger(), debug_level, 
    "ClientInterceptor::receive_exception");
  l.level_vlog(debug_level, "operation: %s", operation); 
  l.level_vlog(debug_level, "exception: %s", r->received_exception_id()); 

  if (!strcmp(r->received_exception_id(), "IDL:omg.org/CORBA/NO_PERMISSION:1.0")) {
    CORBA::SystemException* ex = CORBA::SystemException::_decode(*r->received_exception());
    if (ex->completed() == CORBA::COMPLETED_NO) {
      l.level_vlog(debug_level, "minor: %d", ex->minor());
      Connection& conn = getCurrentConnection(r);
      l.level_vlog(debug_level, "minor: %d", ex->minor());
      if (ex->minor() == idl_ac::InvalidCredentialCode) {
        l.level_vlog(debug_level, "creating credential session");
        IOP::ServiceContext_var sctx;
        if (sctx = r->get_request_service_context(idl_cr::CredentialContextId)) {
          /* montando CredentialReset que foi enviado por quem está respondendo a um pedido de 
          ** inicialização de uma sessão. */
          CORBA::OctetSeq o(
            sctx->context_data.length(), 
            sctx->context_data.length(), 
            sctx->context_data.get_buffer());
          //[doubt] pegar exceção FormatMismatch ?
          CORBA::Any_var any = _cdrCodec->decode_value(o, idl_cr::_tc_CredentialReset);
          idl_cr::CredentialReset credentialReset;
          any >>= credentialReset;
        
          /* decifrar o segredo usando a chave do usuário. */
          unsigned char* secret = openssl::decrypt(conn.key(), credentialReset.challenge, 256);

          /* adquirindo uma chave para a sessão que corresponde a esta requisição. */
          std::string sessionKey = getSessionKey(r);
          
          /* criando uma sessão. */
          SecretSession* session = new SecretSession();
          session->id = credentialReset.session;
          session->remoteid  = CORBA::string_dup(credentialReset.login);
          session->secret = secret;
          session->ticket = 0;
          {
            AutoLock m(&_mutex);
            _sessionLRUCache->insert(sessionKey, session);
          }
          /* retransmitindo a requisição após ter estabelecido uma sessão. */
          throw PortableInterceptor::ForwardRequest(r->target(), false);
        }
      } else if (ex->minor() == idl_ac::NoCredentialCode) {
        throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, CORBA::COMPLETED_NO);
      } else if (ex->minor() == idl_ac::InvalidLoginCode) {
        AutoLock conn_mutex(&conn._mutex);
        idl_ac::LoginInfo oldLogin = *conn.login();
        const char* oldBusid = CORBA::string_dup(conn.busid());
        conn_mutex.unlock();
        conn._logout(true);
        conn_mutex.lock();
        Connection::InvalidLoginCallback_ptr callback = conn.onInvalidLogin();
        conn_mutex.unlock();
        if (callback) 
          if ((callback)(conn, oldLogin, oldBusid))
            throw PortableInterceptor::ForwardRequest(r->target(), false);            
      }
    }
  }
}

void ClientInterceptor::resetCaches() { 
  AutoLock m(&_mutex);
  _sessionLRUCache->clear();
  _callChainLRUCache->clear(); 
}

}
}
