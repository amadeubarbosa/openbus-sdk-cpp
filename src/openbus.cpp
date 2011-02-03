/*
** openbus.cpp
*/

#include <openbus/version.h>
#include "openbus/openbus.h"

#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef OPENBUS_ORBIX
  #include <omg/orb.hh>
  #include <it_ts/thread.h>
  #include <stubs/orbix/scs.hh>
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

//Logger* Openbus::logger  = 0;
//char* Openbus::debugFile = 0;

// char* Openbus::FTConfigFilename = 0;
// bool Openbus::orbRunning = true;
// interceptors::orb_initializer_impl* Openbus::ini = 0;

// CORBA::ORB_var Openbus::orb = CORBA::ORB::_nil();

// PortableServer::POA_var Openbus::poa = PortableServer::POA::_nil();
// lua_State* Openbus::luaState = 0;
// Openbus* Openbus::bus = 0;
// Openbus::LeaseExpiredCallback* Openbus::_leaseExpiredCallback = 0;

// #ifdef OPENBUS_ORBIX
// IT_Mutex Openbus::mutex;
// Openbus::RenewLeaseThread* Openbus::renewLeaseThread = 0;
// #else
// MICOMT::Mutex Openbus::mutex;
// #ifdef MULTITHREAD
// Openbus::RunThread* Openbus::runThread = 0;
// Openbus::RenewLeaseThread* Openbus::renewLeaseThread = 0;
// #endif
// #endif

namespace idl_namespace = tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE;
namespace fault_tolerance = tecgraf::openbus::fault_tolerance;

#if (OPENBUS_ORBIX || (!OPENBUS_ORBIX && MULTITHREAD))
Openbus::RenewLeaseThread::RenewLeaseThread()
{
  runningLeaseExpiredCallback = false;
  sigINT = false;
}
    
void Openbus::RenewLeaseThread::sleep(unsigned short time)
{
  unsigned short count = 0;
  for (;count < time; count++)
  {
    mutex.lock();
    if (sigINT)
    {
      sigINT = false;
      mutex.unlock();
      break;
    }
    else
    {
      mutex.unlock();
    }
#ifdef OPENBUS_ORBIX
    IT_CurrentThread::sleep(1000);
#else
    ::sleep(1);
#endif
  }
}
    
void Openbus::RenewLeaseThread::stop()
{
  mutex.lock();
  sigINT = true;
  mutex.unlock();
}
    
