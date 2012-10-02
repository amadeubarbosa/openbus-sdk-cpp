/*
** interceptors/ServerInterceptor.cpp
*/

#include <cstdio>
#include "interceptors/ServerInterceptor.h"
#include "openbus.h"

namespace openbus {
  namespace interceptors {
    unsigned long ServerInterceptor::validationTime = 30000; /* ms */
    std::set<tecgraf::openbus::core::v1_05::access_control_service::Credential, ServerInterceptor::setCredentialCompare>::iterator ServerInterceptor::itCredentialsCache;
    std::set<tecgraf::openbus::core::v1_05::access_control_service::Credential, ServerInterceptor::setCredentialCompare> 
      ServerInterceptor::credentialsCache;
  #ifdef OPENBUS_ORBIX
    ServerInterceptor::CredentialsValidationThread::CredentialsValidationThread() {
      Openbus::logger->log(logger::INFO,
        "CredentialsValidationThread::CredentialsValidationThread() BEGIN");
      Openbus::logger->indent();
      Openbus::logger->dedent(logger::INFO,
        "CredentialsValidationThread::CredentialsValidationThread() END");
    }

    ServerInterceptor::CredentialsValidationThread::~CredentialsValidationThread() {
      Openbus::logger->log(logger::INFO,
        "CredentialsValidationThread::~CredentialsValidationThread() BEGIN");
      Openbus::logger->indent();
      Openbus::logger->dedent(logger::INFO,
        "CredentialsValidationThread::~CredentialsValidationThread() END");
    }

    void* ServerInterceptor::CredentialsValidationThread::run() {
      Openbus::logger->log(logger::INFO,
        "CredentialsValidationThread::run() BEGIN");
      Openbus::logger->indent();
      openbus::Openbus* bus = openbus::Openbus::getInstance();
      access_control_service::IAccessControlService* iAccessControlService = 
        bus->getAccessControlService();
      for (itCredentialsCache = credentialsCache.begin();
           itCredentialsCache != credentialsCache.end();
          )
      {
        std::stringstream out;
        out << "Validando a credencial: " << (const char*) ((*itCredentialsCache).identifier) 
          << " ...";
        Openbus::logger->log(logger::INFO, out.str());
        try {
          if (iAccessControlService->isValid(*itCredentialsCache)) {
            Openbus::logger->log(logger::INFO, "Credencial ainda � v�lida.");
          } else {
            Openbus::logger->log(logger::WARNING, "Credencial N�O � mais v�lida!");
            credentialsCache.erase(itCredentialsCache++);
            continue;
          }
        } catch (CORBA::SystemException& e) {
          Openbus::logger->log(logger::ERROR, "Erro ao verificar validade da credencial");
        }
        ++itCredentialsCache;
      }
      std::stringstream str;
      str << "Pr�xima valida��o em: " << validationTime << "ms" << endl;
      Openbus::logger->log(logger::INFO, str.str());
      Openbus::logger->dedent(logger::INFO, "CredentialsValidationThread::run() END");
      return 0;
    }
  #else
    ServerInterceptor::CredentialsValidationCallback::CredentialsValidationCallback() {
    };

    void ServerInterceptor::CredentialsValidationCallback::callback(
      CORBA::Dispatcher* dispatcher, 
      Event event)
    {
      Openbus::logger->log(logger::INFO, 
        "ServerInterceptor::CredentialsValidationCallback() BEGIN");
    #ifndef _WIN32
      Openbus::logger->indent();
      openbus::Openbus* bus = openbus::Openbus::getInstance();
      tecgraf::openbus::core::v1_05::access_control_service::IAccessControlService* iAccessControlService = 
        bus->getAccessControlService();
      for (itCredentialsCache = credentialsCache.begin();
           itCredentialsCache != credentialsCache.end(); 
          )
      {
        std::stringstream out;
        out << "Validando a credencial: " << 
          (const char*) ((*itCredentialsCache).identifier) << " ...";
        Openbus::logger->log(logger::INFO, out.str());
        try {
          if (iAccessControlService->isValid(*itCredentialsCache)) {
            Openbus::logger->log(logger::INFO, "Credencial ainda � v�lida.");
          } else {
            Openbus::logger->log(logger::WARNING, "Credencial N�O � mais v�lida!");
            credentialsCache.erase(itCredentialsCache++);
            continue;
          }
        } catch (CORBA::SystemException& e) {
          Openbus::logger->log(logger::ERROR, 
            "Erro ao verificar validade da credencial");
        }
        ++itCredentialsCache;
      }
      dispatcher->tm_event(this, validationTime);
      std::stringstream str;
      str << "Pr�xima valida��o em: " << validationTime << "ms" << std::endl;
      Openbus::logger->log(logger::INFO, str.str());
    #endif
      Openbus::logger->dedent(logger::INFO, 
        "ServerInterceptor::CredentialsValidationCallback() END");
    }
  #endif

