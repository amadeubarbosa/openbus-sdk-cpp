/*
** openbus.cpp
*/

#include "openbus.h"

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if HAVE_UNISTD_H
  #include <unistd.h>
#endif

#ifdef OPENBUS_ORBIX
  #include <omg/orb.hh>
  #include <it_ts/thread.h>
  #include <scs.hh>
  extern "C" {
    #include <IOR.h>
  }
#else
  #include <CORBA.h>
  #include <scs.h>
  #include <mico/pi_impl.h>
#endif

#define CHALLENGE_SIZE 36

namespace openbus {
  logger::Logger* Openbus::logger  = 0;
  char* Openbus::debugFile = 0;
  char* Openbus::FTConfigFilename = 0;
  bool Openbus::orbRunning = true;
  interceptors::ORBInitializerImpl* Openbus::ini = 0;
  CORBA::ORB_var Openbus::orb = CORBA::ORB::_nil();
  PortableServer::POA_var Openbus::poa = PortableServer::POA::_nil();
  lua_State* Openbus::luaState = 0;
  Openbus* Openbus::bus = 0;
  Openbus::LeaseExpiredCallback* Openbus::_leaseExpiredCallback = 0;
#ifdef OPENBUS_ORBIX
  IT_Mutex Openbus::mutex;
  Openbus::RenewLeaseThread* Openbus::renewLeaseThread = 0;
#else
  MICOMT::Mutex Openbus::mutex;
  #ifdef OPENBUS_SDK_MULTITHREAD
  Openbus::RunThread* Openbus::runThread = 0;
  Openbus::RenewLogin * Openbus::renewLogin = 0;
  MICOMT::Thread::ThreadKey Openbus::threadKey;
  #endif
#endif

#ifdef OPENBUS_ORBIX
  Openbus::RenewLeaseThread::RenewLeaseThread() {
    runningLeaseExpiredCallback = false;
    sigINT = false;
  }
    
  bool Openbus::RenewLeaseThread::sleep(unsigned short time) {
    unsigned short count = 0;
    for (;count < time; count++) {
      if (sigINT) {
        sigINT = false;
        return true;
      }
      #ifdef OPENBUS_ORBIX
        IT_CurrentThread::sleep(1000);
      #endif
    }
    return false;
  }
    
  void Openbus::RenewLeaseThread::stop() {
    sigINT = true;
  }
    
