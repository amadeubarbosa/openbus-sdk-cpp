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

#ifdef OPENBUS_ORBIX
  #include <omg/orb.hh>
  #include <it_ts/thread.h>
  #include <it_ts/timer.h>
  #include <it_ts/mutex.h>
  #include "stubs/orbix/access_control_service.hh"
  #include "stubs/orbix/registry_service.hh"
  #include "stubs/orbix/fault_tolerance.hh"
#else
  #include <CORBA.h>
  #include "stubs/mico/access_control_service.h"
  #include "stubs/mico/registry_service.h"
  #include "stubs/mico/fault_tolerance.h"
#endif

#include <stdexcept>
#include <string.h>
#include <map>
#include <set>
#include <lua.hpp>

using namespace tecgraf::openbus::core::v1_05;
using namespace tecgraf::openbus::fault_tolerance::v1_05;
using namespace std;
using namespace logger;

/**
* \brief Stubs dos serviços básicos.
*/
namespace tecgraf {
  namespace openbus {
    namespace core {
      namespace v1_05 {

      /**
      * \brief Stub do serviço de acesso.
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
  typedef map<string, MethodSet*> MethodsNotInterceptable;

/**
* \brief Falha no processo de login, ou seja, o par nome de usuário e senha não foi validado.
*/
  class LOGIN_FAILURE : public runtime_error {
    public:
      LOGIN_FAILURE(const string& msg = "") : runtime_error(msg) {}
  };

/**
* \brief Falha no mecanismo de autenticação por certificado digital.
* Algumas possíveis causas:
*  + Não foi possível obter o desafio.
*  + Falha na manipulação de uma chave privada ou pública.
*  + Falha na manipulação de um certificado.
*/
  class SECURITY_EXCEPTION : public runtime_error {
    public:
      SECURITY_EXCEPTION(const string& msg = "") : runtime_error(msg) {}
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

    /**
    * Mutex. 
    */
    #ifdef OPENBUS_ORBIX
      static IT_Mutex mutex;
    #else
      static MICOMT::Mutex mutex;
    #endif

    /**
    * A instância única do barramento.
    */
      static Openbus* bus;

    /**
    * Inicializador do ORB. 
    */
      static interceptors::ORBInitializerImpl* ini;

    /**
    * ORB. 
    */
      static CORBA::ORB_var orb;
      
    /**
    * Determina se o ORB está escutando requisições CORBA.
    */
      static bool orbRunning;

    /**
    * POA.
    */
      static PortableServer::POA_var poa;

    /**
    * Máquina virtual de Lua.
    */
      static lua_State* luaState;

    /**
    * Nome do arquivo de configuração das réplicas.
    */
      static char* FTConfigFilename;

    /**
    * Parâmetro argc da linha de comando. 
    */
      int _argc;

    /**
    * Parâmetro argv da linha de comando. 
    */
      char** _argv;
      
    /**
    * Ponteiro para o stub do serviço de acesso.
    */
      access_control_service::IAccessControlService_var iAccessControlService;

    /**
    * Ponteiro para o stub do serviço de registro.
    */
      registry_service::IRegistryService* iRegistryService;

    /**
    * Ponteiro para a faceta ILeaseProvider. 
    */
      access_control_service::ILeaseProvider_var iLeaseProvider;

    /**
    * Ponteiro para o IComponent do serviço de acesso. 
    */
      scs::core::IComponent_var iComponentAccessControlService;

    /**
    * Ponteiro para o stub do serviço de tolerância a falhas.
    */
      IFaultTolerantService_var iFaultTolerantService;

    /**
    * Fábrica de componentes SCS. 
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
    * Credencial de identificação do usuário frente ao barramento. 
    */
      access_control_service::Credential* credential;

    #if (!OPENBUS_ORBIX && MULTITHREAD)
      static MICOMT::Thread::ThreadKey threadKey;
    #endif
    
