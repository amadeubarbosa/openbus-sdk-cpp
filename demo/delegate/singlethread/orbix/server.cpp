/*
* OpenBus Demo - delegate/singlethread/orbix
* server.cpp
*/

#include <fstream>
#include <iostream>
#include <omg/orb.hh>
#include <it_ts/thread.h>
#include <it_ts/termination_handler.h>

#include <openbus.h>
#include <scs/ComponentContext.h>

#include "delegateS.hh"

using namespace std;
using namespace tecgraf::openbus::core::v1_05::registry_service;

openbus::Openbus* bus;
registry_service::IRegistryService* registryService = 0;
char* registryId;
scs::core::ComponentContext* componentContext;
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

static void myTerminationHandler(long signal) {
  cout << "Encerrando o processo servidor..." << endl;
  delete componentContext;
  openbus::Openbus::terminationHandlerCallback(signal);
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
  IT_TerminationHandler termination_handler(myTerminationHandler);

  bus = openbus::Openbus::getInstance();

  bus->init(argc, argv);

  commandLineParse(argc, argv);

  cout << "Conectando no barramento..." << endl;

/* Conexão com o barramento através de certificado. */
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

  componentContext = new scs::core::ComponentContext(bus->getORB(), componentId);

  componentContext->addFacet(facetName, "IDL:demoidl/demoDelegate/IHello:1.0", new HelloImpl);

  openbus::util::PropertyListHelper* propertyListHelper = \
    new openbus::util::PropertyListHelper();

/* Criação de uma *oferta de serviço*. */
  openbus::util::ServiceOffer serviceOffer;
  serviceOffer.properties = propertyListHelper->getPropertyList();
  serviceOffer.member = componentContext->getIComponent();
  delete propertyListHelper;

  cout << "Registrando serviço IHello no barramento..." << endl;

/* Registro do serviço no barramento. */
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
  cout << "Serviço IHello registrado." << endl;
  cout << "Aguardando requisições..." << endl;

  bus->run();

  return 0;
}

