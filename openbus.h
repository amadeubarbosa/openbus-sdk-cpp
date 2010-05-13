/**
* \mainpage API - Openbus C++
* \file openbus.h
*/

#ifndef OPENBUS_H_
#define OPENBUS_H_

#include "logger.h"
#include "openbus/util/Helper.h"

#include "openbus/interceptors/ORBInitializerImpl.h"
#include <ComponentBuilder.h>

#ifdef OPENBUS_MICO
  #include <CORBA.h>
  #include "stubs/mico/access_control_service.h"
  #include "stubs/mico/session_service.h"
  #include "stubs/mico/fault_tolerance.h"
#else
  #include <omg/orb.hh>
  #include <it_ts/thread.h>
  #include <it_ts/mutex.h>
  #include "stubs/orbix/access_control_service.hh"
  #include "stubs/orbix/session_service.hh"
  #include "stubs/orbix/fault_tolerance.hh"
#endif

#include <stdexcept>
#include <string.h>
#include <map>
#include <set>
#include <lua.hpp>

using namespace tecgraf::openbus::core::v1_05;
using namespace tecgraf::openbus::session_service::v1_05;
using namespace tecgraf::openbus::fault_tolerance::v1_05;
using namespace std;
using namespace logger;

/**
* \brief Stubs dos servi�os b�sicos.
*/
namespace tecgraf {
  namespace openbus {
    namespace core {
      namespace v1_05 {

      /**
      * \brief Stub do servi�o de acesso.
      */
        namespace access_control_service {

        /**
        * \class Credential
        * \brief Credencial de acesso ao barramento.
        */
        }
      }
    }
  }
}

/**
* \brief openbus
*/
namespace openbus {

  typedef set<string> MethodSet;
  typedef map<string, MethodSet*> IfaceMap;

/**
* \brief Falha no processo de login, ou seja, o par nome de usu�rio e senha n�o
* foi validado.
*/
  class LOGIN_FAILURE : public runtime_error {
    public:
      LOGIN_FAILURE(const string& msg = "") : runtime_error(msg) {}
  };

/**
* \brief Falha no mecanismo de autentica��o por certificado digital.
* Algumas poss�veis causas:
*  + N�o foi poss�vel obter o desafio.
*  + Falha na manipula��o de uma chave privada ou p�blica.
*  + Falha na manipula��o de um certificado.
*/
  class SECURITY_EXCEPTION : public runtime_error {
    public:
      SECURITY_EXCEPTION(const string& msg = "") : runtime_error(msg) {}
  };

/**
* \brief N�o h� conex�o estabelecida com nenhum barramento.
*/
  class NO_CONNECTED : public runtime_error {
    public:
      NO_CONNECTED(const string& msg = "") : runtime_error(msg) {}
  };

/**
* \brief N�o � poss�vel obter o servi�o de sess�o no barramento em uso.
*/
  class NO_SESSION_SERVICE : public runtime_error {
    public:
      NO_SESSION_SERVICE(const string& msg = "") : runtime_error(msg) {}
  };

  /**
  * \brief Representa um barramento.
  */
  class Openbus {
    public:
      class LeaseExpiredCallback;

    private:
      static char* debugFile;
      Level debugLevel;

  #ifndef OPENBUS_MICO
    /**
    * Mutex. 
    */
      static IT_Mutex mutex;
  #endif

    /**
    * A inst�ncia �nica do barramento.
    */
      static Openbus* bus;

    /**
    * M�quina virtual de Lua.
    */
      static lua_State* luaState;

    /**
    * Nome do arquivo de configuracao das replicas.
    */
      static char* FTConfigFilename;

    /**
    * Par�metro argc da linha de comando. 
    */
      int _argc;

    /**
    * Par�metro argv da linha de comando. 
    */
      char** _argv;
      
    /**
    * Ponteiro para o stub do servi�o de acesso.
    */
      access_control_service::IAccessControlService_var iAccessControlService;

    /**
    * Ponteiro para o stub do servi�o de registro.
    */
      registry_service::IRegistryService* iRegistryService;

    /**
    * Ponteiro para o stub do servi�o de sess�o.
    */
      ISessionService* iSessionService;

    /**
    * Ponteiro para a faceta ILeaseProvider. 
    */
      access_control_service::ILeaseProvider_var iLeaseProvider;