    /**
    * Política de validação de credenciais.
    */
      interceptors::CredentialValidationPolicy credentialValidationPolicy;

    /**
    * Máquina em que está o barramento. 
    */
      string hostBus;

    /**
    * A porta da máquina em que se encontra o barramento.
    */
      unsigned short portBus;

    /**
    * Possíveis estados para a conexão. 
    */
      enum ConnectionStates {
        CONNECTED,
        DISCONNECTED
      };

    /**
    * Indica o estado da conexão. 
    */
      ConnectionStates connectionState;

    /**
    * Intervalo de tempo em segundos que determina quando que a credencial será renovada.
    */
      unsigned short timeRenewing;

    /**
    * Especifica se o tempo de renovação da credencial é fixo.
    */
      bool timeRenewingFixe;

    /*
    * Intervalo de validação das credenciais do cache.
    */
      unsigned long credentialsValidationTime;

     /**
     * Mapa de métodos relacionados a suas respectivas interfaces que devem ser ignorados pelo 
     * interceptador servidor.
     */
      MethodsNotInterceptable methodsNotInterceptable;

    /**
    * Trata os parâmetros de linha de comando.
    */
      void commandLineParse();

    /**
    * Inicializa um valor default para a máquina e porta do barramento. 
    */
      void initialize();

    /**
    * Cria implicitamente um ORB e um POA. 
    */
      void createORB();

    /**
    * Registra os interceptadores cliente e servidor. 
    */
      void registerInterceptors();

    /**
    * Cria um estado novo. 
    */
      void newState();
      
    /**
    * Cria o proxy para o serviço de acesso.
    */
      void createProxyToIAccessControlService();
      friend class openbus::interceptors::ClientInterceptor;

    /**
    * Cria o objeto registryService.
    */
      void setRegistryService();

    /**
    * Callback registrada para a notificação da expiração do lease.
    */
      static LeaseExpiredCallback* _leaseExpiredCallback;

    /**
    * Thread/Callback responsável pela renovação da credencial do usuário que está logado neste 
    * barramento.
    */
    #if (OPENBUS_ORBIX || (!OPENBUS_ORBIX && MULTITHREAD))
      #ifdef OPENBUS_ORBIX
      IT_Thread renewLeaseIT_Thread;
      class RenewLeaseThread : public IT_ThreadBody {
        public:
          void* run();
      #else
      class RenewLeaseThread : public MICOMT::Thread {
        public:
          void _run(void*);
      #endif
          bool runningLeaseExpiredCallback;
          RenewLeaseThread();
          void stop();
        private:
          bool sigINT;
          void sleep(unsigned short time);
      };
      static RenewLeaseThread* renewLeaseThread;
    #else
      class RenewLeaseCallback : public CORBA::DispatcherCallback {
        public:
          RenewLeaseCallback();
          void callback(
            CORBA::Dispatcher* dispatcher, 
            Event event);
      };
      RenewLeaseCallback renewLeaseCallback;
    #endif

    /**
    * [OPENBUS-410] - Mico
    * 
    * O modo reativo não funciona adequadamente na versão *multithread*. A alternativa encontrada 
    * foi utilizar uma thread dedicada ao orb->run() afim de evitar _deadlock_ distribuído.
    */
    #ifndef OPENBUS_ORBIX
      #ifdef MULTITHREAD
        class RunThread : public MICOMT::Thread {
          public:
            void _run(void*);
        };
        friend class Openbus::RunThread;

        static RunThread* runThread;
      #endif
    #endif

    /**
    * Flag que informa se o mecanismo de tolerância a falhas está ativado.
    */
      bool faultToleranceEnable;

      Openbus();
      friend class FaultToleranceManager;
    public:

      static Logger* logger;

      ~Openbus();

    /**
    * Fornece a única instância do barramento.
    *
    * @return Openbus
    */
      static Openbus* getInstance();