  void* Openbus::RenewLeaseThread::run() {
    logger->log(logger::INFO, "Openbus::RenewLeaseThread::run() BEGIN");
    bool interrupted = false;
    unsigned int timeRenewing = 1;
    std::stringstream msg;
    bool tryExec_LeaseExpiredCallback = false;
    bus->setThreadCredential(bus->credential);
    while (true) {
      logger->log(logger::INFO, "Openbus::RenewLeaseThread::run() WHILE BEGIN");
      msg << "sleep [" << timeRenewing <<"]s ...";
      logger->log(logger::INFO, msg.str());
      msg.str("");
      #if _WIN32
      interrupted = this->sleep(timeRenewing * 100);
      #else
      interrupted = this->sleep(timeRenewing);
      #endif

      mutex.lock();
      if (bus && !interrupted) {
        timeRenewing = bus->timeRenewing;
        if (bus->connectionState != CONNECTED) {
          logger->log(logger::INFO, "[RenewLeaseThread] Não há mais conexão com o barramento.");
          break;
        } else {
          logger->log(logger::INFO, "[RenewLeaseThread] Renovando credencial...");
          try {
            bool status = bus->iLeaseProvider->renewLease(*bus->credential, 
            bus->lease);
            if (!bus->timeRenewingFixe) {
              bus->timeRenewing = bus->lease/3;
            }
            msg << "[RenewLeaseThread] Próximo intervalo de renovação: " << bus->timeRenewing << "s";
            logger->log(logger::INFO, msg.str());
            msg.str("");
            if (!status) {
              logger->log(logger::WARNING, "[RenewLeaseThread] Não foi possível renovar a credencial!");
              tryExec_LeaseExpiredCallback = true;
              break;
            } else {
              logger->log(logger::INFO, "[RenewLeaseThread] Credencial renovada!");
            }
          } catch (CORBA::Exception& e) {
            logger->log(logger::WARNING, "[RenewLeaseThread] Não foi possível renovar a credencial!");
            tryExec_LeaseExpiredCallback = true;
            break;
          }
          Openbus::logger->log(logger::INFO, "[RenewLeaseThread] Atualizando intervalo de tempo de renovação na RewnewLeaseThread.");
          timeRenewing = bus->timeRenewing;
          mutex.unlock();
        }
      } else {
        logger->log(logger::WARNING, "[RenewLeaseThread] Referência ao barramento destruída.");
        break;
      }
    } 
    if (tryExec_LeaseExpiredCallback) {
      if (_leaseExpiredCallback) {
        runningLeaseExpiredCallback = true;
        mutex.unlock();
        _leaseExpiredCallback->expired();
        mutex.lock();
        Openbus::logger->log(logger::INFO, "[RenewLeaseThread] LeaseExpiredCallback executada.");
      } else {
        logger->log(logger::INFO, "[RenewLeaseThread] Nenhuma callback registrada.");
      }       
    } 
    runningLeaseExpiredCallback = false;
    mutex.unlock();
    logger->log(logger::INFO, "Openbus::RenewLeaseThread::run() END");
    return 0;
  }

#else
#ifdef OPENBUS_SDK_MULTITHREAD

void Openbus::RunThread::_run(void *) {
  logger->log(logger::INFO, "[RunThread::RunThread]");
  orb->run();
}

Openbus::RenewLogin::RenewLogin(tecgraf::openbus::core::v1_05::access_control_service::ILeaseProvider_var i, 
                                unsigned short t)
  : _condVar(&_mutex), _iLeaseProvider(i), _validityTime(t), _pause(false), _stop(false) 
{
  logger->log(logger::INFO, "[RenewLogin::RenewLogin]");
}

Openbus::RenewLogin::~RenewLogin() { 
  logger->log(logger::INFO, "[RenewLogin::~RenewLogin]");
}

tecgraf::openbus::core::v1_05::access_control_service::Lease Openbus::RenewLogin::renew() {
  logger->log(logger::INFO, "[RunThread::renew]");
  tecgraf::openbus::core::v1_05::access_control_service::Lease validityTime = _validityTime;
  bool expired = false;
  try {
    logger->log(logger::INFO, "[Chamando remotamente renew()]");
    expired = !(_iLeaseProvider->renewLease(*bus->credential, _validityTime));
  } catch (CORBA::NO_PERMISSION &) {
    expired = true;
  } catch (CORBA::Exception &) {
    logger->log(logger::WARNING, "[Falha na renovacao da credencial]");
  }
  if (expired) {
    logger->log(logger::WARNING, "[Credencial expirada.]");
    if (_leaseExpiredCallback) _leaseExpiredCallback->expired();    
  }
  return validityTime;
}

void Openbus::RenewLogin::_run(void *) {
  logger->log(logger::INFO, "[RenewLogin::_run]");
  _mutex.lock();
  while (!_stop) {
    while (!_pause && _condVar.timedwait(_validityTime * 1000)) {
      _mutex.unlock();
      logger->log(logger::INFO, "[Chamando RenewLogin::renew()]");
      _validityTime = renew();
      _mutex.lock();
    }
    if (!_stop) {
      logger->log(logger::INFO, "[condVar.wait()]");
      MICO_Boolean r = _condVar.wait();
      assert(r);
    }
  }
  _mutex.unlock();
}

void Openbus::RenewLogin::stop() {
  logger->log(logger::INFO, "[RenewLogin::stop]");
  _mutex.lock();
  _stop = true;
  logger->log(logger::INFO, "[condVar.signal()]");
  _condVar.signal();
  _mutex.unlock();
}

void Openbus::RenewLogin::pause() {
  logger->log(logger::INFO, "[RenewLogin::pause]");
  _mutex.lock();
  _pause = true;
  logger->log(logger::INFO, "[condVar.signal()]");
  _condVar.signal();
  _mutex.unlock();
}

void Openbus::RenewLogin::run() {
  logger->log(logger::INFO, "[RenewLogin::run]");
  _mutex.lock();
  _pause = false;
  logger->log(logger::INFO, "[condVar.signal()]");
  _condVar.signal();
  _mutex.unlock();
}
  
#else
  
  Openbus::RenewLeaseCallback::RenewLeaseCallback() { }

  void Openbus::RenewLeaseCallback::callback(CORBA::Dispatcher* dispatcher, Event event) {
    logger->log(logger::INFO, "Openbus::RenewLeaseCallback::callback() BEGIN");
    logger->indent();
    if (bus && bus->connectionState == CONNECTED) {
      logger->log(logger::INFO, "Renovando credencial...");
      try {
        bool status = bus->iLeaseProvider->renewLease(*bus->credential, bus->lease);
        if (!bus->timeRenewingFixe) {
          bus->timeRenewing = bus->lease/3;
        }
        std::stringstream msg;
        msg << "Proximo intervalo de renovacao: " << bus->timeRenewing << "s";
        logger->log(logger::INFO, msg.str());
        if (!status) {
          logger->log(logger::WARNING, "Nao foi possivel renovar a credencial!");
          logger->log(logger::WARNING, "ACS retornou credencial inválida.");
          if (_leaseExpiredCallback) {
            _leaseExpiredCallback->expired();
          } else {
            logger->log(logger::INFO, "Nenhuma callback registrada.");
          }
        } else {
          logger->log(logger::INFO, "Credencial renovada!");
          dispatcher->tm_event(this, bus->timeRenewing*1000);
        }
      } catch (CORBA::Exception& e) {
        logger->log(logger::WARNING, "Nao foi possivel renovar a credencial!");
        /* Passar para o logger. */
        e._print_stack_trace(cout);
        if (_leaseExpiredCallback) {
          _leaseExpiredCallback->expired();
        } else {
          logger->log(logger::INFO, "Nenhuma callback registrada.");
        }
      }
      logger->dedent(logger::INFO, "Openbus::RenewLeaseCallback::callback() END");
    }
  }
#endif
#endif

