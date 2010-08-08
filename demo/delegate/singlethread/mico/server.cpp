/*
* OpenBus Demo - delegate/singlethread/mico
* server.cpp
*/

#include <fstream>
#include <iostream>
#include <CORBA.h>
#include <csignal>

#include <openbus.h>
#include <ComponentBuilder.h>

#include "stubs/delegate.h"

using namespace std;
using namespace tecgraf::openbus::core::v1_05;
using namespace tecgraf::openbus::core::v1_05::registry_service;

openbus::Openbus* bus;
registry_service::IRegistryService* registryService = 0;
char* registryId;
scs::core::ComponentContext* componentContext;

class HelloImpl : virtual public POA_demoidl::demoDelegate::IHello {
  private:
    scs::core::ComponentContext* componentContext;
    HelloImpl(scs::core::ComponentContext* componentContext) {
      this->componentContext = componentContext;
    }
  public:
    static PortableServer::ServantBase* instantiate(
      scs::core::ComponentContext* componentContext) 
    {
      return (PortableServer::ServantBase*) new HelloImpl(componentContext);
    }
    static void destruct(void* obj) {
      delete (HelloImpl*) obj;
    }
    void sayHello(const char* name) throw(CORBA::SystemException) {
      cout << "Servant diz: " << name << endl;
      access_control_service::Credential_var credential = bus->getInterceptedCredential();
      cout << "Usu�rio OpenBus que fez a chamada: " << endl 
           << "[owner] " << credential->owner.in() << endl 
           << "[identifier] " << credential->identifier.in() << endl 
           << "[delegate] " << credential->delegate.in() << endl;
    };
};

void termination_handler(int p) {
  cout << "Encerrando o processo servidor..." << endl;
  if (registryService) {
    try {
      registryService->unregister(registryId);
      registryService = 0;
    } catch(CORBA::Exception& e) {
      cout << "Nao foi possivel remover a oferta de servico." << endl;
    }
  }
  openbus::Openbus::terminationHandlerCallback((long) signal);
}

int main(int argc, char* argv[]) {
  signal(SIGINT, termination_handler);
  bus = openbus::Openbus::getInstance();

  bus->init(argc, argv);

  cout << "Conectando no barramento..." << endl;

/* Conexao com o barramento atraves de certificado. */
  try {
    registryService = bus->connect(
      "DelegateService", 
      "DelegateService.key", 
      "AccessControlService.crt");
  } catch (CORBA::SystemException& e) {
    cout << "** N�o foi poss�vel se conectar ao barramento. **" << endl \
         << "* Falha na comunica��o. *" << endl;
    exit(1);
  } catch (openbus::LOGIN_FAILURE& e) {
    cout << "** N�o foi poss�vel se conectar ao barramento. **" << endl \
         << "* Par usu�rio/senha inv�lido. *" << endl;
    exit(1);
  } catch (openbus::SECURITY_EXCEPTION& e) {
    cout << e.what() << endl;
    exit(1);
  }

  cout << "Conex�o com o barramento estabelecida com sucesso!" << endl;

/* F�brica de componentes */
  scs::core::ComponentBuilder* componentBuilder = bus->getComponentBuilder();

/* Defini��o do componente. */
  scs::core::ComponentId componentId;
  componentId.name = "HelloComponent";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "nenhuma";

/* Descri��o das facetas. */
  std::list<scs::core::ExtendedFacetDescription> extFacets;
  scs::core::ExtendedFacetDescription helloDesc;
  helloDesc.name = "IHello";
  helloDesc.interface_name = "IDL:demoidl/demoDelegate/IHello:1.0";
  helloDesc.instantiator = HelloImpl::instantiate;
  helloDesc.destructor = HelloImpl::destruct;
  extFacets.push_back(helloDesc);
  componentContext = componentBuilder->newComponent(extFacets, componentId);

  openbus::util::PropertyListHelper* propertyListHelper = \
    new openbus::util::PropertyListHelper();

/* Cria��o de uma *oferta de servi�o*. */
  registry_service::ServiceOffer serviceOffer;
  serviceOffer.properties = propertyListHelper->getPropertyList();
  serviceOffer.member = componentContext->getIComponent();
  delete propertyListHelper;

  cout << "Registrando servi�o IHello no barramento..." << endl;

/* Registro do servi�o no barramento. */
  try {
    if (registryService) {
      registryId = registryService->_cxx_register(serviceOffer);
    } else {
      cout << "N�o foi poss�vel adquirir um proxy para o servi�o de registro." << endl;
      exit(1);
    }
  } catch (UnathorizedFacets& e) {
    cout << "N�o foi poss�vel registrar IHello." << endl;
    CORBA::ULong idx;
    CORBA::ULong length = e.facets.length();
    for (idx = 0; idx < length; idx++) {
      cout << "Faceta n�o autorizada: " << e.facets[idx] << endl;
    }
    exit(1);
  }
  cout << "Servi�o IHello registrado." << endl;
  cout << "Aguardando requisi��es..." << endl;

  bus->run();

  return 0;
}