    /**
    * Inicializa uma referência a um barramento.
    *
    * Um ORB e um POA são criados implicitamente.
    * A fábrica de componentes SCS é criada.
    * Os argumentos Openbus de linha de comando (argc e argv) são tratados.
    * Parâmetros de linha de comando: 
    *   "-OpenbusHost": Máquina em que se encontra o barramento.
    *   "-OpenbusPort": Porta do barramento.
    *   "-OpenbusDebug":
    *     ALL - Ativa todos os níveis de verbose.
    *     ERROR - Ativa o nível ERROR do debug.
    *     INFO - Ativa o nível INFO do debug.
    *     WARNING - Ativa o nível WARNING do debug.
    *   "-OpenbusDebugFile": Caminho completo ou relativo do arquivo que armazenará as  mensagens 
    *     de verbose. Se este parâmetro não for definido, a saída do verbose será a saída padrão.
    *     OBS.:  Se for  definido, o  verbose  somente  será armazenado  no  arquivo  em questão, 
    *     ou seja, a saída padrão não será mais utilizada.
    *   "-OpenbusValidationPolicy": Define uma política de validação das credenciais. Por padrão, 
    *     a política ALWAYS é adotada.
    *     NONE: Não há validação de credenciais.
    *     ALWAYS: Sempre valida cada credencial no ACS.
    *     CACHED: Primeiro  tenta  validar  a   credencial    consultando    um    cache   local, 
    *       se não conseguir, a validação  transcorre normalmente através de  uma  chamada remota 
    *       ao ACS.
    *   "-OpenbusValidationTime": Define o intervalo de tempo(em milisegundos)   de  validação do 
    *     cache de credenciais. O tempo padrão é de 30000ms.
    *   "-OpenbusFTConfigFilename": Caminho completo  ou  relativo  do  arquivo  que  descreve as 
    *     réplicas a serem utilizadas pelo mecanismo de tolerância a falhas.
    *   "-OpenbusTimeRenewing": Tempo em segundos de renovação da credencial.
    *
    * @param[in] argc
    * @param[in] argv
    */
      void init(
        int argc,
        char** argv);

    /**
    * Inicializa uma referência a um barramento.
    *
    * Um ORB e um POA são criados implicitamente.
    * A fábrica de componentes SCS é criada.
    * Os argumentos Openbus de linha de comando (argc e argv) são tratados.
    * Parâmetros de linha de comando: 
    *   "-OpenbusHost": Máquina em que se encontra o barramento.
    *   "-OpenbusPort": Porta do barramento.
    *   "-OpenbusDebug":
    *     ALL - Ativa todos os níveis de verbose.
    *     ERROR - Ativa o nível ERROR do debug.
    *     INFO - Ativa o nível INFO do debug.
    *     WARNING - Ativa o nível WARNING do debug.
    *   "-OpenbusDebugFile": Caminho completo ou relativo do arquivo que armazenará as  mensagens 
    *     de verbose. Se este parâmetro não for definido, a saída do verbose será a saída padrão.
    *     OBS.:  Se for  definido, o  verbose  somente  será armazenado  no  arquivo  em questão, 
    *     ou seja, a saída padrão não será mais utilizada.
    *   "-OpenbusValidationPolicy": Define uma política de validação das credenciais. Por padrão, 
    *     a política ALWAYS é adotada.
    *     NONE: Não há validação de credenciais.
    *     ALWAYS: Sempre valida cada credencial no ACS.
    *     CACHED: Primeiro  tenta  validar  a   credencial    consultando    um    cache   local, 
    *       se não conseguir, a validação  transcorre normalmente através de  uma  chamada remota 
    *       ao ACS.
    *   "-OpenbusValidationTime": Define o intervalo de tempo(em milisegundos)   de  validação do 
    *     cache de credenciais. O tempo padrão é de 30000ms.
    *   "-OpenbusFTConfigFilename": Caminho completo  ou  relativo  do  arquivo  que  descreve as 
    *     réplicas a serem utilizadas pelo mecanismo de tolerância a falhas.
    *   "-OpenbusTimeRenewing": Tempo em segundos de renovação da credencial.
    *   
    *   Atenção:Os parâmetros de linha de comando podem sobrescrer os parâmetros de função: host,
    *           port e policy.
    *
    * @param[in] argc
    * @param[in] argv
    * @param[in] host Máquina em que se encontra o barramento.
    * @param[in] port A porta da máquina em que se encontra o barramento.
    * @param[in] policy Política de renovação de credenciais. Este parâmetro é opcional. O padrão
    *   é adotar a política ALWAYS.
    */
      void init(
        int argc,
        char** argv,
        char* host,
        unsigned short port,
        interceptors::CredentialValidationPolicy policy = interceptors::ALWAYS);

