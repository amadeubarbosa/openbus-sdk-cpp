/*
** interceptors/ServerInterceptor.cpp
*/

#include <cstdio>
#include "interceptors/ServerInterceptor.h"
#include "openbus.h"

namespace openbus {
  namespace interceptors {
    unsigned long ServerInterceptor::validationTime = 30000; /* ms */
    set<access_control_service::Credential, ServerInterceptor::setCredentialCompare>::iterator ServerInterceptor::itCredentialsCache;
    set<access_control_service::Credential, ServerInterceptor::setCredentialCompare> 
      ServerInterceptor::credentialsCache;
  #ifdef OPENBUS_ORBIX
    ServerInterceptor::CredentialsValidationThread::CredentialsValidationThread() {
      Openbus::logger->log(INFO,
        "CredentialsValidationThread::CredentialsValidationThread() BEGIN");
      Openbus::logger->indent();
      Openbus::logger->dedent(INFO,
        "CredentialsValidationThread::CredentialsValidationThread() END");
    }

    ServerInterceptor::CredentialsValidationThread::~CredentialsValidationThread() {
      Openbus::logger->log(INFO,
        "CredentialsValidationThread::~CredentialsValidationThread() BEGIN");
      Openbus::logger->indent();
      Openbus::logger->dedent(INFO,
        "CredentialsValidationThread::~CredentialsValidationThread() END");
    }

    void* ServerInterceptor::CredentialsValidationThread::run() {
      Openbus::logger->log(INFO,
        "CredentialsValidationThread::run() BEGIN");
      Openbus::logger->indent();
      openbus::Openbus* bus = openbus::Openbus::getInstance();
      access_control_service::IAccessControlService* iAccessControlService = 
        bus->getAccessControlService();
      for (itCredentialsCache = credentialsCache.begin();
           itCredentialsCache != credentialsCache.end();
          )
      {
        stringstream out;
        out << "Validando a credencial: " << (const char*) ((*itCredentialsCache).identifier) 
          << " ...";
        Openbus::logger->log(INFO, out.str());
        try {
          if (iAccessControlService->isValid(*itCredentialsCache)) {
            Openbus::logger->log(INFO, "Credencial ainda � v�lida.");
          } else {
            Openbus::logger->log(WARNING, "Credencial N�O � mais v�lida!");
            credentialsCache.erase(itCredentialsCache++);
            continue;
          }
        } catch (CORBA::SystemException& e) {
          Openbus::logger->log(ERROR, "Erro ao verificar validade da credencial");
        }
        ++itCredentialsCache;
      }
      stringstream str;
      str << "Pr�xima valida��o em: " << validationTime << "ms" << endl;
      Openbus::logger->log(INFO, str.str());
      Openbus::logger->dedent(INFO, "CredentialsValidationThread::run() END");
      return 0;
    }
  #else
    ServerInterceptor::CredentialsValidationCallback::CredentialsValidationCallback() {
    };

    void ServerInterceptor::CredentialsValidationCallback::callback(
      CORBA::Dispatcher* dispatcher, 
      Event event)
    {
      Openbus::logger->log(INFO, 
        "ServerInterceptor::CredentialsValidationCallback() BEGIN");
    #ifndef _WIN32
      Openbus::logger->indent();
      openbus::Openbus* bus = openbus::Openbus::getInstance();
      access_control_service::IAccessControlService* iAccessControlService = 
        bus->getAccessControlService();
      for (itCredentialsCache = credentialsCache.begin();
           itCredentialsCache != credentialsCache.end(); 
          )
      {
        stringstream out;
        out << "Validando a credencial: " << 
          (const char*) ((*itCredentialsCache).identifier) << " ...";
        Openbus::logger->log(INFO, out.str());
        try {
          if (iAccessControlService->isValid(*itCredentialsCache)) {
            Openbus::logger->log(INFO, "Credencial ainda � v�lida.");
          } else {
            Openbus::logger->log(WARNING, "Credencial N�O � mais v�lida!");
            credentialsCache.erase(itCredentialsCache++);
            continue;
          }
        } catch (CORBA::SystemException& e) {
          Openbus::logger->log(ERROR, 
            "Erro ao verificar validade da credencial");
        }
        ++itCredentialsCache;
      }
      dispatcher->tm_event(this, validationTime);
      stringstream str;
      str << "Pr�xima valida��o em: " << validationTime << "ms" << endl;
      Openbus::logger->log(INFO, str.str());
    #endif
      Openbus::logger->dedent(INFO, 
        "ServerInterceptor::CredentialsValidationCallback() END");
    }
  #endif

    ServerInterceptor::ServerInterceptor(
      Current* ppicurrent,
      SlotId pslotid,
      IOP::Codec_ptr pcdr_codec)
    {
      Openbus::logger->log(INFO, "ServerInterceptor::ServerInterceptor() BEGIN");
      Openbus::logger->indent();
      slotid = pslotid;
      picurrent = ppicurrent;
      cdr_codec = pcdr_codec;
    #ifdef OPENBUS_ORBIX
      credentialsValidationTimer = 0;
    #endif
      Openbus::logger->dedent(INFO, "ServerInterceptor::ServerInterceptor() END");
    }