  void Openbus::commandLineParse() {
    timeRenewingFixe = false;
    for (short int idx = 1; idx < _argc; idx++) {
      if (!strcmp(_argv[idx], "-OpenbusHost")) {
        hostBus = _argv[++idx];
      } else if (!strcmp(_argv[idx], "-OpenbusPort")) {
        portBus = (unsigned short int) strtoul(_argv[++idx], 0, 10);
      } else if (!strcmp(_argv[idx], "-OpenbusTimeRenewing")) {
        timeRenewing = (unsigned int) strtoul(_argv[++idx], 0, 10);
        timeRenewingFixe = true;
      } else if (!strcmp(_argv[idx], "-OpenbusDebug")) {
        char* debugLevelStr = _argv[++idx];
        if (!strcmp(debugLevelStr, "ALL")) {
          debugLevel = logger::ALL;
        } else if (!strcmp(debugLevelStr, "ERROR")) {
          debugLevel = logger::ERROR;
        } else if (!strcmp(debugLevelStr, "INFO")) {
          debugLevel = logger::INFO;
        } else if (!strcmp(debugLevelStr, "WARNING")) {
          debugLevel = logger::WARNING;
        }
      } else if (!strcmp(_argv[idx], "-OpenbusDebugFile")) {
        debugFile = _argv[++idx];
      } else if (!strcmp(_argv[idx], "-OpenbusValidationPolicy")) {
        char* credentialValidationPolicyStr = _argv[++idx];
        if (!strcmp(credentialValidationPolicyStr, "NONE")) {
          credentialValidationPolicy = interceptors::NONE;
        } else if (!strcmp(credentialValidationPolicyStr, "ALWAYS")) { 
          credentialValidationPolicy = interceptors::ALWAYS;
        } else if (!strcmp(credentialValidationPolicyStr, "CACHED")) { 
          credentialValidationPolicy = interceptors::CACHED;
        }
      } else if (!strcmp(_argv[idx], "-OpenbusFTConfigFilename")) {
        faultToleranceEnable = true;
        FTConfigFilename = _argv[++idx];
      } else if (!strcmp(_argv[idx], "-OpenbusValidationTime")) {
          ini->getServerInterceptor()->setValidationTime(strtoul(_argv[++idx], 0, 10));
      } 
    }
  }

  void Openbus::createORB() {
    if (CORBA::is_nil(orb)) {
      logger->log(logger::INFO, "Criando ORB...");
    #ifdef OPENBUS_ORBIX
      orb = CORBA::ORB_init(_argc, _argv, "tecgraf.openbus");
    #else
      orb = CORBA::ORB_init(_argc, _argv);    
    #endif
      getRootPOA();
    }
  }

  void Openbus::setRegistryService() {
    if (!iRegistryService) {
      iRegistryService = getRegistryService(); 
    }
  }

  void Openbus::initialize() {
    hostBus = "";
    portBus = 2089;
  }

  void Openbus::registerInterceptors() {
    if (!ini) {
      ini = new interceptors::ORBInitializerImpl();
      PortableInterceptor::register_orb_initializer(ini);
    }
  }

  void Openbus::newState() {
    faultToleranceEnable = false;
    debugLevel = logger::OFF;
    connectionState = DISCONNECTED;
    credential = 0;
  #if (!OPENBUS_ORBIX && OPENBUS_SDK_MULTITHREAD)    
    MICOMT::Thread::set_specific(threadKey, 0);
  #endif
    lease = 0;
    iAccessControlService = tecgraf::openbus::core::v1_05::access_control_service::IAccessControlService::_nil();
    iRegistryService = 0;
    iLeaseProvider = tecgraf::openbus::core::v1_05::access_control_service::ILeaseProvider::_nil();
    iComponentAccessControlService = scs::core::IComponent::_nil();
  }

  Openbus::Openbus() {
    luaState = lua_open();
    luaL_openlibs(luaState);
  #ifdef OPENBUS_ORBIX
    luaopen_IOR(luaState);
  #endif
  #if (!OPENBUS_ORBIX && OPENBUS_SDK_MULTITHREAD)
    MICOMT::Thread::create_key(threadKey, 0);
  #endif
    newState();
    credentialValidationPolicy = openbus::interceptors::ALWAYS;
    registerInterceptors();
    initialize();
  }

  Openbus::~Openbus() {
    logger->log(logger::INFO, "Openbus::~Openbus() BEGIN");
    logger->indent();
    mutex.lock();
    if (bus) {
      logger->log(logger::INFO, "Deletando lista de métodos interceptáveis...");
      MethodsNotInterceptable::iterator iter = methodsNotInterceptable.begin();
      while (iter != methodsNotInterceptable.end()) {
        MethodSet *methods = iter->second;
        methodsNotInterceptable.erase(iter++);
        methods->clear();
        delete methods;
      }
      if (!CORBA::is_nil(orb)) {
      #ifndef OPENBUS_ORBIX 
        #ifndef OPENBUS_SDK_MULTITHREAD
          logger->log(logger::INFO, "Removendo callback de renovação de credencial...");
          orb->dispatcher()->remove(&renewLeaseCallback, CORBA::Dispatcher::Timer);
        #endif
      #endif
      }
      #ifdef OPENBUS_ORBIX
        if (renewLeaseThread) {
          bool b;
          b = renewLeaseThread->runningLeaseExpiredCallback;
          if (!b) {
            Openbus::logger->log(logger::INFO, "Aguardando término da renewLeaseThread...");
          /* Por que o meu wait não fica bloqueado quando o LeaseExpiredCallback() chamda 
          *  disconnect() ?!
          */
            mutex.unlock();
            renewLeaseThread->stop();
            Openbus::logger->log(logger::INFO, "Esperando RenewLeaseThread...");
            renewLeaseIT_Thread.join();
            mutex.lock();
            Openbus::logger->log(logger::INFO, "delete renewLeaseThread.");
            delete renewLeaseThread;
            renewLeaseThread = 0;
          } else {
            Openbus::logger->log(logger::WARNING, 
              "Não é possível finalizar RenewLeaseThread porque estamos dentro de \
              LeaseExpiredCallback");
          }
        }
      #else
      #ifdef OPENBUS_SDK_MULTITHREAD
        if (renewLogin) {
          renewLogin->stop();
          renewLogin->wait();
        }
        MICOMT::Thread::delete_key(threadKey);
      #endif
      #endif
      bus = 0;
    }
    mutex.unlock();
    logger->dedent(logger::INFO, "Openbus::~Openbus() END");
    delete logger;
  }

