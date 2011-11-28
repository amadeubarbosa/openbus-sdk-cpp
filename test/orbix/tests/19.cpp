#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <unistd.h>
#include "../util/auxiliar.h"
#include "../stubs/RGSTestS.hh"

#define TESTCASE "19"

using namespace openbus;
using namespace auxiliar;
using namespace tecgraf::openbus::core::v1_05;
using namespace tecgraf::openbus::core::v1_05::registry_service;
stringstream offerId;
stringstream entityName;
stringstream privateKeyFilename;

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
      getServerHost(),
      "-OpenbusPort", 
      getServerPort(),
      "-OpenbusDebug",
      getOpenbusDebug(),
      "-OpenbusTimeRenewing",
      "2"};
    bus->init(9, (char**) _args);
    entityName << "TesteBarramento" << getenv("TEC_UNAME");
    privateKeyFilename << "TesteBarramento" << getenv("TEC_UNAME") << ".key";
    rgs = bus->connect(
      entityName.str().c_str(), 
      privateKeyFilename.str().c_str(), 
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
    offerId << getenv("TEC_UNAME") << TESTCASE << getpid();
    propertyListHelper->add("id", offerId.str().c_str());
  
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
    
    registry_service::ServiceOfferList* serviceOfferList = 
      rgs->find(facetListHelper->getFacetList());
    if (serviceOfferList->length() != 2) {
      fail(TESTCASE, "Deveria existir duas ofertas.");
    }
    
    bus->disconnect();
  } catch (LOGIN_FAILURE& e) {
    fail(TESTCASE, "LOGIN_FAILURE");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  finish(TESTCASE);
}
