/*
* OpenBus Demo - delegate/multithread/mico
* server.cpp
*/

#include <fstream>
#include <iostream>
#include <CORBA.h>
#include <csignal>

#include <openbus.h>
#include <scs/ComponentContext.h>

#include "delegate.h"

using namespace std;
using namespace tecgraf::openbus::core::v1_05;
using namespace tecgraf::openbus::core::v1_05::registry_service;

openbus::Openbus* bus;
registry_service::IRegistryService* registryService = 0;
char* registryId;
scs::core::ComponentContext* componentContext = 0;
const char* entityName;
const char* privateKeyFilename;
const char* ACSCertificateFilename;
const char* facetName;

struct HelloImpl : virtual public POA_demoidl::demoDelegate::IHello {
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
  openbus::Openbus::terminationHandlerCallback((long) signal);
}

void commandLineParse(int argc, char* argv[]) {
  for (short i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-EntityName")) {
      i++;
      entityName = argv[i];
    } else if (!strcmp(argv[i], "-PrivateKeyFilename")) {
      i++;
      privateKeyFilename = argv[i];
    } else if (!strcmp(argv[i], "-ACSCertificateFilename")) {
      i++;
      ACSCertificateFilename = argv[i];
    } else if (!strcmp(argv[i], "-FacetName")) {
      i++;
      facetName = argv[i];
    } 
  }
}

int main(int argc, char* argv[]) {
  signal(SIGINT, termination_handler);
  bus = openbus::Openbus::getInstance();

  bus->init(argc, argv);

  commandLineParse(argc, argv);

  cout << "Conectando no barramento..." << endl;

/* Conexao com o barramento atraves de certificado. */
  try {
    registryService = bus->connect(
      entityName, 
      privateKeyFilename,
      ACSCertificateFilename);
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

/* Definição do componente. */
  scs::core::ComponentId componentId;
  componentId.name = "HelloComponent";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "nenhuma";

  ::componentContext = new scs::core::ComponentContext(bus->getORB(), componentId);
  std::auto_ptr<PortableServer::ServantBase> facet(new HelloImpl);
  ::componentContext->addFacet(facetName, "IDL:demoidl/demoDelegate/IHello:1.0", facet);
  assert(facet.get() == 0);

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