  void Openbus::terminationHandlerCallback(long signalType) {
    logger->log(logger::INFO, "Openbus::terminationHandlerCallback() BEGIN");
    logger->indent();
  /* Prevenção caso o usuário consiga executar o _termination handler_ mais de uma vez */
    if (bus) {
      try {
        if (bus->isConnected()) {
          Openbus::logger->log(logger::INFO, "Desconectando usuário do barramento.");
          bus->disconnect();
        }
      } catch(CORBA::Exception& e) {
        logger->log(logger::WARNING, 
          "Não foi possível se desconectar corretamente do barramento."); 
      }
      bus->stop();
      bus->finish();
      delete bus;
    }
  }

  Openbus* Openbus::getInstance() {
    mutex.lock();
    if (!bus) {
      logger = new logger::Logger();
      bus = new Openbus();
    }
    mutex.unlock();
    return bus;
  }

  void Openbus::init(
    int argc,
    char** argv)
  {
    std::stringstream msgLog;
    _argc = argc;
    _argv = argv;
    commandLineParse();
    logger->setOutput(debugFile);
    logger->setLevel(debugLevel);
    logger->log(logger::INFO, "Openbus::init() BEGIN");
    logger->indent();
    msgLog << "hostBus: " << hostBus;
    logger->log(logger::INFO, msgLog.str());
    msgLog.str("");
    msgLog << "portBus: " << portBus;
    logger->log(logger::INFO, msgLog.str());
    msgLog.str("");
    msgLog << "credentialValidationPolicy: " << credentialValidationPolicy;
    logger->log(logger::INFO, msgLog.str());
    msgLog.str("");
    createORB();
    #ifdef OPENBUS_ORBIX
      if (credentialValidationPolicy == interceptors::CACHED) {
        ini->getServerInterceptor()->registerValidationTimer();
      }
    #else
      if (credentialValidationPolicy == interceptors::CACHED) {
        ini->getServerInterceptor()->registerValidationDispatcher();
      }
      #ifdef OPENBUS_SDK_MULTITHREAD
        if (!runThread) {
          Openbus::logger->log(logger::INFO, "Criando uma RunThread.");
          runThread = new RunThread();
          runThread->start();
        }
      #endif
    #endif
    logger->dedent(logger::INFO, "Openbus::init() END");
  }

  void Openbus::init(
    int argc,
    char** argv,
    char* host,
    unsigned short port,
    interceptors::CredentialValidationPolicy policy)
  {
    hostBus = host;
    portBus = port;
    credentialValidationPolicy = policy;
    init(argc, argv);
  }

  bool Openbus::isConnected() {
    logger->log(logger::INFO, "Openbus::isConnected() BEGIN");
    logger->indent();
    if (connectionState == CONNECTED) {
      logger->log(logger::INFO, "Está conectado.");
      logger->dedent(logger::INFO, "Openbus::isConnected() END");
      return true;
    }
    logger->log(logger::INFO, "NÃO está conectado.");
    logger->dedent(logger::INFO, "Openbus::isConnected() END");
    return false;
  }

  CORBA::ORB* Openbus::getORB() {
    return orb;
  }

  PortableServer::POA* Openbus::getRootPOA() {
    logger->log(logger::INFO, "Openbus::getRootPOA() BEGIN");
    logger->indent();
    if (CORBA::is_nil(poa) && !CORBA::is_nil(orb)) {
      logger->log(logger::INFO, "Criando POA...");
      CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
      poa = PortableServer::POA::_narrow(poa_obj);
      poa_manager = poa->the_POAManager();
      poa_manager->activate();
    }
    logger->dedent(logger::INFO, "Openbus::getRootPOA() END");
    return poa;
  }

  tecgraf::openbus::core::v1_05::access_control_service::Credential_var Openbus::getInterceptedCredential() {
    return ini->getServerInterceptor()->getCredential();
  }

  tecgraf::openbus::core::v1_05::access_control_service::IAccessControlService* Openbus::getAccessControlService() {
    return iAccessControlService;
  }

