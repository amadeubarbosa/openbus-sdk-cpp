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
* \brief Falha no processo de login, ou seja, o par nome de usu�rio e senha n�o foi validado.
*/
class login_error : public std::runtime_error
{
public:
  login_error(const std::string& msg = "") : runtime_error(msg) {}
};

/**
* \brief Falha no mecanismo de autentica��o por certificado digital.
* Algumas poss�veis causas:
*  + N�o foi poss�vel obter o desafio.
*  + Falha na manipula��o de uma chave privada ou p�blica.
*  + Falha na manipula��o de um certificado.
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
   * Thread/Callback respons�vel pela renova��o da credencial do usu�rio que est� logado neste 
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
   * O modo reativo n�o funciona adequadamente na vers�o *multithread*. A alternativa encontrada 
   * foi utilizar uma thread dedicada ao orb->run() afim de evitar _deadlock_ distribu�do.
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
   * Inicializa uma refer�ncia a um barramento.
   *
   * Um ORB e um POA s�o criados implicitamente.
   * A f�brica de componentes SCS � criada.
   * Os argumentos Openbus de linha de comando (argc e argv) s�o tratados.
   * Par�metros de linha de comando: 
   *   "-OpenbusHost": M�quina em que se encontra o barramento.
   *   "-OpenbusPort": Porta do barramento.
   *   "-OpenbusDebug":
   *     ALL - Ativa todos os n�veis de verbose.
   *     ERROR - Ativa o n�vel ERROR do debug.
   *     INFO - Ativa o n�vel INFO do debug.
   *     WARNING - Ativa o n�vel WARNING do debug.
   *   "-OpenbusDebugFile": Caminho completo ou relativo do arquivo que armazenar� as  mensagens 
   *     de verbose. Se este par�metro n�o for definido, a sa�da do verbose ser� a sa�da padr�o.
   *     OBS.:  Se for  definido, o  verbose  somente  ser� armazenado  no  arquivo  em quest�o, 
   *     ou seja, a sa�da padr�o n�o ser� mais utilizada.
   *   "-OpenbusValidationPolicy": Define uma pol�tica de valida��o das credenciais. Por padr�o, 
   *     a pol�tica ALWAYS � adotada.
   *     NONE: N�o h� valida��o de credenciais.
   *     ALWAYS: Sempre valida cada credencial no ACS.
   *     CACHED: Primeiro  tenta  validar  a   credencial    consultando    um    cache   local, 
   *       se n�o conseguir, a valida��o  transcorre normalmente atrav�s de  uma  chamada remota 
   *       ao ACS.
   *   "-OpenbusValidationTime": Define o intervalo de tempo(em milisegundos)   de  valida��o do 
   *     cache de credenciais. O tempo padr�o � de 30000ms.
   *   "-OpenbusFTConfigFilename": Caminho completo  ou  relativo  do  arquivo  que  descreve as 
   *     r�plicas a serem utilizadas pelo mecanismo de toler�ncia a falhas.
   *   "-OpenbusTimeRenewing": Tempo em segundos de renova��o da credencial.
   *
   * @param[in] argc
   * @param[in] argv
   */
  Openbus(int argc
          , char** argv);

  /**
   * Inicializa uma refer�ncia a um barramento.
   *
   * Um ORB e um POA s�o criados implicitamente.
   * A f�brica de componentes SCS � criada.
   * Os argumentos Openbus de linha de comando (argc e argv) s�o tratados.
   * Par�metros de linha de comando: 
   *   "-OpenbusHost": M�quina em que se encontra o barramento.
   *   "-OpenbusPort": Porta do barramento.
   *   "-OpenbusDebug":
   *     ALL - Ativa todos os n�veis de verbose.
   *     ERROR - Ativa o n�vel ERROR do debug.
   *     INFO - Ativa o n�vel INFO do debug.
   *     WARNING - Ativa o n�vel WARNING do debug.
   *   "-OpenbusDebugFile": Caminho completo ou relativo do arquivo que armazenar� as  mensagens 
   *     de verbose. Se este par�metro n�o for definido, a sa�da do verbose ser� a sa�da padr�o.
   *     OBS.:  Se for  definido, o  verbose  somente  ser� armazenado  no  arquivo  em quest�o, 
   *     ou seja, a sa�da padr�o n�o ser� mais utilizada.
   *   "-OpenbusValidationPolicy": Define uma pol�tica de valida��o das credenciais. Por padr�o, 
   *     a pol�tica ALWAYS � adotada.
   *     NONE: N�o h� valida��o de credenciais.
   *     ALWAYS: Sempre valida cada credencial no ACS.
   *     CACHED: Primeiro  tenta  validar  a   credencial    consultando    um    cache   local, 
   *       se n�o conseguir, a valida��o  transcorre normalmente atrav�s de  uma  chamada remota 
   *       ao ACS.
   *   "-OpenbusValidationTime": Define o intervalo de tempo(em milisegundos)   de  valida��o do 
   *     cache de credenciais. O tempo padr�o � de 30000ms.
   *   "-OpenbusFTConfigFilename": Caminho completo  ou  relativo  do  arquivo  que  descreve as 
   *     r�plicas a serem utilizadas pelo mecanismo de toler�ncia a falhas.
   *   "-OpenbusTimeRenewing": Tempo em segundos de renova��o da credencial.
   *   
   *   Aten��o:Os par�metros de linha de comando podem sobrescrer os par�metros de fun��o: host,
   *           port e policy.
   *
   * @param[in] argc
   * @param[in] argv
   * @param[in] host M�quina em que se encontra o barramento.
   * @param[in] port A porta da m�quina em que se encontra o barramento.
   * @param[in] policy Pol�tica de renova��o de credenciais. Este par�metro � opcional. O padr�o
   *   � adotar a pol�tica ALWAYS.
   */
  Openbus(int argc
          , char** argv
          , char* host
          , unsigned short port
          , interceptors::CredentialValidationPolicy policy = interceptors::ALWAYS);

  /**
   * Informa o estado de conex�o com o barramento.
   *
   * @return true caso a conex�o esteja ativa, ou false, caso contr�rio.
   */
  bool isConnected();

  /** 
   *  Disponibiliza um *termination handler* que desconecta o usu�rio do barramento e finaliza a 
   *  execu��o do Openbus::run().
   *
   *  Essa callback pode ser utlizada em uma implementa��o de um *termination handler* a ser es-
   *  crito pelo usu�rio. No caso do Orbix, o m�todo pode ser registrado diretamente na   classe
   *  IT_TerminationHandler(), e.g.:
   *
   *  IT_TerminationHandler termination_handler(openbus::Openbus::terminationHandlerCallback)
   *
   *  O m�todo desconecta o usu�rio do barramento,   se   este   estiver  conectado,  executa um
   *  Openbus::stop()  seguido  por  um  Openbus::finish(),  e,  por  �ltimo  faz   _delete_  da 
   *  instancia��o do Openbus.
   *
   *  @param signalType
   */
  //static void terminationHandlerCallback(long signalType);

  /**
   *  Retorna o RootPOA.
   *
   *  OBS: A chamada a este m�todo ativa o POAManager.
   *
   *  @return POA
   */
  PortableServer::POA_var getRootPOA() const;

  /**
   * Retorna a f�brica de componentes. 
   * @return F�brica de componentes
   */
  scs::core::ComponentBuilder& getComponentBuilder();

  /**
   * Retorna a credencial interceptada pelo interceptador servidor. 
   * @return Credencial. \see openbusidl::acs::Credential
   */
  idl_namespace::access_control_service::Credential_var getInterceptedCredential();

  /**
   * Retorna o servi�o de registro. 
   * @return Servi�o de registro
   */
  idl_namespace::registry_service::IRegistryService_var getRegistryService() const;

  /**
   * Retorna a credencial de identifica��o do usu�rio frente ao barramento. 
   * @return credencial
   */
  boost::optional<idl_namespace::access_control_service::Credential>
    getCredential() const;

  /**
   * Define uma credencial a ser utilizada no lugar da credencial corrente. 
   * �til para fornecer uma credencial com o campo delegate preenchido.
   * 
   * !Aten��o!
   *   Nesta vers�o este m�todo n�o considera um comportamento *multi  thread*, 
   *   sendo assim a altera��o de credencial ser� v�lida para todas as threads.
   *
   * @param[in] credential Credencial a ser utilizada nas requisi��es a serem
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
   *  Realiza uma tentativa de conex�o com o barramento.
   *
   *  @param[in] user Nome do usu�rio.
   *  @param[in] password Senha do usu�rio.
   *  @throw LOGIN_FAILURE O par nome de usu�rio e senha n�o foram validados.
   *  @throw CORBA::SystemException Alguma falha de comunica��o com o barramento ocorreu.
   *  @return  Se a tentativa de conex�o for bem sucedida, uma inst�ncia que representa o 
   *    servi�o � retornada.
   */
  idl_namespace::registry_service::IRegistryService* connect(const char* user
                                                     , const char* password);

  /**
   *  Realiza uma tentativa de conex�o com o barramento utilizando o mecanismo de 
   *  certifica��o para o processo de login.
   *
   *  @param[in] entity Nome da entidade a ser autenticada atrav�s de um certificado digital.
   *  @param[in] privateKeyFilename Nome do arquivo que armazena a chave privada do servi�o.
   *  @param[in] ACSCertificateFilename Nome do arquivo que armazena o certificado do servi�o.
   *  @throw LOGIN_FAILURE O par nome de usu�rio e senha n�o foram validados.
   *  @throw CORBA::SystemException Alguma falha de comunica��o com o barramento ocorreu.
   *  @throw SECURITY_EXCEPTION Falha no mecanismo de autentica��o por certificado digital.
   *    Algumas poss�veis causas:
   *     + N�o foi poss�vel obter o desafio.
   *     + Falha na manipula��o de uma chave privada ou p�blica.
   *     + Falha na manipula��o de um certificado.
   *       entidade ou do certificado do ACS.
   *  @return  Se a tentativa de conex�o for bem sucedida, uma inst�ncia que representa o 
   *    servi�o � retornada.
   */
    idl_namespace::registry_service::IRegistryService* connect(const char* entity
                                                       , const char* privateKeyFilename
                                                       , const char* ACSCertificateFilename);

  /**
   *  Desfaz a conex�o atual.
   *
   *  @return Caso a conex�o seja desfeita, true � retornado, caso contr�rio, o valor de 
   *  retorno � false.
   */
  void disconnect();

  /**
   * Loop que processa requisi��es CORBA.
   *
   * !Aten��o! Este m�todo na vers�o Mico *multi thread* faz um wait() na RunThread.
   */
  void run();
      
  /**
   * P�ra de processar requisi��es CORBA. Finaliza a execu��o do run.
   * 
   * !Aten��o! N�o faz nada na vers�o Mico *multi thread*.
   */  
  void stop();

  /**
   * Finaliza a execu��o do ORB.
   *
   * O m�todo chama a dupla orb->shutdown(force) e orb->destroy().
   *
   * @param[in] bool force Se a finaliza��o deve ser for�ada ou n�o.
   */
  void finish(bool force = 0);

  /**
   * Define se os m�todos de uma determinada interface devem ou n�o ser interceptados pelo
   * interceptador servidor.
   *
   * @param[in] string interfaceRepID RepID da interface.
   * @param[in] string method Nome do m�todo.
   * @param[in] bool isInterceptable Indica se o m�todo deve ser interceptado.
   *
   */
  void setInterceptable(std::string interfaceRepID
                        , std::string method
                        , bool isInterceptable);

  /**
   * Consulta se o mecanismo de tolerancia a falhas est� ativado.
   *
   * @return true se o mecanismo de tolerancia a falhas est� ativado ou false 
   * caso contrario.
   */
  bool isFaultToleranceEnable();

  idl_namespace::access_control_service::IAccessControlService* getAccessControlService();

  CORBA::ORB_var getORB() const;
};

}

#endif
