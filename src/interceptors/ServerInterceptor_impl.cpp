#include <iostream>
#include <openssl/sha.h>
#include <openssl/x509.h>

#include "openbus/interceptors/ServerInterceptor_impl.h"
#include "stubs/credential_v1_5.h"
#include "openbus/util/OpenSSL.h"
#include "openbus/log.h"
#include "openbus/util/AutoLock_impl.h"

namespace openbus {
namespace interceptors {

Session::Session(CORBA::ULong i, const char * login) 
  : id(i), remoteId(CORBA::string_dup(login))
{
  tickets_init(&tickets);
  for (short i=0;i<SECRET_SIZE;++i) secret[i] = rand()%255;
}

ServerInterceptor::ServerInterceptor(
  PortableInterceptor::Current *piCurrent, 
  PortableInterceptor::SlotId slotId_requesterConnection,
  PortableInterceptor::SlotId slotId_receiveConnection,
  PortableInterceptor::SlotId slotId_joinedCallChain,
  PortableInterceptor::SlotId slotId_signedCallChain, 
  PortableInterceptor::SlotId slotId_legacyCallChain,
  IOP::Codec *cdr_codec) 
  : _piCurrent(piCurrent), _slotId_requesterConnection(slotId_requesterConnection),
    _slotId_receiveConnection(slotId_receiveConnection),
    _slotId_joinedCallChain(slotId_joinedCallChain),_slotId_signedCallChain(slotId_signedCallChain),
    _slotId_legacyCallChain(slotId_legacyCallChain), _cdrCodec(cdr_codec), _openbusContext(0),
    _sessionLRUCache(SessionLRUCache(LOGINCACHE_LRU_SIZE))
{
  log_scope l(log.general_logger(), debug_level,"ServerInterceptor::ServerInterceptor");
}

void ServerInterceptor::sendCredentialReset(Connection *conn, Login *caller, 
  PortableInterceptor::ServerRequestInfo *r) 
{
  idl_cr::CredentialReset credentialReset;

  AutoLock m(&_mutex);
  Session session(_sessionLRUCache.size()+1, caller->loginInfo->id); 
  _sessionLRUCache.insert(session.id, session);
  credentialReset.session = session.id;

  /* cifrando o segredo com a chave pública do cliente. */
  CORBA::OctetSeq encrypted = openssl::encrypt(caller->key, session.secret, SECRET_SIZE); 
  m.unlock();

  AutoLock conn_mutex(&conn->_mutex);
  credentialReset.login = conn->_login()->id;
  conn_mutex.unlock();
  memcpy(credentialReset.challenge, encrypted.get_buffer(), idl::EncryptedBlockSize);

  CORBA::Any any;
  any <<= credentialReset;
  CORBA::OctetSeq_var o = _cdrCodec->encode_value(any);

  /* anexando CredentialReset a resposta para o cliente. */
  IOP::ServiceContext serviceContext;
  serviceContext.context_id = idl_cr::CredentialContextId;
  IOP::ServiceContext::_context_data_seq s(o->length(), o->length(), o->get_buffer(), 0);
  serviceContext.context_data = s;
  r->add_reply_service_context(serviceContext, true);          

  throw CORBA::NO_PERMISSION(idl_ac::InvalidCredentialCode, CORBA::COMPLETED_NO);              
}

ServerInterceptor::~ServerInterceptor() { }

Connection * ServerInterceptor::getDispatcher(OpenBusContext &context, const char *busId, 
                                              const char *loginId, const char *operation)
{
  Connection *conn = 0;
  log_scope l(log.general_logger(), debug_level,"ServerInterceptor::getDispatcher");
  try {
    if (context.onCallDispatch()) {
      conn = context.onCallDispatch()(context, busId, loginId, operation);
    }
  } catch (...) {
    // [TODO] mais detalhes?
    l.level_log(warning_level, "Falha na execucao da callback CallDispatch.");
  }
  if (conn) {
    if (!conn->login() || (strcmp(conn->busid(), busId))) {
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    } 
  } else {
    if (!(conn = context.getDefaultConnection())) {
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    }
  }
  return conn;
}

void ServerInterceptor::receive_request_service_contexts(PortableInterceptor::ServerRequestInfo *r)
{
  const char *operation = r->operation();
  log_scope l(log.general_logger(), debug_level,
    "ServerInterceptor::receive_request_service_contexts");
  l.level_vlog(debug_level, "operation: %s", operation);
  
  /* extraindo a credencial desta requisição. */
  CORBA::Any_var any;
  bool hasContext = true;
  try {
    IOP::ServiceContext_var sc = r->get_request_service_context(idl_cr::CredentialContextId);
    IOP::ServiceContext::_context_data_seq &cd = sc->context_data;
    CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);    
    any = _cdrCodec->decode_value(contextData, idl_cr::_tc_CredentialData);
  } catch (CORBA::BAD_PARAM&) {
    hasContext = false;
  }
  idl_cr::CredentialData credential;
  if (hasContext && (any >>= credential)) {
    Connection *conn = getDispatcher(*_openbusContext, credential.bus, credential.login, operation);

    /* disponibilizando a conexão atual para OpenBusContext::getRequester() */
    size_t bufSize = sizeof(Connection*);
    unsigned char buf[bufSize];
    memcpy(buf, &conn, bufSize);
    idl::OctetSeq_var connectionAddrOctetSeq = new idl::OctetSeq(bufSize, bufSize, buf);
    CORBA::Any connectionAddrAny;
    connectionAddrAny <<= *(connectionAddrOctetSeq);
    r->set_slot(_slotId_requesterConnection, connectionAddrAny);

    /* Disponibilização da conexão que está recebendo esta chamada. 
    ** Uso em Connection::getCallerChain() */
    r->set_slot(_slotId_receiveConnection, connectionAddrAny);

    /* definindo a conexão atual como a conexão a ser utilizada pelas chamadas remotas a serem 
    ** realizadas por este ponto de interceptação */
    _openbusContext->setRequester(conn);

    AutoLock conn_mutex(&conn->_mutex);
    if (!conn->_login())
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    conn_mutex.unlock();

    Login *caller;
    /* consulta ao cache de logins para saber se este login é valido. 
    ** obtenção da estrutura Login referente a este login id. (caller) */
    if (strcmp(credential.bus.in(), conn->_busid)) {
      l.log("Login diferente daquele que iniciou a sessão.");
      throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    }
    try {
      l.vlog("Validando login: %s", credential.login.in());
      caller = conn->_loginCache->validateLogin(credential.login);
    } catch (CORBA::NO_PERMISSION &e) {
      if (e.minor() == idl_ac::NoLoginCode) 
        throw CORBA::NO_PERMISSION(idl_ac::UnknownBusCode, CORBA::COMPLETED_NO);
    } catch (CORBA::Exception&) {
      /* não há uma entrada válida no cache e o cache nõo consegui validar o login 
      ** com o barramento. */
      throw CORBA::NO_PERMISSION(idl_ac::UnverifiedLoginCode, CORBA::COMPLETED_NO);
    }
    if (!caller) {
      l.log("Lancando excecao InvalidLoginCode.");
      throw CORBA::NO_PERMISSION(idl_ac::InvalidLoginCode, CORBA::COMPLETED_NO);
    }
    l.log("Login valido.");
    idl::HashValue hash;   
    Session *session = 0;

    AutoLock m(&_mutex);
    bool hasSession = _sessionLRUCache.exists(credential.session);
    if (hasSession) session = _sessionLRUCache.fetch_ptr(credential.session);
    m.unlock();

    tickets_History *t = 0;
    const char *remoteId = 0;
    if (hasSession) {
      /* montando uma hash com os dados da credencial recebida e da sessão existente. */
      size_t operationSize = strlen(r->operation());
      int bufSize = 22 + operationSize;
      std::auto_ptr<unsigned char> buf(new unsigned char[bufSize]());
      unsigned char *pBuf = buf.get();
      pBuf[0] = idl::MajorVersion;
      pBuf[1] = idl::MinorVersion;
      m.lock();
      memcpy(pBuf+2, session->secret, SECRET_SIZE);
      m.unlock();
      memcpy(pBuf+18, &credential.ticket, 4);
      memcpy(pBuf+22, r->operation(), operationSize);
      SHA256(pBuf, bufSize, hash);
      m.lock();
      t = &session->tickets;
      remoteId = session->remoteId;
      m.unlock();
    }

    if (!(hasSession && !memcmp(hash, credential.hash, idl::HashValueSize) && 
          !strcmp(remoteId, credential.login.in()) && 
          tickets_check(t, credential.ticket))) 
    {
      l.level_vlog(debug_level, "credential not valid, try to reset credetial session");
      sendCredentialReset(conn, caller, r);
    }

    /* a credencial recebida é válida. */
    l.level_vlog(debug_level, "credential is valid");

    if (!credential.chain.encoded.length())
      throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);