  tecgraf::openbus::core::v1_05::registry_service::IRegistryService* Openbus::getRegistryService() {
    logger->log(logger::INFO, "Openbus::getRegistryService() BEGIN");
    logger->indent();
    scs::core::IComponent_var iComponentRegistryService;
    if ((scs::core::IComponent*) iComponentAccessControlService) {
      CORBA::Object_var objRecep = iComponentAccessControlService->getFacetByName("IReceptacles");
      scs::core::IReceptacles_var recep = scs::core::IReceptacles::_narrow(objRecep);
      try {
        scs::core::ConnectionDescriptions_var conns =
          recep->getConnections("RegistryServiceReceptacle");
        if (conns->length() > 0) {
          logger->log(logger::INFO, "Adquirindo RegistryService...");
          CORBA::Object_var objref = conns[(CORBA::ULong) 0].objref;
          iComponentRegistryService = scs::core::IComponent::_narrow(objref);
          objref = iComponentRegistryService->getFacetByName("IRegistryService_v1_05");
          iRegistryService = tecgraf::openbus::core::v1_05::registry_service::IRegistryService::_narrow(objref);
        }
      } catch (CORBA::Exception& e) {
        logger->log(logger::ERROR, "Não foi possível obter o serviço de registro.");
        // TODO: necessário fazer um throw?
      }
    }
    logger->dedent(logger::INFO, "Openbus::getRegistryService() END");
    return iRegistryService;
  } 

  tecgraf::openbus::core::v1_05::access_control_service::Credential* Openbus::getCredential() {
  #if (!OPENBUS_ORBIX && OPENBUS_SDK_MULTITHREAD)
    tecgraf::openbus::core::v1_05::access_control_service::Credential* threadCredential = 
      (tecgraf::openbus::core::v1_05::access_control_service::Credential*) MICOMT::Thread::get_specific(threadKey);
    if (threadCredential) {
      return threadCredential;
    } else {
      return credential;
    }
  #else
    return credential;
  #endif
    }

  interceptors::CredentialValidationPolicy Openbus::getCredentialValidationPolicy() {
    return credentialValidationPolicy;
  }

  void Openbus::setThreadCredential(tecgraf::openbus::core::v1_05::access_control_service::Credential* credential) {
  #if (!OPENBUS_ORBIX && OPENBUS_SDK_MULTITHREAD)
    MICOMT::Thread::set_specific(threadKey, (void*) credential);
  #else
    this->credential = credential;
  #endif
  }

  void Openbus::setLeaseExpiredCallback(LeaseExpiredCallback* leaseExpiredCallback) 
  {
    mutex.lock();
    _leaseExpiredCallback = leaseExpiredCallback;
    mutex.unlock();
  }

  void Openbus::removeLeaseExpiredCallback()
  {
    mutex.lock();
    _leaseExpiredCallback = 0;
    mutex.unlock();
  }

  void Openbus::createProxyToIAccessControlService() {
    logger->log(logger::INFO, "Openbus::createProxyToIAccessControlService() BEGIN");
    logger->indent();
  /* Pode ser chamado de dentro da RenewLeaseThread. */
    std::stringstream corbalocIC;
    corbalocIC << "corbaloc::" << hostBus << ":" << portBus << "/openbus_v1_05";
    logger->log(logger::INFO, "corbaloc IC do ACS: " + corbalocIC.str());
    CORBA::Object_var objIC = 
      orb->string_to_object(corbalocIC.str().c_str());
    iComponentAccessControlService = scs::core::IComponent::_narrow(objIC);
    CORBA::Object_var objLP = iComponentAccessControlService->getFacet(
      "IDL:tecgraf/openbus/core/v1_05/access_control_service/ILeaseProvider:1.0");
    iLeaseProvider = tecgraf::openbus::core::v1_05::access_control_service::ILeaseProvider::_narrow(objLP);
    CORBA::Object_var objACS = iComponentAccessControlService->getFacet(
      "IDL:tecgraf/openbus/core/v1_05/access_control_service/IAccessControlService:1.0");
    iAccessControlService = 
      tecgraf::openbus::core::v1_05::access_control_service::IAccessControlService::_narrow(objACS);
    CORBA::Object_var objFT = iComponentAccessControlService->getFacet(
      "IDL:tecgraf/openbus/fault_tolerance/v1_05/IFaultTolerantService:1.0");
    iFaultTolerantService = tecgraf::openbus::fault_tolerance::v1_05::IFaultTolerantService::_narrow(objFT);
    logger->dedent(logger::INFO, "Openbus::createProxyToIAccessControlService() END");
  }