    /**
    * Ponteiro para o IComponent do servico de acesso. 
    */
      scs::core::IComponent_var iComponentAccessControlService;

    /**
    * Ponteiro para o stub do servi�o de tolerancia a falhas.
    */
      IFaultTolerantService_var iFaultTolerantService;

    /**
    * Inicializador do ORB. 
    */
      static interceptors::ORBInitializerImpl* ini;

    /**
    * ORB. 
    */
    #ifndef OPENBUS_MICO
      CORBA::ORB_var orb;
    #else
      CORBA::ORB* orb;
    #endif

    /**
    * POA.
    */
      PortableServer::POA_var poa;

    /**
    * F�brica de componentes SCS. 
    */
      scs::core::ComponentBuilder* componentBuilder;

    /**
    * Gerenciador do POA. 
    */
      PortableServer::POAManager_var poa_manager;

    /**
    * Intervalo de tempo que determina quando a credencial expira. 
    */
      access_control_service::Lease lease;

    /**
    * Credencial de identifica��o do usu�rio frente ao barramento. 
    */
      access_control_service::Credential* credential;

    /**
    * Pol�tica de valida��o de credenciais.
    */
      interceptors::CredentialValidationPolicy credentialValidationPolicy;

    /**
    * M�quina em que est� o barramento. 
    */
      string hostBus;

    /**
    * A porta da m�quina em que se encontra o barramento.
    */
      unsigned short portBus;

    /**
    * Poss�veis estados para a conex�o. 
    */
      enum ConnectionStates {
        CONNECTED,
        DISCONNECTED
      };

    /**
    * Indica o estado da conex�o. 
    */
      ConnectionStates connectionState;

    /**
    * Intervalo de tempo que determina quando que a credencial ser� renovada.
    */
      unsigned long timeRenewing;

    /**
    * Especifica se o tempo de renova��o de credencial � fixo.
    */
      bool timeRenewingFixe;

    /*
    * Intervalo de valida��o das credenciais do cache.
    */
      unsigned long credentialsValidationTime;

     /**
     * Cria o objeto registryService.
     */
      IfaceMap ifaceMap;

    /**
    * Trata os par�metros de linha de comando.
    */
      void commandLineParse(
        int argc,
        char** argv);

    /**
    * Inicializa um valor default para a m�quina e porta do barramento. 
    */
      void initialize();

    /**
    * Cria implicitamente um ORB e um POA. 
    */
      void createOrbPoa();

    /**
    * Registra os interceptadores cliente e servidor. 
    */
      void registerInterceptors();

    /**
    * Cria um estado novo. 
    */
      void newState();
      
    /**
    * Cria o proxy para o servi�o de acesso.
    */
      void createProxyToIAccessControlService();
      friend class openbus::interceptors::ClientInterceptor;

    /**
    * Cria o objeto registryService.
    */
      void setRegistryService();

    /**
    * Callback registrada para a notifica��o da 
    * expira��o do lease.
    */
      static LeaseExpiredCallback* _leaseExpiredCallback;

    #ifndef OPENBUS_MICO
      IT_Thread renewLeaseIT_Thread;

    /**
    * Thread respons�vel pela renova��o da credencial do usu�rio que est� 
    * logado neste barramento.
    */
      class RenewLeaseThread : public IT_ThreadBody {
        public:
          RenewLeaseThread();
          void setLeaseExpiredCallback(LeaseExpiredCallback* obj);
          void* run();
      };
      friend class Openbus::RenewLeaseThread;

    /**
    * Thread respons�vel pela renova��o de credencial.
    */
      static RenewLeaseThread* renewLeaseThread;
    #endif

  #ifdef OPENBUS_MICO

    #ifdef MULTITHREAD
      class RunThread : public MICOMT::Thread {
        public:
          void _run(void*);
      };
      friend class Openbus::RunThread;

      static RunThread* runThread;
    #else
      class RenewLeaseCallback : public CORBA::DispatcherCallback {
        public:
          RenewLeaseCallback();
          void setLeaseExpiredCallback(LeaseExpiredCallback* obj);
          void callback(CORBA::Dispatcher* dispatcher, Event event);
      };

    /**
    * Callbak respons�vel por renovar a credencial.
    */
      RenewLeaseCallback renewLeaseCallback;
    #endif
  #endif