    idl::HashValue hashChain;
    SHA256(credential.chain.encoded.get_buffer(), credential.chain.encoded.length(), hashChain);

    openssl::pkey_ctx ctx (EVP_PKEY_CTX_new(conn->__buskey().get(), 0));
    if(!ctx) {
      l.level_log(info_level, "Failed creating a OpenSSL public key context (EVP_PKEY_CTX_new)");
      return;
    }

    int status = EVP_PKEY_verify_init(ctx.get());
    assert(status);
    if (EVP_PKEY_verify(ctx.get(), credential.chain.signature, idl::EncryptedBlockSize, hashChain, 
                        idl::HashValueSize) != 1)
      throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
    else {
      CORBA::Any_var callChainAny = _cdrCodec->decode_value(credential.chain.encoded, 
                                                            idl_ac::_tc_CallChain);
      idl_ac::CallChain callChain;
      callChainAny >>= callChain;
      conn_mutex.lock();
      int res = strcmp(callChain.target, conn->_login()->id);
      conn_mutex.unlock();
      if (res) { 
        /* a cadeia tem como destino(target) outro login. */
        sendCredentialReset(conn, caller, r);
      } else if(strcmp(callChain.caller.id, caller->loginInfo->id)) {
        /* o último elemento da cadeia não é quem está me chamando. */
        throw CORBA::NO_PERMISSION(idl_ac::InvalidChainCode, CORBA::COMPLETED_NO);
      } else {
        CORBA::Any signedCallChainAny;
        signedCallChainAny <<= credential.chain;
        r->set_slot(_slotId_signedCallChain, signedCallChainAny);
      }
    }
  } else {
    l.level_vlog(debug_level, "verificando se existe uma credencial legacy");
    hasContext = true;
    CORBA::Any_var lany;
    try {
      IOP::ServiceContext_var sc = r->get_request_service_context(1234);
      IOP::ServiceContext::_context_data_seq &cd = sc->context_data;
      CORBA::OctetSeq contextData(cd.length(), cd.length(), cd.get_buffer(), 0);    
      lany = _cdrCodec->decode_value(contextData, openbus::legacy::v1_5::_tc_Credential);
    } catch (CORBA::BAD_PARAM&) {
      hasContext = false;
    }
    openbus::legacy::v1_5::Credential legacyCredential;
    if (hasContext && (lany >>= legacyCredential)) {
      l.level_vlog(info_level, "extraindo credencial legacy");
      l.level_vlog(debug_level, "credential.identifier: %s", legacyCredential.identifier.in());
      l.level_vlog(debug_level, "credential.owner: %s", legacyCredential.owner.in());
      l.level_vlog(debug_level, "credential.delegate: %s", legacyCredential.delegate.in());
      idl_ac::CallChain legacyChain;
      legacyChain.target = "";
      if (strcmp(legacyCredential.delegate, "")) {
        legacyChain.originators.length(1);
        idl_ac::LoginInfo delegate;
        delegate.id = "<unknown>";
        delegate.entity = legacyCredential.delegate;
        legacyChain.originators[0] = delegate;
        idl_ac::LoginInfo login;
        login.id = legacyCredential.identifier;
        login.entity = legacyCredential.owner;
        legacyChain.caller = login;
      } else {
        legacyChain.originators.length(0);
        idl_ac::LoginInfo loginInfo;
        loginInfo.id = legacyCredential.identifier;
        loginInfo.entity = legacyCredential.owner;
        legacyChain.caller = loginInfo;            
      }
      CORBA::Any legacyChainAny;
      legacyChainAny <<= legacyChain;
      r->set_slot(_slotId_legacyCallChain, legacyChainAny);
    } else throw CORBA::NO_PERMISSION(idl_ac::NoCredentialCode, CORBA::COMPLETED_NO);
  }
}

}
}