  tecgraf::openbus::core::v1_05::registry_service::IRegistryService* Openbus::connect(
    const char* user,
    const char* password)
  {
    logger->log(logger::INFO, "Openbus::connect() BEGIN");
    logger->indent();
    mutex.lock();
    if (connectionState == DISCONNECTED) {
      try {
        std::stringstream portMSG;
        std::stringstream userMSG;
        std::stringstream passwordMSG;
        std::stringstream orbMSG;
        logger->log(logger::INFO, "host = " + hostBus);
        portMSG << "port = " << portBus; 
        logger->log(logger::INFO, portMSG.str());
        userMSG << "username = " << user;
        logger->log(logger::INFO, userMSG.str());
        passwordMSG<< "password = " << password;
        logger->log(logger::INFO, passwordMSG.str());
        orbMSG<< "orb = " << &orb;
        logger->log(logger::INFO, orbMSG.str());
        if (CORBA::is_nil(iAccessControlService)) {
          createProxyToIAccessControlService();
        }

        std::stringstream iACSMSG;
        iACSMSG << "iAccessControlService = " << &iAccessControlService; 
        logger->log(logger::INFO, iACSMSG.str());
        if (!iAccessControlService->loginByPassword(user, password, credential,
          lease))
        {
          logger->log(logger::ERROR, "Throwing LOGIN_FAILURE...");
          logger->dedent(logger::INFO, "Openbus::connect() END");
          mutex.unlock();
          throw LOGIN_FAILURE();
        } else {
          std::stringstream msg;
          msg << "Associando credencial " << credential << " ao ORB.";
          logger->log(logger::INFO, msg.str());
          connectionState = CONNECTED;
          setThreadCredential(credential);
          if (!timeRenewingFixe) {
            timeRenewing = lease/3;
          }
          setRegistryService();
          #ifdef OPENBUS_ORBIX
            if (!renewLeaseThread) {
              Openbus::logger->log(logger::INFO, "Criando uma RenewLeaseThread.");
              renewLeaseThread = new RenewLeaseThread();
              renewLeaseIT_Thread = IT_ThreadFactory::smf_start(
                *renewLeaseThread, 
                IT_ThreadFactory::attached, 
                0);
            }
          #else
            #ifdef OPENBUS_SDK_MULTITHREAD
              if (!renewLogin) {
                Openbus::logger->log(logger::INFO, "Criando uma RenewLogin.");
                renewLogin = new Openbus::RenewLogin(iLeaseProvider, timeRenewing);
                renewLogin->start();
              } else renewLogin->run();
            #else
              Openbus::logger->log(logger::INFO, "Registrando evento de renovação da credencial...");
              orb->dispatcher()->tm_event(&renewLeaseCallback, timeRenewing*1000);
            #endif
          #endif
          logger->dedent(logger::INFO, "Openbus::connect() END");
          mutex.unlock();
          return iRegistryService;
        }
      } catch (const CORBA::SystemException& systemException) {
        logger->log(logger::ERROR, "Throwing CORBA::SystemException...");
        logger->dedent(logger::INFO, "Openbus::connect() END");
        mutex.unlock();
        throw;
      }
    } else {
      logger->log(logger::INFO, "Ja ha uma conexão ativa.");
      logger->dedent(logger::INFO, "Openbus::connect() END");
      mutex.unlock();
      throw LOGIN_FAILURE();
    }
  }