    ServerInterceptor::~ServerInterceptor() {
    #ifdef OPENBUS_ORBIX
      if (credentialsValidationTimer) {
        credentialsValidationTimer->stop();
        delete credentialsValidationThread;
        delete credentialsValidationTimer;
      }
    #else
      openbus::Openbus* bus = openbus::Openbus::getInstance();
      bus->getORB()->dispatcher()->remove(&credentialsValidationCallback, 
        CORBA::Dispatcher::Timer);
    #endif
    }

    void ServerInterceptor::receive_request(ServerRequestInfo_ptr ri) {
      ::IOP::ServiceContext_var sc = ri->get_request_service_context(1234);
      Openbus::logger->log(INFO, "ServerInterceptor::receive_request() BEGIN");
      Openbus::logger->indent();
      stringstream request;
      char* repID = ri->target_most_derived_interface();
      char* operation = ri->operation();
      request << "RepID: " << repID;
      Openbus::logger->log(INFO, request.str());
      request.str(" ");
      request << "M�todo: " << operation;
      Openbus::logger->log(INFO, request.str());
      
      openbus::Openbus* bus = openbus::Openbus::getInstance();
      if (!bus->isInterceptable(repID, operation)) {
        free(repID);
        free(operation);
        Openbus::logger->log(WARNING, "Este m�todo n�o ser� interceptado.");
        Openbus::logger->dedent(INFO, "ServerInterceptor::receive_request() END");
      } else {
        CORBA::ULong z;
        stringstream contextData;
        contextData << "Context Data: ";
        for (z = 0; z < sc->context_data.length(); z++) {
          contextData << (unsigned) sc->context_data[z];
        }
        Openbus::logger->log(INFO, contextData.str());

        IOP::ServiceContext::_context_data_seq& context_data = sc->context_data;

        CORBA::OctetSeq octets(context_data.length(),
          context_data.length(),
          context_data.get_buffer(),
          0);

        CORBA::Any_var any = cdr_codec->decode_value(
          octets, 
          access_control_service::_tc_Credential);
      #ifdef OPENBUS_ORBIX
        access_control_service::Credential* c;
      #else
        access_control_service::Credential c;
      #endif
        any >>= c;
      #ifdef OPENBUS_ORBIX
        Openbus::logger->log(INFO, "credential->owner: " + (string) c->owner);
        Openbus::logger->log(INFO, "credential->identifier: " + (string) c->identifier);
        Openbus::logger->log(INFO, "credential->delegate: " + (string) c->delegate);
      #else
        Openbus::logger->log(INFO, "credential->owner: " + (string) c.owner);
        Openbus::logger->log(INFO, "credential->identifier: " + (string) c.identifier);
        Openbus::logger->log(INFO, "credential->delegate: " + (string) c.delegate);
      #endif
        CredentialValidationPolicy policy = bus->getCredentialValidationPolicy(); 
        picurrent->set_slot(slotid, any);
        if (policy == ALWAYS) {
          Openbus::logger->log(INFO, "Pol�tica de renova��o de credenciais: " + 
            (string) + "ALWAYS");
          Openbus::logger->log(INFO, "Validando credencial REMOTAMENTE...");
          try {
          #ifdef OPENBUS_ORBIX
            if (bus->getAccessControlService()->isValid(*c)) {
          #else
            if (bus->getAccessControlService()->isValid(c)) {
          #endif
              Openbus::logger->log(INFO, "Credencial validada.");
              Openbus::logger->dedent(INFO, "ServerInterceptor::receive_request() END");
            } else {
              Openbus::logger->log(INFO, "Falha na valida��o da credencial.");
              Openbus::logger->log(ERROR, "Throwing CORBA::NO_PERMISSION...");
              Openbus::logger->dedent(INFO, "ServerInterceptor::receive_request() END");
              throw CORBA::NO_PERMISSION();
            }
          } catch (CORBA::SystemException& e) {
            Openbus::logger->log(INFO, "Falha na renova��o da credencial.");
            Openbus::logger->log(ERROR, "Throwing CORBA::SystemException");
            Openbus::logger->dedent(INFO, "ServerInterceptor::receive_request() END");
            throw;
          }
        } else if (policy == CACHED) {
          Openbus::logger->log(INFO, "Pol�tica de renova��o de credenciais: " + 
            (string) "CACHED");
          stringstream out;
          out << "N�mero de credenciais no cache: " << credentialsCache.size();
          Openbus::logger->log(INFO, out.str());
        #ifdef OPENBUS_ORBIX
          if (credentialsCache.find(*c) != credentialsCache.end()
              &&
              !credentialsCache.empty()) 
          {  
        #else
          if (credentialsCache.find(c) != credentialsCache.end()
              &&
              !credentialsCache.empty()) 
          {  
        #endif
            Openbus::logger->log(INFO, "Credencial est� no cache.");
            Openbus::logger->dedent(INFO, 
              "ServerInterceptor::receive_request() END");
          } else {
            Openbus::logger->log(INFO, "Credencial N�O est� no cache.");
            Openbus::logger->log(INFO, "Validando credencial no ACS...");
            try {
            #ifdef OPENBUS_ORBIX
              if (bus->getAccessControlService()->isValid(*c)) {
            #else
              if (bus->getAccessControlService()->isValid(c)) {
            #endif
                Openbus::logger->log(INFO, "Credencial validada.");
              } else {
                Openbus::logger->log(INFO, "Falha na valida��o da credencial.");
                Openbus::logger->log(ERROR, "Throwing CORBA::NO_PERMISSION...");
                Openbus::logger->dedent(INFO, "ServerInterceptor::receive_request() END");
                throw CORBA::NO_PERMISSION();
              }
              Openbus::logger->log(INFO, "Inserindo credencial no cache...");
            #ifdef OPENBUS_ORBIX
              credentialsCache.insert(*c);
            #else
              credentialsCache.insert(c);
            #endif  
              Openbus::logger->dedent(INFO, "ServerInterceptor::receive_request() END");
            } catch (CORBA::SystemException& e) {
              Openbus::logger->log(INFO, "Falha na valida��o da credencial.");
              Openbus::logger->log(ERROR, "Throwing CORBA::SystemException");
              Openbus::logger->dedent(INFO, "ServerInterceptor::receive_request() END");
              throw;
            }
          }
        } else {
          Openbus::logger->log(INFO, "Pol�tica de renova��o de credenciais: " + (string) "NONE");
          Openbus::logger->dedent(INFO, "ServerInterceptor::receive_request() END");
        }
      }
    }          
      
    void ServerInterceptor::receive_request_service_contexts(
      ServerRequestInfo*) {
    }

    void ServerInterceptor::send_reply(ServerRequestInfo*) {}

    void ServerInterceptor::send_exception(ServerRequestInfo*) {}

    void ServerInterceptor::send_other(ServerRequestInfo*) {}

    char* ServerInterceptor::name() {
      return CORBA::string_dup("AccessControl");
    }

    void ServerInterceptor::destroy() {}

    access_control_service::Credential_var ServerInterceptor::getCredential() {
      Openbus::logger->log(INFO, "ServerInterceptor::getCredential() BEGIN");
      Openbus::logger->indent();
      CORBA::Any_var any = picurrent->get_slot(slotid);

    #ifdef OPENBUS_ORBIX
      access_control_service::Credential* c = 0;
      any >>= c;
      if (c) {
        Openbus::logger->log(INFO, "credential->owner: " + (string) c->owner);
        Openbus::logger->log(INFO, "credential->identifier: " + (string) c->identifier);
        Openbus::logger->log(INFO, "credential->delegate: " + (string) c->delegate);
        access_control_service::Credential_var ret = new access_control_service::Credential();
        ret->owner = CORBA::string_dup(c->owner);
        ret->identifier = CORBA::string_dup(c->identifier);
        ret->delegate = CORBA::string_dup(c->delegate);
    #else
      access_control_service::Credential c;
      if (any >>=c) {
        Openbus::logger->log(INFO, "credential->owner: " + (string) c.owner);
        Openbus::logger->log(INFO, "credential->identifier: " + (string) c.identifier);
        Openbus::logger->log(INFO, "credential->delegate: " + (string) c.delegate);
        access_control_service::Credential_var ret = new access_control_service::Credential();
        ret->owner = CORBA::string_dup(c.owner);
        ret->identifier = CORBA::string_dup(c.identifier);
        ret->delegate = CORBA::string_dup(c.delegate);
    #endif
        Openbus::logger->dedent(INFO, "ServerInterceptor::getCredential() END");
        return ret._retn();
      } else {
        Openbus::logger->log(WARNING, "Nao foi possivel adquirir uma credencial. " + 
          (string) + "Verifique se esta chamada esta sendo realizada dentro de um metodo " + 
          (string) + "disponibilizado remotamente.");
        Openbus::logger->dedent(INFO, "ServerInterceptor::getCredential() END");
        return 0;
      }
    }

  #ifdef OPENBUS_ORBIX
    void ServerInterceptor::registerValidationTimer() {
      IT_Duration start(0);
      IT_Duration interval(validationTime/1000, 0);
      credentialsValidationThread = new CredentialsValidationThread();
      credentialsValidationTimer = new IT_Timer(
        start, 
        interval, 
        *credentialsValidationThread, 
        true);
    }
  #else
    void ServerInterceptor::registerValidationDispatcher() {
      openbus::Openbus* bus = openbus::Openbus::getInstance();
      bus->getORB()->dispatcher()->tm_event(&credentialsValidationCallback, 
        validationTime);
    }
  #endif

    void ServerInterceptor::setValidationTime(unsigned long pValidationTime) {
      validationTime = pValidationTime;
    }

    unsigned long ServerInterceptor::getValidationTime() {
      return validationTime;
    }
  }
}