    ServerInterceptor::ServerInterceptor(
                                         PortableInterceptor::Current* ppicurrent,
                                         PortableInterceptor::SlotId pslotid,
      IOP::Codec_ptr pcdr_codec)
    {
      Openbus::logger->log(logger::INFO, "ServerInterceptor::ServerInterceptor() BEGIN");
      Openbus::logger->indent();
      slotid = pslotid;
      picurrent = ppicurrent;
      cdr_codec = pcdr_codec;
    #ifdef OPENBUS_ORBIX
      credentialsValidationTimer = 0;
    #endif
      Openbus::logger->dedent(logger::INFO, "ServerInterceptor::ServerInterceptor() END");
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

    void ServerInterceptor::receive_request(PortableInterceptor::ServerRequestInfo_ptr ri) {
      ::IOP::ServiceContext_var sc = ri->get_request_service_context(1234);
      Openbus::logger->log(logger::INFO, "ServerInterceptor::receive_request() BEGIN");
      Openbus::logger->indent();
      std::stringstream request;
      char* repID = ri->target_most_derived_interface();
      char* operation = ri->operation();
      request << "RepID: " << repID;
      Openbus::logger->log(logger::INFO, request.str());
      request.str(" ");
      request << "M�todo: " << operation;
      Openbus::logger->log(logger::INFO, request.str());
      
      openbus::Openbus* bus = openbus::Openbus::getInstance();
      if (!bus->isInterceptable(repID, operation) || !strcmp(operation, "_is_a") 
          || !strcmp(operation, "_interface") || !strcmp(operation, "_non_existent"))
      {
        Openbus::logger->log(logger::WARNING, "Este m�todo n�o ser� interceptado.");
        Openbus::logger->dedent(logger::INFO, "ServerInterceptor::receive_request() END");
      } else {
        CORBA::ULong z;
        std::stringstream contextData;
        contextData << "Context Data: ";
        for (z = 0; z < sc->context_data.length(); z++) {
          contextData << (unsigned) sc->context_data[z];
        }
        Openbus::logger->log(logger::INFO, contextData.str());

        IOP::ServiceContext::_context_data_seq& context_data = sc->context_data;

        CORBA::OctetSeq octets(context_data.length(),
          context_data.length(),
          context_data.get_buffer(),
          0);

        CORBA::Any_var any = cdr_codec->decode_value(
          octets, 
          tecgraf::openbus::core::v1_05::access_control_service::_tc_Credential);
      #ifdef OPENBUS_ORBIX
        tecgraf::openbus::core::v1_05::access_control_service::Credential* c;
      #else
        tecgraf::openbus::core::v1_05::access_control_service::Credential c;
      #endif
        any >>= c;
      #ifdef OPENBUS_ORBIX
        Openbus::logger->log(logger::INFO, "credential->owner: " + (std::string) c->owner);
        Openbus::logger->log(logger::INFO, "credential->identifier: " + (std::string) c->identifier);
        Openbus::logger->log(logger::INFO, "credential->delegate: " + (std::string) c->delegate);
      #else
        Openbus::logger->log(logger::INFO, "credential->owner: " + (std::string) c.owner);
        Openbus::logger->log(logger::INFO, "credential->identifier: " + (std::string) c.identifier);
        Openbus::logger->log(logger::INFO, "credential->delegate: " + (std::string) c.delegate);
      #endif
        CredentialValidationPolicy policy = bus->getCredentialValidationPolicy(); 
        picurrent->set_slot(slotid, any);
        if (policy == ALWAYS) {
          Openbus::logger->log(logger::INFO, "Pol�tica de renova��o de credenciais: " + 
                               (std::string) + "ALWAYS");
          Openbus::logger->log(logger::INFO, "Validando credencial REMOTAMENTE...");
          try {
          #ifdef OPENBUS_ORBIX
            if (bus->getAccessControlService()->isValid(*c)) {
          #else
            if (bus->getAccessControlService()->isValid(c)) {
          #endif
              Openbus::logger->log(logger::INFO, "Credencial validada.");
              Openbus::logger->dedent(logger::INFO, "ServerInterceptor::receive_request() END");
            } else {
              Openbus::logger->log(logger::INFO, "Falha na valida��o da credencial.");
              Openbus::logger->log(logger::ERROR, "Throwing CORBA::NO_PERMISSION...");
              Openbus::logger->dedent(logger::INFO, "ServerInterceptor::receive_request() END");
              throw CORBA::NO_PERMISSION();
            }
          } catch (CORBA::SystemException& e) {
            Openbus::logger->log(logger::INFO, "Falha na renova��o da credencial.");
            Openbus::logger->log(logger::ERROR, "Throwing CORBA::SystemException");
            Openbus::logger->dedent(logger::INFO, "ServerInterceptor::receive_request() END");
            throw;
          }
        } else if (policy == CACHED) {
          Openbus::logger->log(logger::INFO, "Pol�tica de renova��o de credenciais: " + 
                               (std::string) "CACHED");
          std::stringstream out;
          out << "N�mero de credenciais no cache: " << credentialsCache.size();
          Openbus::logger->log(logger::INFO, out.str());
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
            Openbus::logger->log(logger::INFO, "Credencial est� no cache.");
            Openbus::logger->dedent(logger::INFO, 
              "ServerInterceptor::receive_request() END");
          } else {
            Openbus::logger->log(logger::INFO, "Credencial N�O est� no cache.");
            Openbus::logger->log(logger::INFO, "Validando credencial no ACS...");
            try {
            #ifdef OPENBUS_ORBIX
              if (bus->getAccessControlService()->isValid(*c)) {
            #else
              if (bus->getAccessControlService()->isValid(c)) {
            #endif
                Openbus::logger->log(logger::INFO, "Credencial validada.");
              } else {
                Openbus::logger->log(logger::INFO, "Falha na valida��o da credencial.");
                Openbus::logger->log(logger::ERROR, "Throwing CORBA::NO_PERMISSION...");
                Openbus::logger->dedent(logger::INFO, "ServerInterceptor::receive_request() END");
                throw CORBA::NO_PERMISSION();
              }
              Openbus::logger->log(logger::INFO, "Inserindo credencial no cache...");
            #ifdef OPENBUS_ORBIX
              credentialsCache.insert(*c);
            #else
              credentialsCache.insert(c);
            #endif  
              Openbus::logger->dedent(logger::INFO, "ServerInterceptor::receive_request() END");
            } catch (CORBA::SystemException& e) {
              Openbus::logger->log(logger::INFO, "Falha na valida��o da credencial.");
              Openbus::logger->log(logger::ERROR, "Throwing CORBA::SystemException");
              Openbus::logger->dedent(logger::INFO, "ServerInterceptor::receive_request() END");
              throw;
            }
          }
        } else {
            Openbus::logger->log(logger::INFO, "Pol�tica de renova��o de credenciais: " + (std::string) "NONE");
          Openbus::logger->dedent(logger::INFO, "ServerInterceptor::receive_request() END");
        }
      }
    }          
      
    void ServerInterceptor::receive_request_service_contexts(
                                                             PortableInterceptor::ServerRequestInfo*) {
    }

    void ServerInterceptor::send_reply(PortableInterceptor::ServerRequestInfo*) {}

    void ServerInterceptor::send_exception(PortableInterceptor::ServerRequestInfo*) {}

    void ServerInterceptor::send_other(PortableInterceptor::ServerRequestInfo*) {}

    char* ServerInterceptor::name() {
      return CORBA::string_dup("AccessControl");
    }

    void ServerInterceptor::destroy() {}

    tecgraf::openbus::core::v1_05::access_control_service::Credential_var ServerInterceptor::getCredential() {
      Openbus::logger->log(logger::INFO, "ServerInterceptor::getCredential() BEGIN");
      Openbus::logger->indent();
      CORBA::Any_var any = picurrent->get_slot(slotid);

    #ifdef OPENBUS_ORBIX
      tecgraf::openbus::core::v1_05::access_control_service::Credential* c = 0;
      any >>= c;
      if (c) {
        Openbus::logger->log(logger::INFO, "credential->owner: " + (std::string) c->owner);
        Openbus::logger->log(logger::INFO, "credential->identifier: " + (std::string) c->identifier);
        Openbus::logger->log(logger::INFO, "credential->delegate: " + (std::string) c->delegate);
        tecgraf::openbus::core::v1_05::access_control_service::Credential_var ret = new tecgraf::openbus::core::v1_05::access_control_service::Credential();
        ret->owner = CORBA::string_dup(c->owner);
        ret->identifier = CORBA::string_dup(c->identifier);
        ret->delegate = CORBA::string_dup(c->delegate);
    #else
        tecgraf::openbus::core::v1_05::access_control_service::Credential c;
      if (any >>=c) {
        Openbus::logger->log(logger::INFO, "credential->owner: " + (std::string) c.owner);
        Openbus::logger->log(logger::INFO, "credential->identifier: " + (std::string) c.identifier);
        Openbus::logger->log(logger::INFO, "credential->delegate: " + (std::string) c.delegate);
        tecgraf::openbus::core::v1_05::access_control_service::Credential_var ret = new tecgraf::openbus::core::v1_05::access_control_service::Credential();
        ret->owner = CORBA::string_dup(c.owner);
        ret->identifier = CORBA::string_dup(c.identifier);
        ret->delegate = CORBA::string_dup(c.delegate);
    #endif
        Openbus::logger->dedent(logger::INFO, "ServerInterceptor::getCredential() END");
        return ret._retn();
      } else {
        Openbus::logger->log(logger::WARNING, "Nao foi possivel adquirir uma credencial. " + 
                             (std::string) + "Verifique se esta chamada esta sendo realizada dentro de um metodo " + 
                             (std::string) + "disponibilizado remotamente.");
        Openbus::logger->dedent(logger::INFO, "ServerInterceptor::getCredential() END");
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