  tecgraf::openbus::core::v1_05::registry_service::IRegistryService* Openbus::connect(
    const char* entity,
    const char* privateKeyFilename,
    const char* ACSCertificateFilename)
  {
    mutex.lock();
    logger->log(logger::INFO, "Openbus::connect() BEGIN");
    logger->indent();
    if (connectionState == DISCONNECTED) {
      try {
        std::stringstream portMSG;
        std::stringstream entityMSG;
        std::stringstream privateKeyFilenameMSG;
        std::stringstream ACSCertificateFilenameMSG;
        logger->log(logger::INFO, "host = " + hostBus);
        portMSG << "port = " << portBus; 
        logger->log(logger::INFO, portMSG.str());
        entityMSG << "entity = " << entity;
        logger->log(logger::INFO, entityMSG.str());
        privateKeyFilenameMSG << "privateKeyFilename = " << privateKeyFilename;
        logger->log(logger::INFO, privateKeyFilenameMSG.str());
        ACSCertificateFilenameMSG << "ACSCertificateFilename = " << 
          ACSCertificateFilename;
        logger->log(logger::INFO, ACSCertificateFilenameMSG.str());
        if (CORBA::is_nil(iAccessControlService)) {
          createProxyToIAccessControlService();
        }

      /* Requisição de um "desafio" que somente poderá ser decifrado através
      *  da chave privada da entidade reconhecida pelo barramento.
      */
        tecgraf::openbus::core::v1_05::OctetSeq_var octetSeq =
          iAccessControlService->getChallenge(entity);
        if (octetSeq->length() == 0) {
          logger->log(logger::ERROR, "Throwing SECURITY_EXCEPTION...");
          logger->dedent(logger::INFO, "Openbus::connect() END");
          mutex.unlock();
          throw SECURITY_EXCEPTION(
            "O ACS não encontrou o certificado do serviço.");
        }
        unsigned char* challenge = octetSeq->get_buffer();

      /* Leitura da chave privada da entidade. */
        BIO* bio = BIO_new( BIO_s_file() );
        if (BIO_read_filename( bio, privateKeyFilename ) <= 0) {
          std::stringstream filename;
          filename << "Não foi possível abrir o arquivo: " << 
            privateKeyFilename;
          logger->log(logger::WARNING, filename.str());
          logger->log(logger::ERROR, "Throwing SECURITY_EXCEPTION...");
          logger->dedent(logger::INFO, "Openbus::connect() END");
          mutex.unlock();
          throw SECURITY_EXCEPTION(
            "Não foi possível abrir o arquivo que armazena a chave privada.");
        }
        EVP_PKEY* EntityPrivateKey  = PEM_read_bio_PrivateKey( bio, NULL, NULL, 0 );
        if (EntityPrivateKey == 0) {
          logger->log(logger::WARNING, "Não foi possível obter a chave privada da entidade.");
          logger->log(logger::ERROR, "Throwing SECURITY_EXCEPTION...");
          logger->dedent(logger::INFO, "Openbus::connect() END");
          EVP_PKEY_free(EntityPrivateKey);
          mutex.unlock();
          throw SECURITY_EXCEPTION(
            "Não foi possível obter a chave privada da entidade.");
        }
        
        int RSAModulusSize = EVP_PKEY_size(EntityPrivateKey);

      /* Decifrando o desafio. */
        unsigned char* challengePlainText =
          (unsigned char*) malloc(RSAModulusSize);
        memset(challengePlainText, ' ', RSAModulusSize);

        RSA_private_decrypt(RSAModulusSize, challenge, challengePlainText,
          EntityPrivateKey->pkey.rsa, RSA_PKCS1_PADDING);

        bio = BIO_new( BIO_s_file() );
        if (BIO_read_filename( bio, ACSCertificateFilename ) <= 0) {
          std::stringstream filename;
          filename << "Não foi possível abrir o arquivo: " << 
            ACSCertificateFilename;
          logger->log(logger::WARNING, filename.str());
          logger->log(logger::ERROR, "Throwing SECURITY_EXCEPTION...");
          logger->dedent(logger::INFO, "Openbus::connect() END");
          mutex.unlock();
          throw SECURITY_EXCEPTION(
            "Não foi possível abrir o arquivo que armazena o certificado ACS.");
        }
      
        EVP_PKEY_free(EntityPrivateKey);
      
        X509* x509 = d2i_X509_bio(bio, 0);

      /* Obtenção da chave pública do ACS. */
        EVP_PKEY* ACSPublicKey = X509_get_pubkey(x509);
        if (ACSPublicKey == 0) {
          logger->log(logger::WARNING, "Não foi possível obter a chave pública do ACS.");
          logger->log(logger::ERROR, "Throwing SECURITY_EXCEPTION...");
          logger->dedent(logger::INFO, "Openbus::connect() END");
          free(challengePlainText);
          EVP_PKEY_free(ACSPublicKey);
          X509_free(x509);
          mutex.unlock();
          throw SECURITY_EXCEPTION(
            "Não foi possível obter a chave pública do ACS.");
        }

      /* Reposta ao desafio, ou seja, cifra do desafio utilizando a chave
      *  pública do ACS.
      */
        unsigned char* answer = (unsigned char*) malloc(RSAModulusSize);
        RSA_public_encrypt(CHALLENGE_SIZE, challengePlainText, answer,
          ACSPublicKey->pkey.rsa, RSA_PKCS1_PADDING);

        free(challengePlainText);

        tecgraf::openbus::core::v1_05::OctetSeq_var answerOctetSeq = new tecgraf::openbus::core::v1_05::OctetSeq(
          (CORBA::ULong) RSAModulusSize, (CORBA::ULong) RSAModulusSize,
          (CORBA::Octet*)answer, 0);

        EVP_PKEY_free(ACSPublicKey);
        X509_free(x509);

        std::stringstream iACSMSG;
        iACSMSG << "iAccessControlService = " << &iAccessControlService; 
        logger->log(logger::INFO, iACSMSG.str());
        if (!iAccessControlService->loginByCertificate(entity, answerOctetSeq,
          credential, lease))
        {
          free(answer);
          mutex.unlock();
          throw LOGIN_FAILURE();
        } else {
          free(answer);
          std::stringstream msg;
          msg << "Associando credencial " << credential << " ao ORB."; 
          logger->log(logger::INFO, msg.str());
          connectionState = CONNECTED;
          setThreadCredential(credential);
          if (!timeRenewingFixe) {
            timeRenewing = lease/3;
          }
          setRegistryService();
          #ifdef OPENBUS_ORBIX
            if (!renewLeaseThread) {
              Openbus::logger->log(logger::INFO, "Criando uma RenewLeaseThread.");
              renewLeaseThread = new RenewLeaseThread();
              renewLeaseIT_Thread = IT_ThreadFactory::smf_start(
                *renewLeaseThread, 
                IT_ThreadFactory::attached, 
                0);
            }
          #else
            #ifdef OPENBUS_SDK_MULTITHREAD
              if (!renewLogin) {
                Openbus::logger->log(logger::INFO, "Criando uma RenewLogin.");
                renewLogin = new Openbus::RenewLogin(iLeaseProvider, timeRenewing);
                renewLogin->start();
              } else renewLogin->run();
            #else
              Openbus::logger->log(logger::INFO, "Registrando evento de renovação da credencial...");
              orb->dispatcher()->tm_event(&renewLeaseCallback, timeRenewing*1000);
            #endif
          #endif
          mutex.unlock();
          logger->dedent(logger::INFO, "Openbus::connect() END");
          return iRegistryService;
        }
      } catch (const CORBA::SystemException& systemException) {
        logger->log(logger::ERROR, "Throwing CORBA::SystemException...");
        logger->dedent(logger::INFO, "Openbus::connect() END");
        mutex.unlock();
        throw;
      }
    } else {
      logger->log(logger::INFO, "Ja ha uma conexão ativa.");
      logger->dedent(logger::INFO, "Openbus::connect() END");
      mutex.unlock();
      throw LOGIN_FAILURE();
    }
  }

