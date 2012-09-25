/*
** OpenBus Demo - Mico
** server.cpp
*/

#include <fstream>
#include <iostream>
#include <CORBA.h>
#include <csignal>

#include <openbus.h>
#include <scs/ComponentContext.h>

#include "stubs/hello.h"

using namespace std;
using namespace tecgraf::openbus::core::v1_05;
using namespace tecgraf::openbus::core::v1_05::registry_service;

openbus::Openbus* bus;
registry_service::IRegistryService* registryService = 0;
char* registryId;
scs::core::ComponentContext* componentContext;
const char* entityName;
const char* privateKeyFilename;
const char* ACSCertificateFilename;
const char* facetName;

struct HelloImpl : virtual public POA_demoidl::hello::IHello {
    void sayHello() throw(CORBA::SystemException) {
      cout << "Servant diz: HELLO!" << endl;
      access_control_service::Credential_var credential = 
        bus->getInterceptedCredential();
      cout << "Usuario OpenBus que fez a chamada: " << credential->owner.in()
        << endl;
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

  commandLineParse(argc, argv);

  bus->init(argc, argv);

  cout << "Conectando no barramento..." << endl;

/* Conexao com o barramento atraves de certificado. */
  try {
    registryService = bus->connect(
      entityName, 
      privateKeyFilename,
      ACSCertificateFilename);
  } catch (CORBA::SystemException& e) {
    cout << "** Nao foi possivel se conectar ao barramento. **" << endl \
         << "* Falha na comunicacao. *" << endl;
    exit(1);
  } catch (openbus::LOGIN_FAILURE& e) {
    cout << "** Nao foi possivel se conectar ao barramento. **" << endl \
         << "* Par usuario/senha invalido. *" << endl;
    exit(1);
  } catch (openbus::SECURITY_EXCEPTION& e) {
    cout << e.what() << endl;
    exit(1);
  }

  cout << "Conexao com o barramento estabelecida com sucesso!" << endl;

/* Definicao do componente. */
  scs::core::ComponentId componentId;
  componentId.name = "HelloComponent";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "nenhuma";

  ::componentContext = new scs::core::ComponentContext(bus->getORB(), componentId);

  std::auto_ptr<PortableServer::ServantBase> facet(new HelloImpl);
  ::componentContext->addFacet(facetName, "IDL:demoidl/hello/IHello:1.0", facet);
  assert(facet.get() == 0);

// /* Descricao das facetas. */
//   std::list<scs::core::ExtendedFacetDescription> extFacets;
//   scs::core::ExtendedFacetDescription helloDesc;
//   helloDesc.name = facetName;
//   helloDesc.interface_name = "IDL:demoidl/hello/IHello:1.0";
//   helloDesc.instantiator = HelloImpl::instantiate;
//   helloDesc.destructor = HelloImpl::destruct;
//   extFacets.push_back(helloDesc);
//   componentContext = componentBuilder->newComponent(extFacets, componentId);

  openbus::util::PropertyListHelper* propertyListHelper = \
    new openbus::util::PropertyListHelper();

/* Criacao de uma *oferta de servico*. */
  registry_service::ServiceOffer serviceOffer;
  serviceOffer.properties = propertyListHelper->getPropertyList();
  serviceOffer.member = componentContext->getIComponent();
  delete propertyListHelper;

  cout << "Registrando servico IHello no barramento..." << endl;

/* Registro do servico no barramento. */
  try {
    if (registryService) {
      registryId = registryService->_cxx_register(serviceOffer);
    } else {
      cout << "Nao foi possivel adquirir um proxy para o servico de registro." 
        << endl;
      exit(1);
    }
  } catch (UnathorizedFacets& e) {
    cout << "Nao foi possivel registrar IHello." << endl;
    CORBA::ULong idx;
    CORBA::ULong length = e.facets.length();
    for (idx = 0; idx < length; idx++) {
      cout << "Faceta nao autorizada: " << e.facets[idx] << endl;
    }
    exit(1);
  }
  cout << "Servico IHello registrado." << endl;
  cout << "Aguardando requisicoes..." << endl;

  bus->run();

  return 0;
}