#ifdef OPENBUS_ORBIX
void* Openbus::RenewLeaseThread::run()
{
#else
void Openbus::RenewLeaseThread::_run(void*)
{
#endif
  unsigned int timeRenewing = 1;
  std::stringstream msg;
  bool tryExec_LeaseExpiredCallback = false;
  while (true)
  {
    //logger->log(INFO, "Openbus::RenewLeaseThread::run() BEGIN");
    msg << "sleep [" << timeRenewing <<"]s ...";
    //logger->log(INFO, msg.str());
    msg.str("");
    this->sleep(timeRenewing);

    mutex.lock();
    if (bus)
    {
      timeRenewing = bus->timeRenewing;
      if (bus->connectionState != orb_state::CONNECTED)
      {
        break;
      }
      else
      {
        //logger->log(INFO, "Renovando credencial...");
        try
        {
          bool status = bus->iLeaseProvider->renewLease(*bus->credential
                                                        , bus->lease);
          if (!bus->timeRenewingFixe)
          {
            bus->timeRenewing = bus->lease/3;
          }
          msg << "Próximo intervalo de renovação: " << bus->timeRenewing << "s";
          //logger->log(INFO, msg.str());
          msg.str("");
          if (!status)
          {
            logger->log(WARNING, "Não foi possível renovar a credencial!");
            tryExec_LeaseExpiredCallback = true;
            break;
          }
          else
          {
            logger->log(INFO, "Credencial renovada!");
          }
        }
        catch (CORBA::Exception& e)
        {
          logger->log(WARNING, "Não foi possível renovar a credencial!");
          tryExec_LeaseExpiredCallback = true;
          break;
        }
        //Openbus::logger->log(INFO, "Atualizando intervalo de tempo de na RewnewLeaseThread.");
        timeRenewing = bus->timeRenewing;
        mutex.unlock();
        continue; 
      }
    }
    mutex.unlock();
    break;
  } 
  if (tryExec_LeaseExpiredCallback)
  {
    if (_leaseExpiredCallback)
    {
      runningLeaseExpiredCallback = true;
      mutex.unlock();
      _leaseExpiredCallback->expired();
      //Openbus::logger->log(INFO, "LeaseExpiredCallback executada.");
    }
    else
    {
      //logger->log(INFO, "Nenhuma callback registrada.");
    }       
  } 
  runningLeaseExpiredCallback = false;
  mutex.unlock();
  //logger->log(INFO, "Openbus::RenewLeaseThread::run() END");
}
    
#ifdef MULTITHREAD
void Openbus::RunThread::_run(void*)
{
  //logger->log(INFO, "[RunThread Iniciada]");
  orb->run();
  //logger->log(INFO, "[RunThread Encerrada]");
}
#endif
#else
// Openbus::RenewLeaseCallback::RenewLeaseCallback(openbus::orb_state& orb_state)
//   : orb_state(&orb_state)
// {
// }

// void Openbus::RenewLeaseCallback::callback(CORBA::Dispatcher* dispatcher 
//                                            , Event event)
// {
//   //logger->log(INFO, "Openbus::RenewLeaseCallback::callback() BEGIN");
//   //logger->indent();
//   if (orb_state->connectionState == orb_state::CONNECTED)
//   {
//     //logger->log(INFO, "Renovando credencial...");
//     try
//     {
//       bool status = orb_state->iLeaseProvider->renewLease(*orb_state->credential, orb_state->lease);
//       if (!orb_state->timeRenewingFixe)
//       {
//         orb_state->timeRenewing = orb_state->lease/3;
//       }
//       std::stringstream msg;
//       msg << "Proximo intervalo de renovacao: " << orb_state->timeRenewing << "s";
//       //logger->log(INFO, msg.str());
//       if (!status)
//       {
//         //logger->log(WARNING, "Nao foi possivel renovar a credencial!");
//         //logger->log(WARNING, "ACS retornou credencial inválida.");
//         if (orb_state->_leaseExpiredCallback)
//         {
//           orb_state->_leaseExpiredCallback->expired();
//         }
//         else
//         {
//           //logger->log(INFO, "Nenhuma callback registrada.");
//         }
//       }
//       else
//       {
//         //logger->log(INFO, "Credencial renovada!");
//         dispatcher->tm_event(this, orb_state->timeRenewing*1000);
//       }
//     }
//     catch (CORBA::Exception& e)
//     {
//       //logger->log(WARNING, "Nao foi possivel renovar a credencial!");
//       /* Passar para o logger. */
//       e._print_stack_trace(std::cout);
//       if (orb_state->_leaseExpiredCallback)
//       {
//         orb_state->_leaseExpiredCallback->expired();
//       }
//       else
//       {
//         //logger->log(INFO, "Nenhuma callback registrada.");
//       }
//     }
//     //logger->dedent(INFO, "Openbus::RenewLeaseCallback::callback() END");
//   }
// }
#endif

void orb_state::commandLineParse(int _argc, char** _argv)
{
  timeRenewingFixe = false;
  for (short idx = 1; idx < _argc; idx++)
  {
    if (!strcmp(_argv[idx], "-OpenbusHost"))
    {
      idx++;
      hostBus = _argv[idx];
    }
    else if (!strcmp(_argv[idx], "-OpenbusPort"))
    {
      idx++;
      portBus = atoi(_argv[idx]);
    }
    else if (!strcmp(_argv[idx], "-OpenbusTimeRenewing"))
    {
      timeRenewing = (unsigned int) atoi(_argv[++idx]);
      timeRenewingFixe = true;
    }
    else if (!strcmp(_argv[idx], "-OpenbusDebug"))
    {
      idx++;
      char *debugLevelStr = _argv[idx];
      // if (!strcmp(debugLevelStr, "ALL"))
      // {
      //   debugLevel = ALL;
      // }
      // else if (!strcmp(debugLevelStr, "ERROR"))
      // {
      //   debugLevel = ERROR;
      // }
      // else if (!strcmp(debugLevelStr, "INFO"))
      // {
      //   debugLevel = INFO;
      // }
      // else if (!strcmp(debugLevelStr, "WARNING"))
      // {
      //   debugLevel = WARNING;
      // }
    }
    else if (!strcmp(_argv[idx], "-OpenbusDebugFile"))
    {
      idx++;
      //debugFile = _argv[idx];
    }
    else if (!strcmp(_argv[idx], "-OpenbusValidationPolicy"))
    {
      idx++;
      char* credentialValidationPolicyStr = _argv[idx];
      if (!strcmp(credentialValidationPolicyStr, "NONE"))
      {
        credentialValidationPolicy = interceptors::NONE;
      }
      else if (!strcmp(credentialValidationPolicyStr, "ALWAYS"))
      { 
        credentialValidationPolicy = interceptors::ALWAYS;
      }
      else if (!strcmp(credentialValidationPolicyStr, "CACHED"))
      { 
        credentialValidationPolicy = interceptors::CACHED;
      }
    }
    else if (!strcmp(_argv[idx], "-OpenbusFTConfigFilename"))
    {
      faultToleranceEnable = true;
      idx++;
      FTConfigFilename = _argv[idx];
    }
    else if (!strcmp(_argv[idx], "-OpenbusValidationTime"))
    {
      ini->getServerInterceptor()->setValidationTime((unsigned long) atoi(_argv[++idx]));
    } 
  }
}

// void Openbus::terminationHandlerCallback(long signalType)
// {
//   //logger->log(INFO, "Openbus::terminationHandlerCallback() BEGIN");
//   //logger->indent();
//   /* Prevenção caso o usuário consiga executar o _termination handler_ mais de uma vez */
//   if (bus)
//   {
//     try
//     {
//       if (bus->isConnected())
//       {
//         //Openbus::logger->log(INFO, "Desconectando usuário do barramento.");
//         bus->disconnect();
//       }
//     }
//     catch(CORBA::Exception& e)
//     {
//       //logger->log(WARNING, "Não foi possível se desconectar corretamente do barramento."); 
//     }
//     bus->stop();
//     bus->finish();
//     delete bus;
//   }
//   //logger->dedent(INFO, "Openbus::terminationHandlerCallback() END");      
// }

Openbus::Openbus(int argc, char** argv)
{
  orb_state.reset(new openbus::orb_state(argc, argv, "localhost", 2089, interceptors::ALWAYS));
}

orb_state::orb_state(int argc, char**argv, const char* host
                     , unsigned short port, interceptors::CredentialValidationPolicy policy)
  : connectionState(DISCONNECTED)
  , credential(0)
{
  luaState = lua_open();
  luaL_openlibs(luaState);
#ifdef OPENBUS_ORBIX
  luaopen_IOR(luaState);
#endif

  commandLineParse(argc, argv);

  ini.reset(new interceptors::orb_initializer_impl(*this));
  PortableInterceptor::register_orb_initializer(ini.get());

#ifdef OPENBUS_ORBIX
  orb = CORBA::ORB_init(argc, argv, "tecgraf.openbus");
#else
  orb = CORBA::ORB_init(argc, argv);    
#endif
  CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
  poa = PortableServer::POA::_narrow(poa_obj);
  poa_manager = poa->the_POAManager();
  poa_manager->activate();

  componentBuilder.reset(new scs::core::ComponentBuilder(orb, poa));
}

Openbus::Openbus(int argc, char** argv, char* host
                 , unsigned short port, interceptors::CredentialValidationPolicy policy)
{
  orb_state.reset(new openbus::orb_state(argc, argv, host, port, policy));
}

bool Openbus::isConnected()
{
  return orb_state->connectionState == orb_state::CONNECTED;
}

CORBA::ORB_var Openbus::getORB() const
{
  return orb_state->orb;
}

PortableServer::POA_var Openbus::getRootPOA() const
{
  return orb_state->getRootPOA();
}

PortableServer::POA_var orb_state::getRootPOA() const
{
  CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow(poa_obj);

  return poa;
}

scs::core::ComponentBuilder& Openbus::getComponentBuilder()
{
  return *orb_state->componentBuilder;
}

idl_namespace::access_control_service::Credential_var Openbus::getInterceptedCredential()
{
  return orb_state->ini->getServerInterceptor()->getCredential();
}

idl_namespace::access_control_service::IAccessControlService* Openbus::getAccessControlService()
{
  return orb_state->getAccessControlService();
}

idl_namespace::access_control_service::IAccessControlService* orb_state::getAccessControlService()
{
  return iAccessControlService;
}

idl_namespace::registry_service::IRegistryService_var Openbus::getRegistryService() const
{
  return orb_state->getRegistryService();
}

idl_namespace::registry_service::IRegistryService_var orb_state::getRegistryService() const
{
  scs::core::IComponent_var iComponentRegistryService;
  if (!CORBA::is_nil(iComponentAccessControlService))
  {
    CORBA::Object_var objRecep = iComponentAccessControlService->getFacetByName("IReceptacles");
    if(!CORBA::is_nil(objRecep))
    {
      scs::core::IReceptacles_var recep = scs::core::IReceptacles::_narrow(objRecep);
      if(!CORBA::is_nil(recep))
      {
        scs::core::ConnectionDescriptions_var conns =
          recep->getConnections("RegistryServiceReceptacle");
        if (conns->length() > 0)
        {
          CORBA::Object_var component_objref = conns[(CORBA::ULong) 0].objref;
          scs::core::IComponent_var registry_service_component
            = scs::core::IComponent::_narrow(component_objref);
          CORBA::Object_var registry_facet
            = registry_service_component->getFacetByName("IRegistryService_" OPENBUS_IDL_VERSION_STRING);
          if(!CORBA::is_nil(registry_facet))
            iRegistryService = idl_namespace::registry_service::IRegistryService::_narrow(registry_facet);
        }
      }
    }
  }

  return iRegistryService;
} 

boost::optional<idl_namespace::access_control_service::Credential> Openbus::getCredential() const
{
  return orb_state->getCredential();
}

boost::optional<idl_namespace::access_control_service::Credential> orb_state::getCredential() const
{
  if(credential)
    return *credential;
  else
    return boost::none;
}

interceptors::CredentialValidationPolicy orb_state::getCredentialValidationPolicy()
{
  return credentialValidationPolicy;
}

// void Openbus::setThreadCredential(idl_namespace::access_control_service::Credential* credential)
// {
//   this->credential = credential;
//   openbus::interceptors::client_interceptor::credential = credential;
// }

// void Openbus::setLeaseExpiredCallback(LeaseExpiredCallback* leaseExpiredCallback) 
// {
//   mutex.lock();
//   _leaseExpiredCallback = leaseExpiredCallback;
//   mutex.unlock();
// }

// void Openbus::removeLeaseExpiredCallback()
// {
//   mutex.lock();
//   _leaseExpiredCallback = 0;
//   mutex.unlock();
// }

void orb_state::createProxyToIAccessControlService()
{
  std::stringstream corbalocIC;
  corbalocIC << "corbaloc::" << hostBus << ":" << portBus << "/openbus_" OPENBUS_IDL_VERSION_STRING;
  std::cout << "CORBA LOC: " << corbalocIC.str() << std::endl;
  CORBA::Object_var objIC = 
    orb->string_to_object(corbalocIC.str().c_str());
  if(!CORBA::is_nil(objIC))
  {
    idl_namespace::access_control_service::ILeaseProvider_var iLeaseProvider;
    idl_namespace::access_control_service::IAccessControlService_var iAccessControlService;
    fault_tolerance:: OPENBUS_IDL_VERSION_NAMESPACE ::IFaultTolerantService_var iFaultTolerantService;

    scs::core::IComponent_var iComponentAccessControlService = scs::core::IComponent::_narrow(objIC);
    CORBA::Object_var objLP = iComponentAccessControlService->getFacet
      ("IDL:tecgraf/openbus/core/" OPENBUS_IDL_VERSION_STRING "/access_control_service/ILeaseProvider:1.0");
    if(!CORBA::is_nil(objLP))
    {
      iLeaseProvider = idl_namespace::access_control_service::ILeaseProvider::_narrow(objLP);
      if(CORBA::is_nil(iLeaseProvider))
        throw std::runtime_error("");
    }
    else
      throw std::runtime_error("");
    CORBA::Object_var objACS = iComponentAccessControlService->getFacet
      ("IDL:tecgraf/openbus/core/" OPENBUS_IDL_VERSION_STRING "/access_control_service/IAccessControlService:1.0");
    if(!CORBA::is_nil(objACS))
    {
      iAccessControlService = idl_namespace::access_control_service::IAccessControlService::_narrow(objACS);
      if(CORBA::is_nil(iAccessControlService))
        throw std::runtime_error("");
    }
    else
      throw std::runtime_error("");
    CORBA::Object_var objFT = iComponentAccessControlService->getFacet
      ("IDL:tecgraf/openbus/fault_tolerance/" OPENBUS_IDL_VERSION_STRING "/IFaultTolerantService:1.0");
    if(!CORBA::is_nil(objFT))
      iFaultTolerantService = fault_tolerance:: OPENBUS_IDL_VERSION_NAMESPACE ::IFaultTolerantService::_narrow(objFT);

    // Commit transaction
    {
      this->iLeaseProvider = iLeaseProvider;
      this->iAccessControlService = iAccessControlService;
      this->iFaultTolerantService = iFaultTolerantService;
      this->iComponentAccessControlService = iComponentAccessControlService;
    }
  }
  else
    throw std::runtime_error("");
}

idl_namespace::registry_service::IRegistryService* Openbus::connect(const char* user
                                                            , const char* password)
{
  return orb_state->connect(user, password);
}

idl_namespace::registry_service::IRegistryService* orb_state::connect(const char* user, const char* password)
{
  if (connectionState == DISCONNECTED)
  {
      assert (CORBA::is_nil(iAccessControlService));

      std::cout << "calling createProxyToIAccessControlService" << std::endl;
      createProxyToIAccessControlService();
      assert(!CORBA::is_nil(iAccessControlService));

      if (!iAccessControlService->loginByPassword(user, password, credential
                                                  , lease))
      {
        iRegistryService = idl_namespace::registry_service::IRegistryService::_nil();
        iLeaseProvider = idl_namespace::access_control_service::ILeaseProvider::_nil();
        iComponentAccessControlService = scs::core::IComponent::_nil();
        iAccessControlService = idl_namespace::access_control_service::IAccessControlService::_nil();
        throw login_error();
      }
      else
      {
        openbus::interceptors::client_interceptor::credential = credential;
        if (!timeRenewingFixe)
        {
          timeRenewing = lease/3;
        }

        if (iComponentAccessControlService)
        {
          CORBA::Object_var objRecep = iComponentAccessControlService->getFacetByName("IReceptacles");
          scs::core::IReceptacles_var recep = scs::core::IReceptacles::_narrow(objRecep);
          if(!CORBA::is_nil(recep))
          {
            scs::core::ConnectionDescriptions_var conns =
              recep->getConnections("RegistryServiceReceptacle");
            if (conns->length() > 0) 
            {
              CORBA::ULong zero = 0u;
              CORBA::Object_var objref = conns[zero].objref;
              scs::core::IComponent_var iComponentRegistryService = scs::core::IComponent::_narrow(objref);
              objref = iComponentRegistryService->getFacetByName("IRegistryService_" OPENBUS_IDL_VERSION_STRING);
              if(!CORBA::is_nil(objref))
              {
                iRegistryService = idl_namespace::registry_service::IRegistryService::_narrow(objref);
              }
              else
                std::cout << "IRegistryService facet is nil" << std::endl;
            }
            else
              std::cout << "There are zero connections" << std::endl;
          }
          else
            std::cout << "receptables is nil" << std::endl;
        }
        else
          std::cout << "iComponentAccessControlService NULL" << std::endl;
        

// #ifdef OPENBUS_ORBIX
//         if (!renewLeaseThread)
//         {
//           renewLeaseThread = new RenewLeaseThread();
//           renewLeaseIT_Thread = IT_ThreadFactory::smf_start(*renewLeaseThread
//                                                             , IT_ThreadFactory::attached
//                                                             , 0);
//         }
// #else
// #ifdef MULTITHREAD
//         if (!renewLeaseThread)
//         {
//           //Openbus::logger->log(INFO, "Criando uma RenewLeaseThread.");
//           renewLeaseThread = new RenewLeaseThread();
//           renewLeaseThread->start();
//         }
// #else
//         //Openbus::logger->log(INFO, "Registrando evento de renovação da credencial...");
//         orb->dispatcher()->tm_event(&renewLeaseCallback, timeRenewing*1000);
// #endif
// #endif
        connectionState = CONNECTED;
        return iRegistryService;
      }
      //    }
  }
  else
    throw login_error();

  return 0;
}

idl_namespace::registry_service::IRegistryService* Openbus::connect(const char* entity
                                                            , const char* privateKeyFilename
                                                            , const char* ACSCertificateFilename)
{
  std::cout << "Openbus::connect" << std::endl;
  return orb_state->connect(entity, privateKeyFilename, ACSCertificateFilename);
}

idl_namespace::registry_service::IRegistryService* orb_state::connect(const char* entity
                                                            , const char* privateKeyFilename
                                                            , const char* ACSCertificateFilename)
{
  assert(privateKeyFilename != 0);
  assert(ACSCertificateFilename != 0);
  assert(entity != 0);
  if (connectionState == DISCONNECTED) 
  {
      assert (CORBA::is_nil(iAccessControlService));
      if (CORBA::is_nil(iAccessControlService))
      {
        createProxyToIAccessControlService();
        assert (!CORBA::is_nil(iAccessControlService));
      }

      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
      /* Requisição de um "desafio" que somente poderá ser decifrado através
       *  da chave privada da entidade reconhecida pelo barramento.
       */
      CORBA::OctetSeq_var octetSeq =
        iAccessControlService->getChallenge(entity);
      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
      if(!octetSeq)
        throw std::runtime_error("");
      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
      if (octetSeq->length() == 0)
      {
        std::cout << "O ACS não encontrou o certificado do serviço." << std::endl;
        throw security_error("O ACS não encontrou o certificado do serviço.");
      }
      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
      unsigned char* challenge = octetSeq->get_buffer();

      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
      /* Leitura da chave privada da entidade. */
      FILE* fp = fopen(privateKeyFilename, "r");
      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
      if (fp == 0)
      {
        std::cout << "Não foi possível abrir o arquivo que armazena a chave privada." << std::endl;
        throw security_error("Não foi possível abrir o arquivo que armazena a chave privada.");
      }
      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
      EVP_PKEY* privateKey = PEM_read_PrivateKey(fp, 0, 0, 0);
      fclose(fp);
      if (privateKey == 0)
      {
        EVP_PKEY_free(privateKey);
        std::cout << "Não foi possível obter a chave privada da entidade." << std::endl;
        throw security_error("Não foi possível obter a chave privada da entidade.");
      }
      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
      int RSAModulusSize = EVP_PKEY_size(privateKey);

      /* Decifrando o desafio. */
      unsigned char* challengePlainText =
        static_cast<unsigned char*>(std::malloc(RSAModulusSize));
      memset(challengePlainText, ' ', RSAModulusSize);

      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
      RSA_private_decrypt(RSAModulusSize, challenge, challengePlainText
                          , privateKey->pkey.rsa, RSA_PKCS1_PADDING);

      /* Leitura do certificado do ACS. */
      FILE* certificateFile = fopen(ACSCertificateFilename, "rb");
      if (certificateFile == 0)
      {
      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
        free(challengePlainText);
        EVP_PKEY_free(privateKey);
        fclose(certificateFile);
        std::cout << "Não foi possível abrir o arquivo que armazena o certificado ACS." << std::endl;
        throw security_error("Não foi possível abrir o arquivo que armazena o certificado ACS.");
      }

      std::cout << __FILE__ ":" << __LINE__ << " - " << __FUNCTION__ << std::endl;
      EVP_PKEY_free(privateKey);

      X509* x509 = d2i_X509_fp(certificateFile, 0);
      fclose(certificateFile);
      
      /* Obtenção da chave pública do ACS. */
      EVP_PKEY* publicKey = X509_get_pubkey(x509);
      if (publicKey == 0)
      {
        free(challengePlainText);
        EVP_PKEY_free(publicKey);
        X509_free(x509);
        std::cout << "Não foi possível obter a chave pública do ACS." << std::endl;
        throw security_error("Não foi possível obter a chave pública do ACS.");
      }

      /* Reposta ao desafio, ou seja, cifra do desafio utilizando a chave
      *  pública do ACS.
      */
      unsigned char* answer = 
        static_cast<unsigned char*>(std::malloc(RSAModulusSize));
      RSA_public_encrypt(CHALLENGE_SIZE, challengePlainText, answer
                         , publicKey->pkey.rsa, RSA_PKCS1_PADDING);

      std::free(challengePlainText);

      CORBA::OctetSeq_var answerOctetSeq
        = new CORBA::OctetSeq((CORBA::ULong) RSAModulusSize, (CORBA::ULong) RSAModulusSize
                              , (CORBA::Octet*)answer, 0);

      EVP_PKEY_free(publicKey);
      X509_free(x509);
      
      if (!iAccessControlService->loginByCertificate(entity, answerOctetSeq
                                                     , credential, lease))
      {
        std::free(answer);
        std::cout << "login_error" << std::endl;
        throw login_error();
      }
      else
      {
        std::free(answer);
        connectionState = CONNECTED;
        openbus::interceptors::client_interceptor::credential = credential;
        if (!timeRenewingFixe)
        {
          timeRenewing = lease/3;
        }

        if (iComponentAccessControlService)
        {
          CORBA::Object_var objRecep = iComponentAccessControlService->getFacetByName("IReceptacles");
          scs::core::IReceptacles_var recep = scs::core::IReceptacles::_narrow(objRecep);
          if(!CORBA::is_nil(recep))
          {
            scs::core::ConnectionDescriptions_var conns =
              recep->getConnections("RegistryServiceReceptacle");
            if (conns->length() > 0) 
            {
              CORBA::ULong zero = 0u;
              CORBA::Object_var objref = conns[zero].objref;
              scs::core::IComponent_var iComponentRegistryService = scs::core::IComponent::_narrow(objref);
              objref = iComponentRegistryService->getFacetByName("IRegistryService_" OPENBUS_IDL_VERSION_STRING);
              if(!CORBA::is_nil(objref))
              {
                iRegistryService = idl_namespace::registry_service::IRegistryService::_narrow(objref);
              }
              else
                std::cout << "IRegistryService facet is nil" << std::endl;
            }
            else
              std::cout << "There are zero connections" << std::endl;
          }
          else
            std::cout << "receptables is nil" << std::endl;
        }
        else
          std::cout << "iComponentAccessControlService NULL" << std::endl;

// #ifdef OPENBUS_ORBIX
//         if (!renewLeaseThread)
//         {
//           renewLeaseThread = new RenewLeaseThread();
//           renewLeaseIT_Thread = IT_ThreadFactory::smf_start(*renewLeaseThread
//                                                             , IT_ThreadFactory::attached
//                                                             , 0);
//         }
// #else
// #ifdef MULTITHREAD
//         if (!renewLeaseThread)
//         {
//           renewLeaseThread = new RenewLeaseThread();
//           renewLeaseThread->start();
//         }
// #else
//         orb->dispatcher()->tm_event(&renewLeaseCallback, timeRenewing*1000);
// #endif
// #endif
        connectionState = CONNECTED;
        return iRegistryService;
       }
      //    }
  }
  else
  {
    std::cout << "loging_error " __FILE__ ":" << __LINE__ << std::endl;
    throw login_error();
  }
  return 0;
}

void Openbus::disconnect()
{
  return orb_state->disconnect();
}

void orb_state::disconnect()
{
  if (connectionState == CONNECTED)
  {
    iRegistryService = idl_namespace::registry_service::IRegistryService::_nil();
    iLeaseProvider = idl_namespace::access_control_service::ILeaseProvider::_nil();
    iComponentAccessControlService = scs::core::IComponent::_nil();
    bool status = iAccessControlService->logout(*credential);
    iAccessControlService = idl_namespace::access_control_service::IAccessControlService::_nil();
    if (!status)
    {
      //logger->log(WARNING, "Não foi possível realizar logout.");
    }
    openbus::interceptors::client_interceptor::credential = 0;
#if defined(OPENBUS_ORBIX) || (!defined(OPENBUS_ORBIX) && defined(MULTITHREAD))
    if (renewLeaseThread)
    {
      bool b;
      mutex.lock();
      b = renewLeaseThread->runningLeaseExpiredCallback;
      mutex.unlock();
      if (!b)
      {
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
        delete renewLeaseThread;
        renewLeaseThread = 0;
      }
      else
      {
      }
    }
#endif
    connectionState = DISCONNECTED;
  }
  else
    throw std::runtime_error("");
}

void Openbus::run()
{
// #if (!OPENBUS_ORBIX && MULTITHREAD)
//   /* Já temos uma runThread ? */
//   if (!runThread)
//   {
//     //Openbus::logger->log(INFO, "Criando uma RunThread.");
//     runThread = new RunThread();
//     runThread->start();
//   }
//   runThread->wait();
// #else
  // bool orbRunning = true;
  // while (orbRunning)
  // {
  //   if (orb_state->orb->work_pending())
  //   {
  //     orb_state->orb->perform_work();
  //   }
  // }
  orb_state->orb->run();
// #endif
}

// void Openbus::stop()
// {
//   //Openbus::logger->log(INFO, "Openbus::stop() BEGIN");
// #if (!OPENBUS_ORBIX && MULTITHREAD)
//   /* empty */
// #else  
//   orbRunning = false;
// #endif
//   //Openbus::logger->log(INFO, "Openbus::stop() END");
// }

// void Openbus::finish(bool force)
// {
//   //logger->log(INFO, "Openbus::finish() BEGIN");
//   //logger->indent();
//   std::stringstream msg;
//   msg << "Desligando orb com force = " << force; 
//   //logger->log(INFO, msg.str());
//   orb->shutdown(force);
//   orb->destroy();
//   orb = CORBA::ORB::_nil();
//   //logger->dedent(INFO, "Openbus::finish() END");
// }

// void Openbus::setInterceptable(std::string interfaceRepID, std::string method
//                                , bool isInterceptable)
// {
//   MethodSet *methods;
//   MethodsNotInterceptable::iterator iter;
//   /* Guarda apenas os métodos que não são interceptados */
//   if (isInterceptable)
//   {
//     iter = methodsNotInterceptable.find(interfaceRepID);
//     if (iter != methodsNotInterceptable.end())
//     {
//       methods = iter->second;
//       methods->erase(method);
//       if (methods->size() == 0)
//       {
//         methodsNotInterceptable.erase(iter);
//         delete methods;
//       }
//     }
//   }
//   else
//   {
//     iter = methodsNotInterceptable.find(interfaceRepID);
//     if (iter == methodsNotInterceptable.end())
//     {
//       methods = new MethodSet();
//       methodsNotInterceptable[interfaceRepID] = methods;
//     }
//     else
//       methods = iter->second;
//     methods->insert(method);
//   }
// }

bool orb_state::isInterceptable(std::string interfaceRepID, std::string method)
{
  std::cout << "Checking if " << interfaceRepID <<  "->" << method << " should be intercepted" << std::endl;
  // MethodsNotInterceptable::iterator iter = methodsNotInterceptable.find(interfaceRepID);
  // if (iter != methodsNotInterceptable.end())
  // {
  //   MethodSet *methods = iter->second;
  //   MethodSet::iterator method_iter = methods->find(method);
  //   return (method_iter == methods->end());
  // }
  return true;
}

// bool Openbus::isFaultToleranceEnable()
// {
//   return faultToleranceEnable;
// }

}