  bool Openbus::disconnect() {
    logger->log(logger::INFO, "Openbus::disconnect() BEGIN");
    logger->indent();
    mutex.lock();
    if (connectionState == CONNECTED) {
    #ifndef OPENBUS_ORBIX
      #ifndef OPENBUS_SDK_MULTITHREAD
        logger->log(logger::INFO, "Removendo callback de renovação de credencial...");
        orb->dispatcher()->remove(&renewLeaseCallback, CORBA::Dispatcher::Timer);
      #endif
    #endif
      delete iRegistryService;
      bool status;
      try {
        status = iAccessControlService->logout(*credential);
        if (!status) {
          logger->log(logger::WARNING, "Não foi possível realizar logout.");
        }
      } catch (CORBA::Exception& e) {
        logger->log(logger::WARNING, "Não foi possível realizar logout.");
      }
      newState();
      #ifdef OPENBUS_ORBIX
        if (renewLeaseThread) {
          bool b;
          b = renewLeaseThread->runningLeaseExpiredCallback;
          if (!b) {
            Openbus::logger->log(logger::INFO, "Aguardando término da renewLeaseThread...");
          /* Por que o meu wait não fica bloqueado quando o LeaseExpiredCallback() chama 
          *  disconnect() ?!
          */
            mutex.unlock();
            Openbus::logger->log(logger::INFO, "Esperando RenewLeaseThread...");
            #ifdef OPENBUS_ORBIX
              renewLeaseThread->stop();
              renewLeaseIT_Thread.join();
            #endif
            mutex.lock();
            Openbus::logger->log(logger::INFO, "delete renewLeaseThread.");
            delete renewLeaseThread;
            renewLeaseThread = 0;
          } else {
            Openbus::logger->log(logger::WARNING, 
              "Não é possível finalizar RenewLeaseThread porque estamos dentro de \
              LeaseExpiredCallback");
          }
        }
      #else
        if (renewLogin) renewLogin->pause();
      #endif
      logger->dedent(logger::INFO, "Openbus::disconnect() END");
      mutex.unlock();
      return status;
    } else {
      logger->log(logger::INFO, "Não há conexão a ser desfeita.");
      logger->dedent(logger::INFO, "Openbus::disconnect() END");
      mutex.unlock();
      return false;
    }
  }

  void Openbus::run() {
    Openbus::logger->log(logger::INFO, "Openbus::run()");
    #if (OPENBUS_ORBIX)
      orb->run();
    #else
      #if (OPENBUS_SDK_MULTITHREAD)
      /* Já temos uma runThread ? */
        if (!runThread) {
          Openbus::logger->log(logger::INFO, "Criando uma RunThread.");
          runThread = new RunThread();
          runThread->start();
        }
        runThread->wait();
      #else
      orbRunning = true;
      while (orbRunning) {
        if (orb->work_pending()) {
          orb->perform_work();
        }
      }
      #endif
    #endif
  }

  void Openbus::stop() {
    Openbus::logger->log(logger::INFO, "Openbus::stop() BEGIN");
    #if (!OPENBUS_ORBIX && !OPENBUS_SDK_MULTITHREAD)
      orbRunning = false;
    #endif
    Openbus::logger->log(logger::INFO, "Openbus::stop() END");
  }

  void Openbus::finish(bool force) {
    logger->log(logger::INFO, "Openbus::finish() BEGIN");
    logger->indent();
    std::stringstream msg;
    msg << "Desligando orb com force = " << force; 
    logger->log(logger::INFO, msg.str());
    orb->shutdown(force);
    orb->destroy();
    orb = CORBA::ORB::_nil();
    logger->dedent(logger::INFO, "Openbus::finish() END");
  }

  void Openbus::setInterceptable(std::string interfaceRepID, std::string method,
    bool isInterceptable)
  {
    MethodSet *methods;
    MethodsNotInterceptable::iterator iter;
    /* Guarda apenas os métodos que não são interceptados */
    if (isInterceptable) {
      iter = methodsNotInterceptable.find(interfaceRepID);
      if (iter != methodsNotInterceptable.end()) {
        methods = iter->second;
        methods->erase(method);
        if (methods->size() == 0) {
          methodsNotInterceptable.erase(iter);
          delete methods;
        }
      }
    }
    else {
      iter = methodsNotInterceptable.find(interfaceRepID);
      if (iter == methodsNotInterceptable.end()) {
        methods = new MethodSet();
        methodsNotInterceptable[interfaceRepID] = methods;
      }
      else
        methods = iter->second;
      methods->insert(method);
    }
  }

  bool Openbus::isInterceptable(std::string interfaceRepID, std::string method)
  {
    MethodsNotInterceptable::iterator iter = methodsNotInterceptable.find(interfaceRepID);
    if (iter != methodsNotInterceptable.end()) {
      MethodSet *methods = iter->second;
      MethodSet::iterator method_iter = methods->find(method);
      return (method_iter == methods->end());
    }
    return true;
  }

  bool Openbus::isFaultToleranceEnable() {
    return faultToleranceEnable;
  }
}