    /**
    * Informa o estado de conexão com o barramento.
    *
    * @return true caso a conexão esteja ativa, ou false, caso contrário.
    */
      bool isConnected();

    /** 
    *  Disponibiliza um *termination handler* que desconecta o usuário do barramento e finaliza a 
    *  execução do Openbus::run().
    *
    *  Essa callback pode ser utlizada em uma implementação de um *termination handler* a ser es-
    *  crito pelo usuário. No caso do Orbix, o método pode ser registrado diretamente na   classe
    *  IT_TerminationHandler(), e.g.:
    *
    *  IT_TerminationHandler termination_handler(openbus::Openbus::terminationHandlerCallback)
    *
    *  O método desconecta o usuário do barramento,   se   este   estiver  conectado,  executa um
    *  Openbus::stop()  seguido  por  um  Openbus::finish(),  e,  por  último  faz   _delete_  da 
    *  instanciação do Openbus.
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
    *  OBS: A chamada a este método ativa o POAManager.
    *
    *  @return POA
    */
      PortableServer::POA* getRootPOA();

    /**
    * Retorna a fábrica de componentes. 
    * @return Fábrica de componentes
    */
      scs::core::ComponentBuilder* getComponentBuilder();

    /**
    * Retorna a credencial interceptada pelo interceptador servidor. 
    * @return Credencial. \see openbusidl::acs::Credential
    */
      access_control_service::Credential_var getInterceptedCredential();

    /**
    * Retorna o serviço de acesso. 
    * @return Serviço de acesso
    */
      access_control_service::IAccessControlService* getAccessControlService();

    /**
    * Retorna o serviço de registro. 
    * @return Serviço de registro
    */
      registry_service::IRegistryService* getRegistryService();

    /**
    * Retorna a credencial de identificação do usuário frente ao barramento. 
    * @return credencial
    */
      access_control_service::Credential* getCredential();

    /**
    * Retorna a política de validação de credenciais.
    * @return Política de validação de credenciais.
    */
      interceptors::CredentialValidationPolicy getCredentialValidationPolicy();

    /**
    * Define uma credencial a ser utilizada no lugar da credencial corrente. 
    * Útil para fornecer uma credencial com o campo delegate preenchido.
    * 
    * !Atenção!
    *   Nesta versão este método não considera um comportamento *multi  thread*, 
    *   sendo assim a alteração de credencial será válida para todas as threads.
    *
    * @param[in] credential Credencial a ser utilizada nas requisições a serem
    *   realizadas.
    */
      void setThreadCredential(access_control_service::Credential* credential);

    /**
    * Representa uma callback para a notificação de que um lease expirou.
    */
      class LeaseExpiredCallback {
        public:
          virtual ~LeaseExpiredCallback() {};
          virtual void expired() = 0;
      };

    /**
    * Registra uma callback para a notificação de que o lease da credencial de identificação 
    * do usuário, frente ao barramento, expirou.
    *
    * @param[in] A callback a ser registrada.
    * @return True se a callback foi registrada com sucesso, ou false 
    * se a callback já estava registrada.
    */
      void setLeaseExpiredCallback(LeaseExpiredCallback* leaseExpiredCallback);

