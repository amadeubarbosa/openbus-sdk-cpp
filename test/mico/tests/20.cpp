#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include "../util/auxiliar.h"
#include "../stubs/RGSTest.h"

#define TESTCASE "20"

using namespace openbus;
using namespace auxiliar;
using namespace tecgraf::openbus::core::v1_05;
using namespace tecgraf::openbus::core::v1_05::registry_service;

registry_service::IRegistryService* rgs;
access_control_service::Credential* credential;
char* registryIdentifier;
char* registryIdentifier2;
openbus::util::PropertyListHelper* propertyListHelper;
openbus::util::PropertyListHelper* propertyListHelper2;

class RGSTest : virtual public POA_IRGSTest {
    void foo() 
      throw(CORBA::SystemException) 
    {
      std::cout << "Calling foo..." << std::endl;
    };
};

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    Openbus* bus = Openbus::getInstance();
    const char* _args[] = {
      "exec", 
      "-OpenbusHost", 
      getServerHost().c_str(),
      "-OpenbusPort", 
      getServerPort().c_str(),
      "-OpenbusDebug",
      getOpenbusDebug(),
      "-OpenbusTimeRenewing",
      "2"};
    bus->init(9, (char**) _args);
    rgs = bus->connect(
     "TesteBarramento", 
     "TesteBarramento.key", 
     "AccessControlService.crt"); 
    if (!rgs) {
      fail(TESTCASE, "Nao foi possivel obter o servico de registro.");
    }
    
    scs::core::ComponentId id;
    id.name = "RGSTestSuiteComponent";
    id.major_version = '1';
    id.minor_version = '0';
    id.patch_version = '0';
    id.platform_spec = "none";
  
    scs::core::ComponentContext* context = 
      new scs::core::ComponentContext(bus->getORB(), id);
    context->addFacet("IRGSTest", "IDL:IRGSTest:1.0", new RGSTest);
    scs::core::IComponent_var component = context->getIComponent();
  
    openbus::util::PropertyListHelper* propertyListHelper = 
      new openbus::util::PropertyListHelper();
    propertyListHelper->add("description", "blabla");
  
    registry_service::ServiceOffer serviceOffer;
    serviceOffer.properties = propertyListHelper->getPropertyList();
    serviceOffer.member = component;
    try {
      registryIdentifier = rgs->_cxx_register(serviceOffer);
      if (!registryIdentifier) {
        fail(TESTCASE, "Falha no _cxx_register().");
      }
    } catch (UnathorizedFacets& e) {
      cout << "Nao foi possivel registrar a oferta." << endl;
      CORBA::ULong idx;
      CORBA::ULong length = e.facets.length();
      for (idx = 0; idx < length; idx++) {
        cout << "Faceta nao autorizada: " << e.facets[idx] << endl;
      }
      fail(TESTCASE, "UnathorizedFacet");
    }
  
    propertyListHelper2 = new openbus::util::PropertyListHelper();
    serviceOffer.properties = propertyListHelper2->getPropertyList();
    serviceOffer.member = component;
    try {
      registryIdentifier2 = rgs->_cxx_register(serviceOffer);
      if (!registryIdentifier2) {
        fail(TESTCASE, "Falha no _cxx_register().");
      }
    } catch (UnathorizedFacets& e) {
      cout << "Nao foi possivel registrar a oferta." << endl;
      CORBA::ULong idx;
      CORBA::ULong length = e.facets.length();
      for (idx = 0; idx < length; idx++) {
        cout << "Faceta nao autorizada: " << e.facets[idx] << endl;
      }
      fail(TESTCASE, "UnathorizedFacet");
    }

    openbus::util::FacetListHelper* facetListHelper = \
      new openbus::util::FacetListHelper();
    facetListHelper->add("IRGSTest");
    
    registry_service::ServiceOfferList* serviceOfferList = \
      rgs->findByCriteria(
        facetListHelper->getFacetList(),
        propertyListHelper->getPropertyList());
    if (serviceOfferList->length() != 1) {
      fail(TESTCASE, "Deveria existir uma ofertas.");
    }
        
    bus->disconnect();
  } catch (LOGIN_FAILURE& e) {
    fail(TESTCASE, "LOGIN_FAILURE");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exce��o desconhecida");
  }
  finish(TESTCASE);
}
