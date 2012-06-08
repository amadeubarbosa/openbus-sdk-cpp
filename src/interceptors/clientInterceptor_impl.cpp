#include <interceptors/clientInterceptor_impl.h>
#include <legacy/stubs/credential_v1_05.h>
#include <util/openssl.h>
#include <log.h>

#include <openssl/sha.h>
#include <sstream>

namespace openbus {
namespace interceptors {    

/* monta uma indentificador(chave) para uma requisição através de uma hash do profile. */
std::string getSessionKey(PortableInterceptor::ClientRequestInfo* r) {
  idl::HashValue profileDataHash;
  ::IOP::TaggedProfile::_profile_data_seq profile = r->effective_profile()->profile_data;
  SHA256(profile.get_buffer(), profile.length(), profileDataHash);
  std::string sprofileDataHash((const char*) profileDataHash, 32);
  return sprofileDataHash;
}

Connection& ClientInterceptor::getCurrentConnection(PortableInterceptor::ClientRequestInfo* r) {
  log_scope l(log.client_interceptor_logger(), info_level, 
    "ClientInterceptor::getCurrentConnection");
  Connection* conn = 0;
  CORBA::Any_var connectionAddrAny;
  connectionAddrAny = r->get_slot(_slotId_connectionAddr);
  idl::OctetSeq connectionAddrOctetSeq;
  if (*connectionAddrAny >>= connectionAddrOctetSeq) {
    assert(connectionAddrOctetSeq.length() == sizeof(conn));
    std::memcpy(&conn, connectionAddrOctetSeq.get_buffer(), sizeof(conn));
  }
  if(!conn)
    if (!(conn = _manager->getDefaultConnection()))
      throw CORBA::NO_PERMISSION(idl_ac::NoLoginCode, CORBA::COMPLETED_NO);
  assert(conn != 0);
  l.vlog("connection:%p", conn);
  return *conn;
}

CallerChain* ClientInterceptor::getJoinedChain(PortableInterceptor::ClientRequestInfo* r) {
  CORBA::Any_var signedCallChainAny= r->get_slot(_slotId_joinedCallChain);
  idl_cr::SignedCallChain signedCallChain;
  if (*signedCallChainAny >>= signedCallChain) {
    CallerChain* c = new CallerChain();
    CORBA::Any_var callChainAny = _cdrCodec->decode_value(signedCallChain.encoded,
      idl_ac::_tc_CallChain);
    idl_ac::CallChain callChain;
    if (callChainAny >>= callChain) {
      c->signedCallChain(signedCallChain);
      c->_busid = callChain.target;
      c->_callers = callChain.callers;
      return c;
    } else return 0;
  } else return 0;
}

ClientInterceptor::ClientInterceptor(
  PortableInterceptor::SlotId slotId_connectionAddr,
  PortableInterceptor::SlotId slotId_joinedCallChain,
  IOP::Codec* cdr_codec) 
  : allowRequestWithoutCredential(false), _cdrCodec(cdr_codec), _manager(0),
  _slotId_connectionAddr(slotId_connectionAddr), _slotId_joinedCallChain(slotId_joinedCallChain) 
{ 
  log_scope l(log.client_interceptor_logger(), info_level,
    "ClientInterceptor::ClientInterceptor");
  _sessionLRUCache = std::auto_ptr<SessionLRUCache> (new SessionLRUCache(LOGINCACHE_LRU_SIZE));
  _callChainLRUCache = std::auto_ptr<CallChainLRUCache>(new CallChainLRUCache(LOGINCACHE_LRU_SIZE));
}

ClientInterceptor::~ClientInterceptor() { }

void ClientInterceptor::send_request(PortableInterceptor::ClientRequestInfo* r)
  throw (CORBA::Exception)
{
  const char* operation = r->operation();
  log_scope l(log.client_interceptor_logger(), debug_level, "ClientInterceptor::send_request");
  l.level_vlog(debug_level, "operation: %s", operation);

  /* esta chamada remota precisa ser interceptada? */
  if (!allowRequestWithoutCredential) {
    Connection& conn = getCurrentConnection(r);
    if (conn.login()) {
      CallerChain* callerChain = 0;
      IOP::ServiceContext serviceContext;
      serviceContext.context_id = idl_cr::CredentialContextId;

      /* montando uma credencial com os dados(busid e login) da conexão. */
      idl_cr::CredentialData credential;
      credential.bus = CORBA::string_dup(conn.busid());
      credential.login = CORBA::string_dup(conn.login()->id);
      
      /* adquirindo uma chave para a sessão que corresponde a esta requisição. */
      std::string sessionKey = getSessionKey(r);
    
      SecretSession* session;
      if (_sessionLRUCache->fetch(sessionKey, session)) {
        /* recuperando uma sessão para esta requisição. */
        credential.session = session->id;
        session->ticket++;
        credential.ticket = session->ticket;
        int slen = 22 + strlen(operation);
        unsigned char* s = new unsigned char[slen];
        s[0] = 2;
        s[1] = 0;
        memcpy(s+2, session->secret, 16);
        memcpy(s+18, &credential.ticket, 4);
        memcpy(s+22, operation, strlen(operation));
        SHA256(s, slen, credential.hash);
        
        callerChain = getJoinedChain(r);
        if (strcmp(conn.busid(), session->remoteid)) {
          /* esta requisição não é para o barramento, então preciso assinar essa cadeia. */
          /* montando uma hash para consultar o cache de cadeias assinadas. */
          idl::HashValue hash;
          const char* connId = conn.login()->id;
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
          if (_callChainLRUCache->exists(shash)) {
            signedCallChain = _callChainLRUCache->fetch(shash);
            l.level_vlog(debug_level,"Recuperando signedCallChain. remoteid: %s",session->remoteid);
            credential.chain = signedCallChain;
          } else {
            credential.chain = *conn.access_control()->signChainFor(session->remoteid);
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
      //[todo]
      IOP::ServiceContext legacyContext;
      legacyContext.context_id = 1234;
      legacy::v1_05::Credential legacyCredential;
      legacyCredential.identifier = conn.login()->id;
      legacyCredential.owner = conn.login()->entity;
      if (callerChain && (callerChain->_callers.length() > 1))
        legacyCredential.delegate = CORBA::string_dup(callerChain->_callers[0].entity);
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
  log_scope l(log.client_interceptor_logger(), debug_level, 
    "ClientInterceptor::receive_exception");
  l.level_vlog(debug_level, "receive_exception: %s", operation);

  if (!strcmp(r->received_exception_id(), "IDL:omg.org/CORBA/NO_PERMISSION:1.0")) {
    CORBA::SystemException* ex = CORBA::SystemException::_decode(*r->received_exception());
    if (ex->completed() == CORBA::COMPLETED_NO) {
      Connection& conn = getCurrentConnection(r);
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
          _sessionLRUCache->insert(sessionKey, session);
          /* retransmitindo a requisição após ter estabelecido uma sessão. */
          throw PortableInterceptor::ForwardRequest(r->target(), false);
        }
      } else if (ex->minor() == idl_ac::NoCredentialCode) {
        throw CORBA::NO_PERMISSION(idl_ac::InvalidRemoteCode, CORBA::COMPLETED_NO);
      } else if (ex->minor() == idl_ac::InvalidLoginCode) {
        if (conn.onInvalidLogin()) 
          if ((conn.onInvalidLogin())(&conn, conn.login()))
            throw PortableInterceptor::ForwardRequest(r->target(), false);            
        conn._logout(true);
      }
    }
  }
}

void ClientInterceptor::resetCaches() { 
  _sessionLRUCache->clear();
  _callChainLRUCache->clear(); 
}

}
}
