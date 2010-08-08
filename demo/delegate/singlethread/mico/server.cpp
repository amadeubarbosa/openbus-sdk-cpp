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
      cout << "Usuário OpenBus que fez a chamada: " << endl 
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
    cout << "** Não foi possível se conectar ao barramento. **" << endl \
         << "* Falha na comunicação. *" << endl;
    exit(1);
  } catch (openbus::LOGIN_FAILURE& e) {
    cout << "** Não foi possível se conectar ao barramento. **" << endl \
         << "* Par usuário/senha inválido. *" << endl;
    exit(1);
  } catch (openbus::SECURITY_EXCEPTION& e) {
    cout << e.what() << endl;
    exit(1);
  }

  cout << "Conexão com o barramento estabelecida com sucesso!" << endl;

/* Fábrica de componentes */
  scs::core::ComponentBuilder* componentBuilder = bus->getComponentBuilder();

/* Definição do componente. */
  scs::core::ComponentId componentId;
  componentId.name = "HelloComponent";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "nenhuma";

/* Descrição das facetas. */
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

/* Criação de uma *oferta de serviço*. */
  registry_service::ServiceOffer serviceOffer;
  serviceOffer.properties = propertyListHelper->getPropertyList();
  serviceOffer.member = componentContext->getIComponent();
  delete propertyListHelper;

  cout << "Registrando serviço IHello no barramento..." << endl;

/* Registro do serviço no barramento. */
  try {
    if (registryService) {
      registryId = registryService->_cxx_register(serviceOffer);
    } else {
      cout << "Não foi possível adquirir um proxy para o serviço de registro." << endl;
      exit(1);
    }
  } catch (UnathorizedFacets& e) {
    cout << "Não foi possível registrar IHello." << endl;
    CORBA::ULong idx;
    CORBA::ULong length = e.facets.length();
    for (idx = 0; idx < length; idx++) {
      cout << "Faceta não autorizada: " << e.facets[idx] << endl;
    }
    exit(1);
  }
  cout << "Serviço IHello registrado." << endl;
  cout << "Aguardando requisições..." << endl;

  bus->run();

  return 0;
}