    /**
    * Flag que informa se o mecanismo de tolerancia a falhas est� 
    * ativado.
    */
      bool faultToleranceEnable;

      Openbus();
      friend class FaultToleranceManager;
    public:

      static Logger* logger;

      ~Openbus();

    /**
    * Fornece a �nica inst�ncia do barramento.
    *
    * @return Openbus
    */
      static Openbus* getInstance();

    /**
    * Inicializa uma refer�ncia a um barramento.
    *
    * Um ORB e um POA s�o criados implicitamente.
    * A f�brica de componentes SCS � criada.
    * Os argumentos Openbus de linha de comando (argc e argv) s�o tratados.
    * Par�metros de linha de comando: 
    *   "-OpenbusHost": Host do barramento.
    *   "-OpenbusPort": Porta do barramento.
    *   "-TimeRenewing": Tempo em milisegundos de renova��o da credencial.
    *
    * @param[in] argc
    * @param[in] argv
    */
      void init(
        int argc,
        char** argv);

    /**
    * Inicializa uma refer�ncia a um barramento.
    *
    * Um ORB e um POA s�o criados implicitamente.
    * A f�brica de componentes SCS � criada.
    * Os argumentos Openbus de linha de comando (argc e argv) s�o tratados.
    * Par�metros de linha de comando: 
    *   "-OpenbusHost": M�quina em que se encontra o barramento.
    *   "-OpenbusPort": Porta do barramento.
    *   "-TimeRenewing": Tempo em milisegundos de renova��o da credencial.
    *
    * @param[in] argc
    * @param[in] argv
    * @param[in] host M�quina em que se encontra o barramento.
    * @param[in] port A porta da m�quina em que se encontra o barramento.
    * @param[in] policy Pol�tica de renova��o de credenciais. Este par�metro
    *   � opcional. O padr�o � adotar a pol�tica ALWAYS.
    */
      void init(
        int argc,
        char** argv,
        char* host,
        unsigned short port,
        interceptors::CredentialValidationPolicy policy = interceptors::ALWAYS);

    /**
    * Informa o estado de conex�o com o barramento.
    *
    * @return true caso a conex�o esteja ativa, ou false, caso
    * contr�rio.
    */
      bool isConnected();

    /** 
    *  Termination Handler dispon�vel para a classe IT_TerminationHandler()
    *
    *  @param signalType
    */
      static void terminationHandlerCallback(long signalType);

    /**
    *  Retorna o ORB utilizado.
    *  @return ORB
    */
      CORBA::ORB* getORB();

    /**
    *  Retorna o RootPOA.
    *
    *  OBS: A chamada a este m�todo ativa o POAManager.
    *
    *  @return POA
    */
      PortableServer::POA* getRootPOA();

    /**
    * Retorna a f�brica de componentes. 
    * @return F�brica de componentes
    */
      scs::core::ComponentBuilder* getComponentBuilder();

    /**
    * Retorna a credencial interceptada pelo interceptador servidor. 
    * @return Credencial. \see openbusidl::acs::Credential
    */
      access_control_service::Credential_var getInterceptedCredential();

    /**
    * Retorna o servi�o de acesso. 
    * @return Servi�o de acesso
    */
      access_control_service::IAccessControlService* getAccessControlService();

    /**
    * Retorna o servi�o de registro. 
    * @return Servi�o de registro
    */
      registry_service::IRegistryService* getRegistryService();

    /**
    * Retorna o servi�o de sess�o. 
    * @return Servi�o de sess�o.
    */
      ISessionService* getSessionService() 
        throw(NO_CONNECTED, NO_SESSION_SERVICE);

    /**
    * Retorna a credencial de identifica��o do usu�rio frente ao barramento. 
    * @return credencial
    */
      access_control_service::Credential* getCredential();

    /**
    * Retorna a pol�tica de valida��o de credenciais.
    * @return Pol�tica de valida��o de credenciais.
    */
      interceptors::CredentialValidationPolicy getCredentialValidationPolicy();

    /**
    * Define uma credencial a ser utilizada no lugar da credencial corrente. 
    * �til para fornecer uma credencial com o campo delegate preenchido.
    * 
    * @param[in] credential Credencial a ser utilizada nas requisi��es a serem
    *   realizadas.
    */
      void setThreadCredential(access_control_service::Credential* credential);

