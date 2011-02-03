#ifndef OPENBUS_SDK_ORB_STATE_H
#define OPENBUS_SDK_ORB_STATE_H

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

#include <boost/optional.hpp>
#include <boost/function.hpp>
#include <boost/signals2.hpp>

#include <lua.hpp>

#include <memory>

namespace openbus {

namespace idl_namespace = tecgraf::openbus::core::OPENBUS_IDL_VERSION_NAMESPACE;
namespace fault_tolerance = tecgraf::openbus::fault_tolerance;

typedef std::set<std::string> MethodSet;
typedef std::map<std::string, MethodSet*> MethodsNotInterceptable;

struct orb_state
{
  /**
   * Inicializador do ORB. 
   */
  std::auto_ptr<interceptors::orb_initializer_impl> ini;
  /**
   * ORB. 
   */
  CORBA::ORB_var orb;
  /**
   * Determina se o ORB está escutando requisições CORBA.
   */
  bool orbRunning;
  /**
   * POA.
   */
  PortableServer::POA_var poa;

  /**
   * Máquina virtual de Lua.
   */
  lua_State* luaState;

  /**
   * Nome do arquivo de configuração das réplicas.
   */
  char* FTConfigFilename;

  /**
   * Ponteiro para o stub do serviço de acesso.
   */
  idl_namespace::access_control_service::IAccessControlService_var iAccessControlService;

  /**
   * Ponteiro para o stub do serviço de registro.
   */
  mutable idl_namespace::registry_service::IRegistryService_var iRegistryService;

  /**
   * Ponteiro para a faceta ILeaseProvider. 
   */
  idl_namespace::access_control_service::ILeaseProvider_var iLeaseProvider;

  /**
   * Ponteiro para o IComponent do serviço de acesso. 
   */
  scs::core::IComponent_var iComponentAccessControlService;

  /**
   * Ponteiro para o stub do serviço de tolerância a falhas.
   */
  fault_tolerance:: OPENBUS_IDL_VERSION_NAMESPACE ::IFaultTolerantService_var iFaultTolerantService;

  /**
   * Fábrica de componentes SCS. 
   */
  std::auto_ptr<scs::core::ComponentBuilder> componentBuilder;

  /**
   * Gerenciador do POA. 
   */
  PortableServer::POAManager_var poa_manager;

  /**
   * Intervalo de tempo que determina quando a credencial expira. 
   */
  idl_namespace::access_control_service::Lease lease;

  /**
   * Credencial de identificação do usuário frente ao barramento. 
   */
  idl_namespace::access_control_service::Credential* credential;

  /**
   * Política de validação de credenciais.
   */
  interceptors::CredentialValidationPolicy credentialValidationPolicy;

  /**
   * Máquina em que está o barramento. 
   */
  std::string hostBus;

  /**
   * A porta da máquina em que se encontra o barramento.
   */
  unsigned short portBus;

  /**
   * Possíveis estados para a conexão. 
   */
  enum ConnectionStates
  {
    CONNECTED
    , DISCONNECTED
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

  typedef boost::signals2::connection lease_connection;
  boost::signals2::signal<void()> lease_expired_signal;

  /**
   * Flag que informa se o mecanismo de tolerância a falhas está ativado.
   */
  bool faultToleranceEnable;

  /**
   * Cria o proxy para o serviço de acesso.
   */
  void createProxyToIAccessControlService();

  /**
   * Retorna o serviço de acesso. 
   * @return Serviço de acesso
   */
  idl_namespace::access_control_service::IAccessControlService* getAccessControlService();

  void disconnect();

  /**
   *  Retorna o ORB utilizado.
   *  @return ORB
   */
  CORBA::ORB* getORB();

  /**
   * Consulta se o método está sendo interceptado.
   *
   * @param[in] string interfaceRepID RepID da interface.
   * @param[in] string method Nome do método.
   *
   * @return true se o método é interceptado ou false caso contrário.
   */
  bool isInterceptable(std::string interfaceRepID
                       , std::string method);

  /**
   * Retorna a política de validação de credenciais.
   * @return Política de validação de credenciais.
   */
  interceptors::CredentialValidationPolicy getCredentialValidationPolicy();

  /**
   * Trata os parâmetros de linha de comando.
   */
  void commandLineParse(int, char**);

  orb_state(int argc, char** argv, const char* host
            , unsigned short port
            , interceptors::CredentialValidationPolicy policy);

  idl_namespace::registry_service::IRegistryService* connect(const char* user
                                                     , const char* password);

  idl_namespace::registry_service::IRegistryService* connect(const char* entity
                                                     , const char* privateKeyFilename
                                                     , const char* ACSCertificateFilename);

  boost::optional<idl_namespace::access_control_service::Credential> getCredential() const;
  PortableServer::POA_var getRootPOA() const;
  idl_namespace::registry_service::IRegistryService_var getRegistryService() const;

  template <typename F>
  lease_connection setLeaseExpiredCallback(F f)
  {
    return lease_expired_signal.connect(f);
  }
  void removeLeaseExpiredCallback(lease_connection c)
  {
    c.disconnect();
  }
};

}

#endif
