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

openbus::Openbus *bus;
tecgraf::openbus::core::v1_05::registry_service::IRegistryService *registryService = 0;
scs::core::ComponentContext *component;
char *registryId;
const char *entityName;
const char *privateKeyFilename;
const char *ACSCertificateFilename;
const char *facetName;

struct HelloImpl : virtual public POA_demoidl::hello::IHello {
  void sayHello() {
    std::cout << "Servant diz: HELLO!" << std::endl;
    tecgraf::openbus::core::v1_05::access_control_service::Credential_var credential = 
      bus->getInterceptedCredential();
    std::cout << "Usuario OpenBus que fez a chamada: " << credential->owner.in() << std::endl;
  };
};

void loginAndRegister(scs::core::ComponentContext *component) {
/* Conexao com o barramento atraves de certificado. */
  try {
    std::cout << "Conectando no barramento..." << std::endl;
    registryService = bus->connect(entityName, privateKeyFilename, ACSCertificateFilename);
    std::cout << "Conexao com o barramento estabelecida com sucesso!" << std::endl;
  } catch (const CORBA::SystemException &) {
    std::cout << "** Nao foi possivel se conectar ao barramento. **" << std::endl
              << "* Falha na comunicacao. *" << std::endl;
  } catch (const openbus::LOGIN_FAILURE &) {
    std::cout << "** Nao foi possivel se conectar ao barramento. **" << std::endl
              << "* Par usuario/senha invalido. *" << std::endl;
  } catch (const openbus::SECURITY_EXCEPTION &e) {
    std::cout << e.what() << std::endl;
  }
  openbus::util::PropertyListHelper propertyListHelper;

/* Criacao de uma *oferta de servico*. */
  tecgraf::openbus::core::v1_05::registry_service::ServiceOffer serviceOffer;
  serviceOffer.properties = propertyListHelper.getPropertyList();
  serviceOffer.member = component->getIComponent();

/* Registro do servico no barramento. */
  try {
    if (registryService) {
      std::cout << "Registrando servico IHello no barramento..." << std::endl;
      registryId = registryService->_cxx_register(serviceOffer);
      std::cout << "Servico IHello registrado." << std::endl;
    } else 
      std::cout << "Nao foi possivel adquirir um proxy para o servico de registro." << std::endl;
  } catch (const tecgraf::openbus::core::v1_05::registry_service::UnathorizedFacets &e) {
    std::cout << "Nao foi possivel registrar IHello." << std::endl;
    for (CORBA::ULong idx = 0; idx < e.facets.length(); ++idx)
      std::cout << "Faceta nao autorizada: " << e.facets[idx] << std::endl;
  }
}

struct RenewLogin : public openbus::Openbus::LeaseExpiredCallback {
  void expired() {
    try {
      bus->disconnect();
      loginAndRegister(component);
    } catch (const CORBA::Exception &) {
      std::cout << "Nao foi possivel desconectar e conectar novamente." << std::endl;
    }
  }
};

void termination_handler(int p) {
  std::cout << "Encerrando o processo servidor..." << std::endl;
  openbus::Openbus::terminationHandlerCallback((long) signal);
}

void commandLineParse(int argc, char **argv) {
  for (short i = 1; i < argc; ++i)
    if (!strcmp(argv[i], "-EntityName"))
      entityName = argv[++i];
    else if (!strcmp(argv[i], "-PrivateKeyFilename"))
      privateKeyFilename = argv[++i];
    else if (!strcmp(argv[i], "-ACSCertificateFilename"))
      ACSCertificateFilename = argv[++i];
    else if (!strcmp(argv[i], "-FacetName"))
      facetName = argv[++i];
}

int main(int argc, char **argv) {
  RenewLogin renewLogin;
  signal(SIGINT, termination_handler);
  bus = openbus::Openbus::getInstance();
  commandLineParse(argc, argv);
  bus->init(argc, argv);
  bus->setLeaseExpiredCallback(&renewLogin);

/* Definicao do componente. */
  scs::core::ComponentId componentId;
  componentId.name = "HelloComponent";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "nenhuma";
  component = new scs::core::ComponentContext(bus->getORB(), componentId);

  std::auto_ptr<PortableServer::ServantBase> facet(new HelloImpl);
  component->addFacet(facetName, "IDL:demoidl/hello/IHello:1.0", facet);
  assert(!facet.get());
  loginAndRegister(component);
  std::cout << "Aguardando requisicoes..." << std::endl;
  bus->run();
  return 0;
}
