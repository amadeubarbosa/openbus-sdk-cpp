/**
* \mainpage API - Openbus C++
* \file openbus.h
*/

#ifndef OPENBUS_SDK_OPENBUS_H
#define OPENBUS_SDK_OPENBUS_H

#include <openbus/version.h>
#include <openbus/orb_state.h>
#include <openbus/logging/logging.h>
#include <openbus/helper.h>

#include <openbus/interceptors/orb_initializer_impl.h>
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
  #include "access_control_service.h"
  #include "registry_service.h"
  #include "fault_tolerance.h"
#endif

#include <boost/shared_ptr.hpp>
#include <boost/optional.hpp>

#include <stdexcept>
#include <string.h>
#include <map>
#include <set>
#include <lua.hpp>

/**
* \brief openbus
*/
namespace openbus {

/**
* \brief Falha no processo de login, ou seja, o par nome de usuário e senha não foi validado.
*/
class login_error : public std::runtime_error
{
public:
  login_error(const std::string& msg = "") : runtime_error(msg) {}
};

/**
* \brief Falha no mecanismo de autenticação por certificado digital.
* Algumas possíveis causas:
*  + Não foi possível obter o desafio.
*  + Falha na manipulação de uma chave privada ou pública.
*  + Falha na manipulação de um certificado.
*/
class security_error : public std::runtime_error
{
public:
  security_error(const std::string& msg = "") : runtime_error(msg) {}
};

/**
 * \brief Representa um barramento.
 */
class Openbus
{
public:
  // static char* debugFile;
  // Level debugLevel;

  boost::shared_ptr<openbus::orb_state> orb_state;

  /**
   * Mutex. 
   */
#ifdef OPENBUS_ORBIX
  IT_Mutex mutex;
#else
  MICOMT::Mutex mutex;
#endif
  /**
   * Thread/Callback responsável pela renovação da credencial do usuário que está logado neste 
   * barramento.
   */
// #if (OPENBUS_ORBIX || (!OPENBUS_ORBIX && MULTITHREAD))
// #ifdef OPENBUS_ORBIX
//   IT_Thread renewLeaseIT_Thread;
//   class RenewLeaseThread : public IT_ThreadBody
//   {
//   public:
//     void* run();
// #else
//   class RenewLeaseThread : public MICOMT::Thread
//   {
//   public:
//     void _run(void*);
// #endif
//     bool runningLeaseExpiredCallback;
//     RenewLeaseThread();
//     void stop();
//   private:
//     bool sigINT;
//     void sleep(unsigned short time);
//   };
//   RenewLeaseThread* renewLeaseThread;
// #else
//   class RenewLeaseCallback : public CORBA::DispatcherCallback
//   {
//   public:
//     RenewLeaseCallback(openbus::orb_state& orb_state);
//     void callback(CORBA::Dispatcher* dispatcher
//                   , Event event);
//     openbus::orb_state* orb_state;
//   };
//   RenewLeaseCallback renewLeaseCallback;
// #endif

  /**
   * [OPENBUS-410] - Mico
   * 
   * O modo reativo não funciona adequadamente na versão *multithread*. A alternativa encontrada 
   * foi utilizar uma thread dedicada ao orb->run() afim de evitar _deadlock_ distribuído.
   */
#ifndef OPENBUS_ORBIX
#ifdef MULTITHREAD
  // class RunThread : public MICOMT::Thread
  // {
  // public:
  //   void _run(void*);
  // };
  // friend class Openbus::RunThread;

  // RunThread* runThread;
#endif
#endif
public:

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
  Openbus(int argc
          , char** argv);

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
  Openbus(int argc
          , char** argv
          , char* host
          , unsigned short port
          , interceptors::CredentialValidationPolicy policy = interceptors::ALWAYS);

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
  //static void terminationHandlerCallback(long signalType);

  /**
   *  Retorna o RootPOA.
   *
   *  OBS: A chamada a este método ativa o POAManager.
   *
   *  @return POA
   */
  PortableServer::POA_var getRootPOA() const;

  /**
   * Retorna a fábrica de componentes. 
   * @return Fábrica de componentes
   */
  scs::core::ComponentBuilder& getComponentBuilder();

  /**
   * Retorna a credencial interceptada pelo interceptador servidor. 
   * @return Credencial. \see openbusidl::acs::Credential
   */
  idl_namespace::access_control_service::Credential_var getInterceptedCredential();

  /**
   * Retorna o serviço de registro. 
   * @return Serviço de registro
   */
  idl_namespace::registry_service::IRegistryService_var getRegistryService() const;

  /**
   * Retorna a credencial de identificação do usuário frente ao barramento. 
   * @return credencial
   */
  boost::optional<idl_namespace::access_control_service::Credential>
    getCredential() const;

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
  void setThreadCredential(idl_namespace::access_control_service::Credential* credential);

  typedef orb_state::lease_connection
    lease_connection;

  template <typename F>
  lease_connection setLeaseExpiredCallback(F f)
  {
    return orb_state->setLeaseExpiredCallback(f);
  }
  void removeLeaseExpiredCallback(lease_connection c)
  {
    orb_state->removeLeaseExpiredCallback(c);
  }

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
  idl_namespace::registry_service::IRegistryService* connect(const char* user
                                                     , const char* password);

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
    idl_namespace::registry_service::IRegistryService* connect(const char* entity
                                                       , const char* privateKeyFilename
                                                       , const char* ACSCertificateFilename);

  /**
   *  Desfaz a conexão atual.
   *
   *  @return Caso a conexão seja desfeita, true é retornado, caso contrário, o valor de 
   *  retorno é false.
   */
  void disconnect();

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
  void finish(bool force = 0);

  /**
   * Define se os métodos de uma determinada interface devem ou não ser interceptados pelo
   * interceptador servidor.
   *
   * @param[in] string interfaceRepID RepID da interface.
   * @param[in] string method Nome do método.
   * @param[in] bool isInterceptable Indica se o método deve ser interceptado.
   *
   */
  void setInterceptable(std::string interfaceRepID
                        , std::string method
                        , bool isInterceptable);

  /**
   * Consulta se o mecanismo de tolerancia a falhas está ativado.
   *
   * @return true se o mecanismo de tolerancia a falhas está ativado ou false 
   * caso contrario.
   */
  bool isFaultToleranceEnable();

  idl_namespace::access_control_service::IAccessControlService* getAccessControlService();

  CORBA::ORB_var getORB() const;
};

}

#endif
