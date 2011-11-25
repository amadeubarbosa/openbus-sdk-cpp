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
  Logger* Openbus::logger  = 0;
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
  #ifdef MULTITHREAD
    Openbus::RunThread* Openbus::runThread = 0;
    Openbus::RenewLeaseThread* Openbus::renewLeaseThread = 0;
    MICOMT::Thread::ThreadKey Openbus::threadKey;
  #endif
#endif

  #if (OPENBUS_ORBIX || (!OPENBUS_ORBIX && MULTITHREAD))
    Openbus::RenewLeaseThread::RenewLeaseThread() {
      runningLeaseExpiredCallback = false;
      sigINT = false;
    }
    
    void Openbus::RenewLeaseThread::sleep(unsigned short time) {
      unsigned short count = 0;
      for (;count < time; count++) {
        mutex.lock();
        if (sigINT) {
          sigINT = false;
          mutex.unlock();
          break;
        } else {
          mutex.unlock();
        }
        #ifdef OPENBUS_ORBIX
          IT_CurrentThread::sleep(1000);
        #else
        #ifndef _WIN32
          ::sleep(1);
        #else
          _sleep(1);
        #endif
        #endif
      }
    }
    
    void Openbus::RenewLeaseThread::stop() {
      mutex.lock();
      sigINT = true;
      mutex.unlock();
    }
    
    #ifdef OPENBUS_ORBIX
    void* Openbus::RenewLeaseThread::run() {
    #else
    void Openbus::RenewLeaseThread::_run(void*) {
    #endif
      unsigned int timeRenewing = 1;
      stringstream msg;
      bool tryExec_LeaseExpiredCallback = false;
      bus->setThreadCredential(bus->credential);
      while (true) {
        logger->log(INFO, "Openbus::RenewLeaseThread::run() BEGIN");
        msg << "sleep [" << timeRenewing <<"]s ...";
        logger->log(INFO, msg.str());
        msg.str("");
      #if _WIN32
        this->sleep(timeRenewing * 100);
      #else
        this->sleep(timeRenewing);
      #endif

        mutex.lock();
        if (bus) {
          timeRenewing = bus->timeRenewing;
          if (bus->connectionState != CONNECTED) {
            mutex.unlock();
            break;
          } else {
            logger->log(INFO, "Renovando credencial...");
            try {
              bool status = bus->iLeaseProvider->renewLease(*bus->credential, 
                bus->lease);
              if (!bus->timeRenewingFixe) {
                bus->timeRenewing = bus->lease/3;
              }
              msg << "Próximo intervalo de renovação: " << bus->timeRenewing << "s";
              logger->log(INFO, msg.str());
              msg.str("");
              if (!status) {
                logger->log(WARNING, "Não foi possível renovar a credencial!");
                tryExec_LeaseExpiredCallback = true;
                mutex.unlock();
                break;
              } else {
                logger->log(INFO, "Credencial renovada!");
                mutex.unlock();
              }
            } catch (CORBA::Exception& e) {
              logger->log(WARNING, "Não foi possível renovar a credencial!");
              tryExec_LeaseExpiredCallback = true;
              mutex.unlock();
              break;
            }
            Openbus::logger->log(INFO, "Atualizando intervalo de tempo de na RewnewLeaseThread.");
            timeRenewing = bus->timeRenewing;
            mutex.unlock();
            continue; 
          }
        }
      } 
      if (tryExec_LeaseExpiredCallback) {
        if (_leaseExpiredCallback) {
          runningLeaseExpiredCallback = true;
          _leaseExpiredCallback->expired();
          Openbus::logger->log(INFO, "LeaseExpiredCallback executada.");
        } else {
          logger->log(INFO, "Nenhuma callback registrada.");
        }       
      } 
      runningLeaseExpiredCallback = false;
      logger->log(INFO, "Openbus::RenewLeaseThread::run() END");
    #ifdef OPENBUS_ORBIX
      return 0;
    #endif
    }
    
    #ifdef MULTITHREAD
      void Openbus::RunThread::_run(void*) {
        logger->log(INFO, "[RunThread Iniciada]");
        orb->run();
        logger->log(INFO, "[RunThread Encerrada]");
      }
    #endif
  #else
    Openbus::RenewLeaseCallback::RenewLeaseCallback() {
    }

    void Openbus::RenewLeaseCallback::callback(
      CORBA::Dispatcher* dispatcher, 
      Event event) 
    {
      logger->log(INFO, "Openbus::RenewLeaseCallback::callback() BEGIN");
      logger->indent();
      if (bus && bus->connectionState == CONNECTED) {
        logger->log(INFO, "Renovando credencial...");
        try {
          bool status = bus->iLeaseProvider->renewLease(*bus->credential, bus->lease);
          if (!bus->timeRenewingFixe) {
            bus->timeRenewing = bus->lease/3;
          }
          stringstream msg;
          msg << "Proximo intervalo de renovacao: " << bus->timeRenewing << "s";
          logger->log(INFO, msg.str());
          if (!status) {
            logger->log(WARNING, "Nao foi possivel renovar a credencial!");
            logger->log(WARNING, "ACS retornou credencial inválida.");
            if (_leaseExpiredCallback) {
              _leaseExpiredCallback->expired();
            } else {
              logger->log(INFO, "Nenhuma callback registrada.");
            }
          } else {
            logger->log(INFO, "Credencial renovada!");
            dispatcher->tm_event(this, bus->timeRenewing*1000);
          }
        } catch (CORBA::Exception& e) {
          logger->log(WARNING, "Nao foi possivel renovar a credencial!");
        /* Passar para o logger. */
          e._print_stack_trace(cout);
          if (_leaseExpiredCallback) {
            _leaseExpiredCallback->expired();
          } else {
            logger->log(INFO, "Nenhuma callback registrada.");
          }
        }
        logger->dedent(INFO, "Openbus::RenewLeaseCallback::callback() END");
      }
    }
  #endif

  void Openbus::commandLineParse() {
    timeRenewingFixe = false;
    for (short idx = 1; idx < _argc; idx++) {
      if (!strcmp(_argv[idx], "-OpenbusHost")) {
        idx++;
        hostBus = _argv[idx];
      } else if (!strcmp(_argv[idx], "-OpenbusPort")) {
        idx++;
        portBus = atoi(_argv[idx]);
      } else if (!strcmp(_argv[idx], "-OpenbusTimeRenewing")) {
        timeRenewing = (unsigned int) atoi(_argv[++idx]);
        timeRenewingFixe = true;
      } else if (!strcmp(_argv[idx], "-OpenbusDebug")) {
        idx++;
        char *debugLevelStr = _argv[idx];
        if (!strcmp(debugLevelStr, "ALL")) {
          debugLevel = ALL;
        } else if (!strcmp(debugLevelStr, "ERROR")) {
          debugLevel = ERROR;
        } else if (!strcmp(debugLevelStr, "INFO")) {
          debugLevel = INFO;
        } else if (!strcmp(debugLevelStr, "WARNING")) {
          debugLevel = WARNING;
        }
      } else if (!strcmp(_argv[idx], "-OpenbusDebugFile")) {
        idx++;
        debugFile = _argv[idx];
      } else if (!strcmp(_argv[idx], "-OpenbusValidationPolicy")) {
        idx++;
        char* credentialValidationPolicyStr = _argv[idx];
        if (!strcmp(credentialValidationPolicyStr, "NONE")) {
          credentialValidationPolicy = interceptors::NONE;
        } else if (!strcmp(credentialValidationPolicyStr, "ALWAYS")) { 
          credentialValidationPolicy = interceptors::ALWAYS;
        } else if (!strcmp(credentialValidationPolicyStr, "CACHED")) { 
          credentialValidationPolicy = interceptors::CACHED;
        }
      } else if (!strcmp(_argv[idx], "-OpenbusFTConfigFilename")) {
        faultToleranceEnable = true;
        idx++;
        FTConfigFilename = _argv[idx];
      } else if (!strcmp(_argv[idx], "-OpenbusValidationTime")) {
          ini->getServerInterceptor()->setValidationTime(
            (unsigned long) atoi(_argv[++idx]));
      } 
    }
  }

  void Openbus::createORB() {
    if (CORBA::is_nil(orb)) {
      logger->log(INFO, "Criando ORB...");
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
    debugLevel = OFF;
    connectionState = DISCONNECTED;
    credential = 0;
  #if (!OPENBUS_ORBIX && MULTITHREAD)    
    MICOMT::Thread::set_specific(threadKey, 0);
  #endif
    lease = 0;
    iAccessControlService = access_control_service::IAccessControlService::_nil();
    iRegistryService = 0;
    iLeaseProvider = access_control_service::ILeaseProvider::_nil();
    iComponentAccessControlService = scs::core::IComponent::_nil();
  }

  Openbus::Openbus() {
    luaState = lua_open();
    luaL_openlibs(luaState);
  #ifdef OPENBUS_ORBIX
    luaopen_IOR(luaState);
  #endif
  #if (!OPENBUS_ORBIX && MULTITHREAD)
    MICOMT::Thread::create_key(threadKey, 0);
  #endif
    newState();
    credentialValidationPolicy = openbus::interceptors::ALWAYS;
    registerInterceptors();
    initialize();
  }

  Openbus::~Openbus() {
    logger->log(INFO, "Openbus::~Openbus() BEGIN");
    logger->indent();
    if (bus) {
      logger->log(INFO, "Deletando lista de métodos interceptáveis...");
      MethodsNotInterceptable::iterator iter = methodsNotInterceptable.begin();
      while (iter != methodsNotInterceptable.end()) {
        MethodSet *methods = iter->second;
        methodsNotInterceptable.erase(iter++);
        methods->clear();
        delete methods;
      }
      if (!CORBA::is_nil(orb)) {
      #ifndef OPENBUS_ORBIX 
        #ifndef MULTITHREAD
          logger->log(INFO, "Removendo callback de renovação de credencial...");
          orb->dispatcher()->remove(&renewLeaseCallback, CORBA::Dispatcher::Timer);
        #endif
      #endif
      }
      mutex.lock();
      bus = 0;
      mutex.unlock();
      #if (OPENBUS_ORBIX || (!OPENBUS_ORBIX && MULTITHREAD))
        if (renewLeaseThread) {
          bool b;
          mutex.lock();
          b = renewLeaseThread->runningLeaseExpiredCallback;
          mutex.unlock();
          if (!b) {
            Openbus::logger->log(INFO, "Aguardando término da renewLeaseThread...");
          /* Por que o meu wait não fica bloqueado quando o LeaseExpiredCallback() chamda 
          *  disconnect() ?!
          */
            renewLeaseThread->stop();
            #ifdef OPENBUS_ORBIX
              renewLeaseIT_Thread.join();
            #else
              renewLeaseThread->wait();
            #endif
            Openbus::logger->log(INFO, "delete renewLeaseThread.");
            delete renewLeaseThread;
            renewLeaseThread = 0;
          } else {
            Openbus::logger->log(WARNING, 
              "Não é possível finalizar RenewLeaseThread porque estamos dentro de \
              LeaseExpiredCallback");
          }
        }
      #endif
      #if (!OPENBUS_ORBIX && MULTITHREAD)
        MICOMT::Thread::delete_key(threadKey);
      #endif
    }
    logger->dedent(INFO, "Openbus::~Openbus() END");
  }

  void Openbus::terminationHandlerCallback(long signalType) {
    logger->log(INFO, "Openbus::terminationHandlerCallback() BEGIN");
    logger->indent();
  /* Prevenção caso o usuário consiga executar o _termination handler_ mais de uma vez */
    if (bus) {
      try {
        if (bus->isConnected()) {
          Openbus::logger->log(INFO, "Desconectando usuário do barramento.");
          bus->disconnect();
        }
      } catch(CORBA::Exception& e) {
        logger->log(WARNING, 
          "Não foi possível se desconectar corretamente do barramento."); 
      }
      bus->stop();
      bus->finish();
      delete bus;
    }
    logger->dedent(INFO, "Openbus::terminationHandlerCallback() END");      
  }

  Openbus* Openbus::getInstance() {
    logger = Logger::getInstance();
    mutex.trylock();
    if (!bus) {
      bus = new Openbus();
    }
    mutex.unlock();
    return bus;
  }

  void Openbus::init(
    int argc,
    char** argv)
  {
    stringstream msgLog;
    _argc = argc;
    _argv = argv;
    commandLineParse();
    logger->setOutput(debugFile);
    logger->setLevel(debugLevel);
    logger->log(INFO, "Openbus::init() BEGIN");
    logger->indent();
    msgLog << "hostBus: " << hostBus;
    logger->log(INFO, msgLog.str());
    msgLog.str("");
    msgLog << "portBus: " << portBus;
    logger->log(INFO, msgLog.str());
    msgLog.str("");
    msgLog << "credentialValidationPolicy: " << credentialValidationPolicy;
    logger->log(INFO, msgLog.str());
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
      #ifdef MULTITHREAD
        if (!runThread) {
          Openbus::logger->log(INFO, "Criando uma RunThread.");
          runThread = new RunThread();
          runThread->start();
        }
      #endif
    #endif
    logger->dedent(INFO, "Openbus::init() END");
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
    logger->log(INFO, "Openbus::isConnected() BEGIN");
    logger->indent();
    if (connectionState == CONNECTED) {
      logger->log(INFO, "Está conectado.");
      logger->dedent(INFO, "Openbus::isConnected() END");
      return true;
    }
    logger->log(INFO, "NÃO está conectado.");
    logger->dedent(INFO, "Openbus::isConnected() END");
    return false;
  }

  CORBA::ORB* Openbus::getORB() {
    return orb;
  }

  PortableServer::POA* Openbus::getRootPOA() {
    logger->log(INFO, "Openbus::getRootPOA() BEGIN");
    logger->indent();
    if (CORBA::is_nil(poa) && !CORBA::is_nil(orb)) {
      logger->log(INFO, "Criando POA...");
      CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
      poa = PortableServer::POA::_narrow(poa_obj);
      poa_manager = poa->the_POAManager();
      poa_manager->activate();
    }
    logger->dedent(INFO, "Openbus::getRootPOA() END");
    return poa;
  }

  access_control_service::Credential_var Openbus::getInterceptedCredential() {
    return ini->getServerInterceptor()->getCredential();
  }

  access_control_service::IAccessControlService* Openbus::getAccessControlService() {
    return iAccessControlService;
  }

  registry_service::IRegistryService* Openbus::getRegistryService() {
    logger->log(INFO, "Openbus::getRegistryService() BEGIN");
    logger->indent();
    scs::core::IComponent_var iComponentRegistryService;
    if ((scs::core::IComponent*) iComponentAccessControlService) {
      CORBA::Object_var objRecep = iComponentAccessControlService->getFacetByName("IReceptacles");
      scs::core::IReceptacles_var recep = scs::core::IReceptacles::_narrow(objRecep);
      try {
        scs::core::ConnectionDescriptions_var conns =
          recep->getConnections("RegistryServiceReceptacle");
        if (conns->length() > 0) {
          logger->log(INFO, "Adquirindo RegistryService...");
          CORBA::Object_var objref = conns[(CORBA::ULong) 0].objref;
          iComponentRegistryService = scs::core::IComponent::_narrow(objref);
          objref = iComponentRegistryService->getFacetByName("IRegistryService_v1_05");
          iRegistryService = registry_service::IRegistryService::_narrow(objref);
        }
      } catch (CORBA::Exception& e) {
        logger->log(ERROR, "Não foi possível obter o serviço de registro.");
        // TODO: necessário fazer um throw?
      }
    }
    logger->dedent(INFO, "Openbus::getRegistryService() END");
    return iRegistryService;
  } 

  access_control_service::Credential* Openbus::getCredential() {
  #if (!OPENBUS_ORBIX && MULTITHREAD)
    access_control_service::Credential* threadCredential = 
      (access_control_service::Credential*) MICOMT::Thread::get_specific(threadKey);
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

  void Openbus::setThreadCredential(access_control_service::Credential* credential) {
  #if (!OPENBUS_ORBIX && MULTITHREAD)
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
    logger->log(INFO, "Openbus::createProxyToIAccessControlService() BEGIN");
    logger->indent();
  /* Pode ser chamado de dentro da RenewLeaseThread. */
    mutex.trylock();
    std::stringstream corbalocIC;
    corbalocIC << "corbaloc::" << hostBus << ":" << portBus << "/openbus_v1_05";
    logger->log(INFO, "corbaloc IC do ACS: " + corbalocIC.str());
    CORBA::Object_var objIC = 
      orb->string_to_object(corbalocIC.str().c_str());
    iComponentAccessControlService = scs::core::IComponent::_narrow(objIC);
    CORBA::Object_var objLP = iComponentAccessControlService->getFacet(
      "IDL:tecgraf/openbus/core/v1_05/access_control_service/ILeaseProvider:1.0");
    iLeaseProvider = access_control_service::ILeaseProvider::_narrow(objLP);
    CORBA::Object_var objACS = iComponentAccessControlService->getFacet(
      "IDL:tecgraf/openbus/core/v1_05/access_control_service/IAccessControlService:1.0");
    iAccessControlService = 
      access_control_service::IAccessControlService::_narrow(objACS);
    CORBA::Object_var objFT = iComponentAccessControlService->getFacet(
      "IDL:tecgraf/openbus/fault_tolerance/v1_05/IFaultTolerantService:1.0");
    iFaultTolerantService = IFaultTolerantService::_narrow(objFT);
    mutex.unlock();
    logger->dedent(INFO, "Openbus::createProxyToIAccessControlService() END");
  }

  registry_service::IRegistryService* Openbus::connect(
    const char* user,
    const char* password)
    throw (CORBA::SystemException, LOGIN_FAILURE)
  {
    logger->log(INFO, "Openbus::connect() BEGIN");
    logger->indent();
    if (connectionState == DISCONNECTED) {
      try {
        std::stringstream portMSG;
        std::stringstream userMSG;
        std::stringstream passwordMSG;
        std::stringstream orbMSG;
        logger->log(INFO, "host = " + hostBus);
        portMSG << "port = " << portBus; 
        logger->log(INFO, portMSG.str());
        userMSG << "username = " << user;
        logger->log(INFO, userMSG.str());
        passwordMSG<< "password = " << password;
        logger->log(INFO, passwordMSG.str());
        orbMSG<< "orb = " << &orb;
        logger->log(INFO, orbMSG.str());
        if (CORBA::is_nil(iAccessControlService)) {
          createProxyToIAccessControlService();
        }

        stringstream iACSMSG;
        iACSMSG << "iAccessControlService = " << &iAccessControlService; 
        logger->log(INFO, iACSMSG.str());
        mutex.lock();
        if (!iAccessControlService->loginByPassword(user, password, credential,
          lease))
        {
          mutex.unlock();
          logger->log(ERROR, "Throwing LOGIN_FAILURE...");
          logger->dedent(INFO, "Openbus::connect() END");
          throw LOGIN_FAILURE();
        } else {

          stringstream msg;
          msg << "Associando credencial " << credential << " ao ORB.";
          logger->log(INFO, msg.str());
          connectionState = CONNECTED;
          setThreadCredential(credential);
          if (!timeRenewingFixe) {
            timeRenewing = lease/3;
          }
          setRegistryService();
          #ifdef OPENBUS_ORBIX
            if (!renewLeaseThread) {
              renewLeaseThread = new RenewLeaseThread();
              renewLeaseIT_Thread = IT_ThreadFactory::smf_start(
                *renewLeaseThread, 
                IT_ThreadFactory::attached, 
                0);
            }
          #else
            #ifdef MULTITHREAD
              if (!renewLeaseThread) {
                Openbus::logger->log(INFO, "Criando uma RenewLeaseThread.");
                renewLeaseThread = new RenewLeaseThread();
                renewLeaseThread->start();
              }
            #else
              Openbus::logger->log(INFO, "Registrando evento de renovação da credencial...");
              orb->dispatcher()->tm_event(&renewLeaseCallback, timeRenewing*1000);
            #endif
          #endif
          mutex.unlock();
          logger->dedent(INFO, "Openbus::connect() END");
          return iRegistryService;

        }
      } catch (const CORBA::SystemException& systemException) {
        logger->log(ERROR, "Throwing CORBA::SystemException...");
        logger->dedent(INFO, "Openbus::connect() END");
        throw;
      }
    } else {
      logger->log(INFO, "Ja ha uma conexão ativa.");
      logger->dedent(INFO, "Openbus::connect() END");
      throw LOGIN_FAILURE();
    }
  }

  registry_service::IRegistryService* Openbus::connect(
    const char* entity,
    const char* privateKeyFilename,
    const char* ACSCertificateFilename)
    throw (CORBA::SystemException, LOGIN_FAILURE, SECURITY_EXCEPTION)
  {
    logger->log(INFO, "Openbus::connect() BEGIN");
    logger->indent();
    if (connectionState == DISCONNECTED) {
      try {
        std::stringstream portMSG;
        std::stringstream entityMSG;
        std::stringstream privateKeyFilenameMSG;
        std::stringstream ACSCertificateFilenameMSG;
        logger->log(INFO, "host = " + hostBus);
        portMSG << "port = " << portBus; 
        logger->log(INFO, portMSG.str());
        entityMSG << "entity = " << entity;
        logger->log(INFO, entityMSG.str());
        privateKeyFilenameMSG << "privateKeyFilename = " << privateKeyFilename;
        logger->log(INFO, privateKeyFilenameMSG.str());
        ACSCertificateFilenameMSG << "ACSCertificateFilename = " << 
          ACSCertificateFilename;
        logger->log(INFO, ACSCertificateFilenameMSG.str());
        if (CORBA::is_nil(iAccessControlService)) {
          createProxyToIAccessControlService();
        }

      /* Requisição de um "desafio" que somente poderá ser decifrado através
      *  da chave privada da entidade reconhecida pelo barramento.
      */
        OctetSeq_var octetSeq =
          iAccessControlService->getChallenge(entity);
        if (octetSeq->length() == 0) {
          logger->log(ERROR, "Throwing SECURITY_EXCEPTION...");
          logger->dedent(INFO, "Openbus::connect() END");
          throw SECURITY_EXCEPTION(
            "O ACS não encontrou o certificado do serviço.");
        }
        unsigned char* challenge = octetSeq->get_buffer();

      /* Leitura da chave privada da entidade. */
        BIO* bio = BIO_new( BIO_s_file() );
        if (BIO_read_filename( bio, privateKeyFilename ) <= 0) {
          stringstream filename;
          filename << "Não foi possível abrir o arquivo: " << 
            privateKeyFilename;
          logger->log(WARNING, filename.str());
          logger->log(ERROR, "Throwing SECURITY_EXCEPTION...");
          logger->dedent(INFO, "Openbus::connect() END");
          throw SECURITY_EXCEPTION(
            "Não foi possível abrir o arquivo que armazena a chave privada.");
        }
        EVP_PKEY* EntityPrivateKey  = PEM_read_bio_PrivateKey( bio, NULL, NULL, 0 );
        if (EntityPrivateKey == 0) {
          logger->log(WARNING, "Não foi possível obter a chave privada da entidade.");
          logger->log(ERROR, "Throwing SECURITY_EXCEPTION...");
          logger->dedent(INFO, "Openbus::connect() END");
          EVP_PKEY_free(EntityPrivateKey);
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
          stringstream filename;
          filename << "Não foi possível abrir o arquivo: " << 
            ACSCertificateFilename;
          logger->log(WARNING, filename.str());
          logger->log(ERROR, "Throwing SECURITY_EXCEPTION...");
          logger->dedent(INFO, "Openbus::connect() END");
          throw SECURITY_EXCEPTION(
            "Não foi possível abrir o arquivo que armazena o certificado ACS.");
        }
      
        EVP_PKEY_free(EntityPrivateKey);
      
        X509* x509 = d2i_X509_bio(bio, 0);

      /* Obtenção da chave pública do ACS. */
        EVP_PKEY* ACSPublicKey = X509_get_pubkey(x509);
        if (ACSPublicKey == 0) {
          logger->log(WARNING, "Não foi possível obter a chave pública do ACS.");
          logger->log(ERROR, "Throwing SECURITY_EXCEPTION...");
          logger->dedent(INFO, "Openbus::connect() END");
          free(challengePlainText);
          EVP_PKEY_free(ACSPublicKey);
          X509_free(x509);
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

        OctetSeq_var answerOctetSeq = new OctetSeq(
          (CORBA::ULong) RSAModulusSize, (CORBA::ULong) RSAModulusSize,
          (CORBA::Octet*)answer, 0);

        EVP_PKEY_free(ACSPublicKey);
        X509_free(x509);

        stringstream iACSMSG;
        iACSMSG << "iAccessControlService = " << &iAccessControlService; 
        logger->log(INFO, iACSMSG.str());
        mutex.lock();
        if (!iAccessControlService->loginByCertificate(entity, answerOctetSeq,
          credential, lease))
        {
          free(answer);
          mutex.unlock();
          throw LOGIN_FAILURE();
        } else {
          free(answer);
          stringstream msg;
          msg << "Associando credencial " << credential << " ao ORB."; 
          logger->log(INFO, msg.str());
          connectionState = CONNECTED;
          setThreadCredential(credential);
          if (!timeRenewingFixe) {
            timeRenewing = lease/3;
          }
          setRegistryService();
          #ifdef OPENBUS_ORBIX
            if (!renewLeaseThread) {
              renewLeaseThread = new RenewLeaseThread();
              renewLeaseIT_Thread = IT_ThreadFactory::smf_start(
                *renewLeaseThread, 
                IT_ThreadFactory::attached, 
                0);
            }
          #else
            #ifdef MULTITHREAD
              if (!renewLeaseThread) {
                Openbus::logger->log(INFO, "Criando uma RenewLeaseThread.");
                renewLeaseThread = new RenewLeaseThread();
                renewLeaseThread->start();
              }
            #else
              Openbus::logger->log(INFO, "Registrando evento de renovação da credencial...");
              orb->dispatcher()->tm_event(&renewLeaseCallback, timeRenewing*1000);
            #endif
          #endif
          mutex.unlock();
          logger->dedent(INFO, "Openbus::connect() END");
          return iRegistryService;
        }
      } catch (const CORBA::SystemException& systemException) {
        logger->log(ERROR, "Throwing CORBA::SystemException...");
        logger->dedent(INFO, "Openbus::connect() END");
        throw;
      }
    } else {
      logger->log(INFO, "Ja ha uma conexão ativa.");
      logger->dedent(INFO, "Openbus::connect() END");
      throw LOGIN_FAILURE();
    }
  }

  bool Openbus::disconnect() {
    logger->log(INFO, "Openbus::disconnect() BEGIN");
    logger->indent();
    mutex.lock();
    if (connectionState == CONNECTED) {
      mutex.unlock();
    #ifndef OPENBUS_ORBIX
      #ifndef MULTITHREAD
        logger->log(INFO, "Removendo callback de renovação de credencial...");
        orb->dispatcher()->remove(&renewLeaseCallback, CORBA::Dispatcher::Timer);
      #endif
    #endif
      if (iRegistryService) {
        delete iRegistryService;
      }
      bool status;
      try {
        status = iAccessControlService->logout(*credential);
        if (!status) {
          logger->log(WARNING, "Não foi possível realizar logout.");
        }
      } catch (CORBA::Exception& e) {
        logger->log(WARNING, "Não foi possível realizar logout.");
      }
      #if 0
        if (credential) {
          delete credential;
        }
      #endif
      mutex.lock();
      newState();
      mutex.unlock();
      #if (OPENBUS_ORBIX || (!OPENBUS_ORBIX && MULTITHREAD))
        if (renewLeaseThread) {
          bool b;
          mutex.lock();
          b = renewLeaseThread->runningLeaseExpiredCallback;
          mutex.unlock();
          if (!b) {
            Openbus::logger->log(INFO, "Aguardando término da renewLeaseThread...");
          /* Por que o meu wait não fica bloqueado quando o LeaseExpiredCallback() chama 
          *  disconnect() ?!
          */
            renewLeaseThread->stop();
            #ifdef OPENBUS_ORBIX
              renewLeaseIT_Thread.join();
            #else
              renewLeaseThread->wait();
            #endif
            Openbus::logger->log(INFO, "delete renewLeaseThread.");
            delete renewLeaseThread;
            renewLeaseThread = 0;
          } else {
            Openbus::logger->log(WARNING, 
              "Não é possível finalizar RenewLeaseThread porque estamos dentro de \
              LeaseExpiredCallback");
          }
        }
      #endif
      logger->dedent(INFO, "Openbus::disconnect() END");
      return status;
    } else {
      logger->log(INFO, "Não há conexão a ser desfeita.");
      logger->dedent(INFO, "Openbus::disconnect() END");
      mutex.unlock();
      return false;
    }
  }

  void Openbus::run() {
    Openbus::logger->log(INFO, "Openbus::run()");
    #if (OPENBUS_ORBIX)
      orb->run();
    #else
      #if (MULTITHREAD)
      /* Já temos uma runThread ? */
        if (!runThread) {
          Openbus::logger->log(INFO, "Criando uma RunThread.");
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
    Openbus::logger->log(INFO, "Openbus::stop() BEGIN");
    #if (!OPENBUS_ORBIX && !MULTITHREAD)
      orbRunning = false;
    #endif
    Openbus::logger->log(INFO, "Openbus::stop() END");
  }

  void Openbus::finish(bool force) {
    logger->log(INFO, "Openbus::finish() BEGIN");
    logger->indent();
    stringstream msg;
    msg << "Desligando orb com force = " << force; 
    logger->log(INFO, msg.str());
    orb->shutdown(force);
    orb->destroy();
    orb = CORBA::ORB::_nil();
    logger->dedent(INFO, "Openbus::finish() END");
  }

  void Openbus::setInterceptable(string interfaceRepID, string method,
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

  bool Openbus::isInterceptable(string interfaceRepID, string method)
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
