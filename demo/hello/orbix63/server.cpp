/*
** OpenBus Demo - Orbix 6.3
** server.cpp
*/

#include <fstream>
#include <iostream>
#include <omg/orb.hh>
#include <it_ts/thread.h>
#include <it_ts/termination_handler.h>

#include <openbus.h>
#include <ComponentBuilder.h>

#include "stubs/helloS.hh"

using namespace std;
using namespace tecgraf::openbus::core::v1_05::registry_service;

openbus::Openbus* bus;
registry_service::IRegistryService* registryService;
char* registryId;
scs::core::ComponentContext* componentContext;

class HelloImpl : virtual public POA_demoidl::hello::IHello {
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
    void sayHello() 
      throw(CORBA::SystemException) {
      cout << "Servant diz: HELLO!" << endl;
      access_control_service::Credential_var credential = 
        bus->getInterceptedCredential();
      cout << "Usuario OpenBus que fez a chamada: " << credential->owner.in()
        << endl;
    };
};

static void myTerminationHandler(long signal) {
  cout << "Encerrando o processo servidor..." << endl;
  try {
    registryService->unregister(registryId);
  } catch(CORBA::Exception& e) {
    cout << "N�o foi poss�vel remover a oferta de servi�o." << endl;
  }

  scs::core::ComponentBuilder* componentBuilder = bus->getComponentBuilder();
  map<string, string>* errors = componentBuilder->deactivateComponent(componentContext);
  map<string, string>::iterator it;
  for ( it = errors->begin() ; it != errors->end(); it++ ) {
    string error = (string) (*it).second;
    cout << "Erro ao desativar a faceta " << error << "." << endl;
  }
  delete errors;
  delete componentContext;
  openbus::Openbus::terminationHandlerCallback(signal);
}

int main(int argc, char* argv[]) {
  IT_TerminationHandler termination_handler(myTerminationHandler);

  bus = openbus::Openbus::getInstance();

  bus->init(argc, argv);

  cout << "Conectando no barramento..." << endl;

/* Conex�o com o barramento atrav�s de certificado. */
  try {
    registryService = bus->connect("HelloService", "HelloService.key",
      "AccessControlService.crt");
  } catch (CORBA::SystemException& e) {
    cout << "** N�o foi poss�vel se conectar ao barramento. **" << endl \
         << "* Falha na comunica��o. *" << endl;
    exit(-1);
  } catch (openbus::LOGIN_FAILURE& e) {
    cout << "** N�o foi poss�vel se conectar ao barramento. **" << endl \
         << "* Par usu�rio/senha inv�lido. *" << endl;
    exit(-1);
  } catch (openbus::SECURITY_EXCEPTION& e) {
    cout << e.what() << endl;
    exit(-1);
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
  helloDesc.interface_name = "IDL:demoidl/hello/IHello:1.0";
  helloDesc.instantiator = HelloImpl::instantiate;
  helloDesc.destructor = HelloImpl::destruct;
  extFacets.push_back(helloDesc);
  componentContext = componentBuilder->newComponent(extFacets, componentId);

  openbus::util::PropertyListHelper* propertyListHelper = \
    new openbus::util::PropertyListHelper();

/* Cria��o de uma *oferta de servi�o*. */
  openbus::util::ServiceOffer serviceOffer;
  serviceOffer.properties = propertyListHelper->getPropertyList();
  serviceOffer.member = componentContext->getIComponent();
  delete propertyListHelper;

  cout << "Registrando servi�o IHello no barramento..." << endl;

/* Registro do servi�o no barramento. */
  try {
    registryId = registryService->_cxx_register(serviceOffer);
  } catch (UnathorizedFacets& e) {
    cout << "Nao foi possivel registrar IHello." << endl;
    CORBA::ULong idx;
    CORBA::ULong length = e.facets.length();
    for (idx = 0; idx < length; idx++) {
      cout << "Faceta nao autorizada: " << e.facets[idx] << endl;
    }
    exit(-1);
  }
  cout << "Servi�o IHello registrado." << endl;
  cout << "Aguardando requisi��es..." << endl;

  bus->run();

  return 0;
}