    /**
    * Representa uma callback para a notifica��o de que um lease expirou.
    */
      class LeaseExpiredCallback {
        public:
          virtual void expired() = 0;
      };

    /**
    * Registra uma callback para a notifica��o de que o lease da credencial
    * de identifica��o do usu�rio, frente ao barramento, expirou.
    *
    * @param[in] A callback a ser registrada.
    * @return True se a callback foi registrada com sucesso, ou false 
    * se a callback j� estava registrada.
    */
      void setLeaseExpiredCallback(
        LeaseExpiredCallback* leaseExpiredCallback);

    /**
    * Remove uma callback previamente registra para a notifica��o de lease 
    * expirado.
    *
    * @param[in] A callback a ser removida.
    * @return True se a callback foi removida com sucesso, ou false 
    * caso contr�rio.
    */
      void removeLeaseExpiredCallback();

    /**
    *  Realiza uma tentativa de conex�o com o barramento.
    *
    *  @param[in] user Nome do usu�rio.
    *  @param[in] password Senha do usu�rio.
    *  @throw LOGIN_FAILURE O par nome de usu�rio e senha n�o foram validados.
    *  @throw CORBA::SystemException Alguma falha de comunica��o com o 
    *    barramento ocorreu.
    *  @return  Se a tentativa de conex�o for bem sucedida, uma inst�ncia que 
    *    representa o servi�o � retornada.
    */
      registry_service::IRegistryService* connect(
        const char* user,
        const char* password)
        throw (CORBA::SystemException, LOGIN_FAILURE);

    /**
    *  Realiza uma tentativa de conex�o com o barramento utilizando o
    *    mecanismo de certifica��o para o processo de login.
    *
    *  @param[in] entity Nome da entidade a ser autenticada atrav�s de um
    *    certificado digital.
    *  @param[in] privateKeyFilename Nome do arquivo que armazena a chave
    *    privada do servi�o.
    *  @param[in] ACSCertificateFilename Nome do arquivo que armazena o
    *    certificado do servi�o.
    *  @throw LOGIN_FAILURE O par nome de usu�rio e senha n�o foram validados.
    *  @throw CORBA::SystemException Alguma falha de comunica��o com o 
    *    barramento ocorreu.
    *  @throw SECURITY_EXCEPTION Falha no mecanismo de autentica��o por 
    *    certificado digital.
    *    Algumas poss�veis causas:
    *     + N�o foi poss�vel obter o desafio.
    *     + Falha na manipula��o de uma chave privada ou p�blica.
    *     + Falha na manipula��o de um certificado.
    *       entidade ou do certificado do ACS.
    *  @return  Se a tentativa de conex�o for bem sucedida, uma inst�ncia que 
    *    representa o servi�o � retornada.
    */
      registry_service::IRegistryService* connect(
        const char* entity,
        const char* privateKeyFilename,
        const char* ACSCertificateFilename)
        throw (CORBA::SystemException, LOGIN_FAILURE, SECURITY_EXCEPTION);

    /**
    *  Desfaz a conex�o atual.
    *
    *  @return Caso a conex�o seja desfeita, true � retornado, caso contr�rio,
    *  o valor de retorno � false.
    */
      bool disconnect();

    /**
    * Loop que processa requisi��es CORBA. [execu��o do orb->run()]. 
    */
      void run();

    /**
    * Finaliza a execu��o do ORB.
    *
    * @param[in] bool force Se a finaliza��o deve ser for�ada ou n�o.
    */
      void finish(bool force);

    /**
    * Configura os m�todos da interface devem ser interceptados pelo servidor.
    *
    * @param[in] string iface RepID da interface.
    * @param[in] string method Nome do m�todo.
    * @param[in] bool interceptable Indica se o m�todo deve ser interceptado.
    *
    */
      void setInterceptable(string iface, string method, bool interceptable);

    /**
    * Consulta se o m�todo est� sendo interceptado.
    *
    * @param[in] string iface RepID da interface.
    * @param[in] string method Nome do m�todo.
    *
    * @return true se o m�todo � interceptado ou false caso contr�rio.
    */
      bool isInterceptable(string iface, string method);

   /**
   * Consulta se o mecanismo de tolerancia a falhas est� ativado.
   *
   * @return true se o mecanismo de tolerancia a falhas est� ativado ou false 
   * caso contrario.
   */
      bool isFaultToleranceEnable();
  };
}

#endif