    /**
    * Remove uma callback previamente registra para a notificação de lease expirado.
    *
    * @param[in] A callback a ser removida.
    * @return True se a callback foi removida com sucesso, ou false 
    * caso contrário.
    */
      void removeLeaseExpiredCallback();

    /**
    *  Realiza uma tentativa de conexão com o barramento.
    *
    *  @param[in] user Nome do usuário.
    *  @param[in] password Senha do usuário.
    *  @throw LOGIN_FAILURE O par nome de usuário e senha não foram validados.
    *  @throw CORBA::SystemException Alguma falha de comunicação com o barramento ocorreu.
    *  @return  Se a tentativa de conexão for bem sucedida, uma instância que representa o 
    *    serviço é retornada.
    */
      registry_service::IRegistryService* connect(
        const char* user,
        const char* password)
        throw (CORBA::SystemException, LOGIN_FAILURE);

    /**
    *  Realiza uma tentativa de conexão com o barramento utilizando o mecanismo de 
    *  certificação para o processo de login.
    *
    *  @param[in] entity Nome da entidade a ser autenticada através de um certificado digital.
    *  @param[in] privateKeyFilename Nome do arquivo que armazena a chave privada do serviço.
    *  @param[in] ACSCertificateFilename Nome do arquivo que armazena o certificado do serviço.
    *  @throw LOGIN_FAILURE O par nome de usuário e senha não foram validados.
    *  @throw CORBA::SystemException Alguma falha de comunicação com o barramento ocorreu.
    *  @throw SECURITY_EXCEPTION Falha no mecanismo de autenticação por certificado digital.
    *    Algumas possíveis causas:
    *     + Não foi possível obter o desafio.
    *     + Falha na manipulação de uma chave privada ou pública.
    *     + Falha na manipulação de um certificado.
    *       entidade ou do certificado do ACS.
    *  @return  Se a tentativa de conexão for bem sucedida, uma instância que representa o 
    *    serviço é retornada.
    */
      registry_service::IRegistryService* connect(
        const char* entity,
        const char* privateKeyFilename,
        const char* ACSCertificateFilename)
        throw (CORBA::SystemException, LOGIN_FAILURE, SECURITY_EXCEPTION);

    /**
    *  Desfaz a conexão atual.
    *
    *  @return Caso a conexão seja desfeita, true é retornado, caso contrário, o valor de 
    *  retorno é false.
    */
      bool disconnect();

    /**
    * Loop que processa requisições CORBA.
    *
    * !Atenção! Este método na versão Mico *multi thread* faz um wait() na RunThread.
    */
      void run();
      
    /**
    * Pára de processar requisições CORBA. Finaliza a execução do run.
    * 
    * !Atenção! Não faz nada na versão Mico *multi thread*.
    */  
      void stop();

    /**
    * Finaliza a execução do ORB.
    *
    * O método chama a dupla orb->shutdown(force) e orb->destroy().
    *
    * @param[in] bool force Se a finalização deve ser forçada ou não.
    */
      static void finish(bool force = 0);

    /**
    * Define se os métodos de uma determinada interface devem ou não ser interceptados pelo
    * interceptador servidor.
    *
    * @param[in] string interfaceRepID RepID da interface.
    * @param[in] string method Nome do método.
    * @param[in] bool isInterceptable Indica se o método deve ser interceptado.
    *
    */
      void setInterceptable(
        string interfaceRepID, 
        string method, 
        bool isInterceptable);

    /**
    * Consulta se o método está sendo interceptado.
    *
    * @param[in] string interfaceRepID RepID da interface.
    * @param[in] string method Nome do método.
    *
    * @return true se o método é interceptado ou false caso contrário.
    */
      bool isInterceptable(
        string interfaceRepID, 
        string method);

   /**
   * Consulta se o mecanismo de tolerancia a falhas está ativado.
   *
   * @return true se o mecanismo de tolerancia a falhas está ativado ou false 
   * caso contrario.
   */
      bool isFaultToleranceEnable();
  };
}

#endif
