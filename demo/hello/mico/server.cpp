/*
** OpenBus Demo - Mico
** server.cpp
*/

#include <fstream>
#include <iostream>
#include <CORBA.h>
#include <csignal>

#include <openbus/openbus.h>
#include <ComponentBuilder.h>

#include <boost/optional.hpp>

#include "stubs/hello.h"

using namespace std;
using namespace tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE;
using namespace tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE ::registry_service;

boost::optional<openbus::Openbus&> bus;
registry_service::IRegistryService* registryService = 0;
char* registryId;
scs::core::ComponentContext* componentContext;

class HelloImpl : virtual public POA_demoidl::hello::IHello
{
  private:
    scs::core::ComponentContext* componentContext;
    HelloImpl(scs::core::ComponentContext* componentContext)
    {
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
    void sayHello() throw(CORBA::SystemException) {
      cout << "Servant diz: HELLO!" << endl;
      assert(!!bus);
      access_control_service::Credential_var credential = 
        bus->getInterceptedCredential();
      access_control_service::Credential* credential_ptr = credential;
      if(credential_ptr)
        cout << "Usuario OpenBus que fez a chamada: " << credential->owner.in() << endl;
      else
        std::cout << "Intercepted Credential is nil" << std::endl;
    };
};

// void termination_handler(int p) {
//   cout << "Encerrando o processo servidor..." << endl;
//   openbus::Openbus::terminationHandlerCallback((long) signal);
// }

int main(int argc, char* argv[]) {
  //signal(SIGINT, termination_handler);
  openbus::Openbus bus(argc, argv);
  ::bus = bus;
  cout << "Conectando no barramento..." << endl;

/* Conexao com o barramento atraves de certificado. */
  try
  {
    registryService = bus.connect("HelloService", "HelloService.key",
      "AccessControlService.crt");
  }
  catch (CORBA::SystemException& e)
  {
    cout << "** Nao foi possivel se conectar ao barramento. **" << endl \
         << "* Falha na comunicacao. * " << e << endl;
    exit(1);
  }
  catch (openbus::login_error& e)
  {
    cout << "** Nao foi possivel se conectar ao barramento. **" << endl \
         << "* Par usuario/senha invalido. *" << endl;
    exit(1);
  }
  catch (openbus::security_error& e)
  {
    cout << e.what() << endl;
    exit(1);
  }

  cout << "Conexao com o barramento estabelecida com sucesso!" << endl;

/* Fabrica de componentes */
  scs::core::ComponentBuilder& componentBuilder = bus.getComponentBuilder();

/* Definicao do componente. */
  scs::core::ComponentId componentId;
  componentId.name = "HelloComponent";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "nenhuma";

/* Descricao das facetas. */
  std::list<scs::core::ExtendedFacetDescription> extFacets;
  scs::core::ExtendedFacetDescription helloDesc;
  helloDesc.name = "IHello";
  helloDesc.interface_name = "IDL:demoidl/hello/IHello:1.0";
  helloDesc.instantiator = HelloImpl::instantiate;
  helloDesc.destructor = HelloImpl::destruct;
  extFacets.push_back(helloDesc);
  componentContext = componentBuilder.newComponent(extFacets, componentId);

  openbus::util::PropertyListHelper propertyListHelper;
/* Criacao de uma *oferta de servico*. */
  registry_service::ServiceOffer serviceOffer;
  serviceOffer.properties = propertyListHelper.getPropertyList();
  serviceOffer.member = componentContext->getIComponent();

  std::cout << "calling ourselves" << std::endl;

  scs::core::IComponent_var comp = componentContext->getIComponent();
  CORBA::Object_var facet = comp->getFacet("IDL:demoidl/hello/IHello:1.0");
  if(!CORBA::is_nil(facet))
  {
    demoidl::hello::IHello_var hello = demoidl::hello::IHello::_narrow(facet);
    hello->sayHello();
  }

  cout << "Registrando servico IHello no barramento..." << endl;

/* Registro do servico no barramento. */
  try {
    if (registryService) {
      registryId = registryService->_cxx_register(serviceOffer);
      std::cout << registryId << std::endl;
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

  bus.run();

  ::bus = boost::none;

  return 0;
}
