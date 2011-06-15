/*
* rgs/RGSTestSuite.cpp
*/

#ifndef RGS_TESTSUITE_H
#define RGS_TESTSUITE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <cxxtest/TestSuite.h>
#include <openbus.h>
#include "stubs/RGSTestS.hh"

using namespace openbus;
using namespace tecgraf::openbus::core::v1_05;
using namespace tecgraf::openbus::core::v1_05::registry_service;

class RGSTest : virtual public POA_IRGSTest {
  private:
    scs::core::ComponentContext* componentContext;
    RGSTest(scs::core::ComponentContext* componentContext) {
      this->componentContext = componentContext;
    }
  public:
    static PortableServer::ServantBase* instantiate(
      scs::core::ComponentContext* componentContext) 
    {
      return (PortableServer::ServantBase*) new RGSTest(componentContext);
    }
    static void destruct(void* obj) {
      delete (RGSTest*) obj;
    }
    void foo() 
      throw(CORBA::SystemException) 
    {
    };
};

class RGSTestSuite: public CxxTest::TestSuite {
  private:
    Openbus* bus;
    access_control_service::IAccessControlService* iAccessControlService;
    registry_service::IRegistryService* rgs;
    access_control_service::Credential* credential;
    access_control_service::Lease lease;
    char* registryIdentifier;
    char* registryIdentifier2;
    openbus::util::PropertyListHelper* propertyListHelper;
    openbus::util::PropertyListHelper* propertyListHelper2;
    scs::core::IComponent_var component;
    std::string OPENBUS_SERVER_HOST;
    std::string OPENBUS_SERVER_PORT;
    std::string OPENBUS_USERNAME;
    std::string OPENBUS_PASSWORD;
    scs::core::ComponentBuilder* componentBuilder;
    scs::core::ComponentContext* context;

    void fillComponentId(scs::core::ComponentId& id) {
      id.name = "RGSTestSuiteComponent";
      id.major_version = '1';
      id.minor_version = '0';
      id.patch_version = '0';
      id.platform_spec = "none";
    }

  public:
    RGSTestSuite() {
      try {
        std::string OPENBUS_HOME = getenv("OPENBUS_HOME");
        OPENBUS_HOME += "/test/orbix/config.txt";
        std::string temp;
        std::ifstream inFile;
        inFile.open(OPENBUS_HOME.c_str());
        if (!inFile) {
          temp = "Não foi possível carregar o arquivo " + OPENBUS_HOME + ".";
          TS_FAIL( temp );
        }
        while (inFile >> temp) {
          if (temp.compare("OPENBUS_SERVER_HOST") == 0) {
            inFile >> temp; // le o '='
            inFile >> OPENBUS_SERVER_HOST; // le o valor
          }
          if (temp.compare("OPENBUS_SERVER_PORT") == 0) {
            inFile >> temp;
            inFile >> OPENBUS_SERVER_PORT;
          }
          if (temp.compare("OPENBUS_USERNAME") == 0) {
            inFile >> temp;
            inFile >> OPENBUS_USERNAME;
          }
          if (temp.compare("OPENBUS_PASSWORD") == 0) {
            inFile >> temp;
            inFile >> OPENBUS_PASSWORD;
          }
        }
        inFile.close();
      }
      catch ( const char* errmsg ) {
        TS_FAIL( errmsg );
      }
    }

    ~RGSTestSuite() {
      try {
        if (bus) {
          if (bus->disconnect())
            delete bus;
        }
        delete propertyListHelper;
        delete propertyListHelper2;
        delete credential;
      }
      catch (const char* errmsg ) {
        TS_FAIL(errmsg);
      }
    }

    void setUP() {
    }

    void tearDown() {
    }

    void testGetRegisterService() {
      try {
        bus = Openbus::getInstance();
        const char* argv[] = {
          "exec", 
          "-OpenbusHost", 
          const_cast<char*>(OPENBUS_SERVER_HOST.c_str()), 
          "-OpenbusPort", 
          OPENBUS_SERVER_PORT.c_str(),
          "-OpenbusDebug",
          "ALL"}; 
        bus->init(7, (char**) argv);
        credential = new access_control_service::Credential;
        rgs = bus->connect(
         "TesteBarramento", 
         "TesteBarramento.key", 
         "AccessControlService.crt"); 
        iAccessControlService = bus->getAccessControlService();
        rgs = bus->getRegistryService();
        TS_ASSERT(rgs);
      } catch (const char* errmsg) {
        TS_FAIL(errmsg);
      }
    }

    void testRegister() {
      try {
        componentBuilder = bus->getComponentBuilder();

        scs::core::ComponentId id;
        fillComponentId(id);

      /* Descrição das facetas. */
        std::list<scs::core::ExtendedFacetDescription> extFacets;
        scs::core::ExtendedFacetDescription desc;
        desc.name = "IRGSTest";
        desc.interface_name = "IDL:IRGSTest:1.0";
        desc.instantiator = RGSTest::instantiate;
        desc.destructor = RGSTest::destruct;
        extFacets.push_back(desc);

        context = componentBuilder->newComponent(extFacets, id);
        component = context->getIComponent();

        propertyListHelper = new openbus::util::PropertyListHelper();
        propertyListHelper->add("description", "blabla");

        registry_service::ServiceOffer serviceOffer;
        serviceOffer.properties = propertyListHelper->getPropertyList();
        serviceOffer.member = component;
        try {
          registryIdentifier = rgs->_cxx_register(serviceOffer);
          TS_ASSERT(registryIdentifier);
        } catch (UnauthorizedFacets& e) {
          cout << "Nao foi possivel registrar a oferta." << endl;
          CORBA::ULong idx;
          CORBA::ULong length = e.facets.length();
          for (idx = 0; idx < length; idx++) {
            cout << "Faceta nao autorizada: " << e.facets[idx] << endl;
          }

          exit(-1);
        }

        propertyListHelper2 = new openbus::util::PropertyListHelper();
        serviceOffer.properties = propertyListHelper2->getPropertyList();
        serviceOffer.member = component;
        try {
          registryIdentifier2 = rgs->_cxx_register(serviceOffer);
          TS_ASSERT(registryIdentifier2);
        } catch (UnauthorizedFacets& e) {
          cout << "Nao foi possivel registrar a oferta." << endl;
          CORBA::ULong idx;
          CORBA::ULong length = e.facets.length();
          for (idx = 0; idx < length; idx++) {
            cout << "Faceta nao autorizada: " << e.facets[idx] << endl;
          }

          exit(-1);
        }
      } catch (const char* errmsg) {
        TS_FAIL(errmsg);
      }
    }

    void testFind() {
      openbus::util::FacetListHelper* facetListHelper = \
        new openbus::util::FacetListHelper();
      facetListHelper->add("IRGSTest");

      registry_service::ServiceOfferList* serviceOfferList = 
        rgs->find(facetListHelper->getFacetList());
      TS_ASSERT(serviceOfferList->length() == 2);
      delete serviceOfferList;
      delete facetListHelper;
    }

    void testFindByCriteria() {
      openbus::util::FacetListHelper* facetListHelper = \
        new openbus::util::FacetListHelper();
      facetListHelper->add("IRGSTest");

      registry_service::ServiceOfferList* serviceOfferList = \
        rgs->findByCriteria(
          facetListHelper->getFacetList(),
          propertyListHelper->getPropertyList());
      TS_ASSERT(serviceOfferList->length() == 1);
      delete serviceOfferList;
      delete facetListHelper;
    }
    
    void testSetInterceptable() {
      openbus::util::FacetListHelper* facetListHelper = new openbus::util::FacetListHelper();
      facetListHelper->add("IRGSTest");
      
      registry_service::ServiceOfferList_var serviceOfferList = rgs->find(facetListHelper->getFacetList());
      registry_service::ServiceOffer serviceOffer = serviceOfferList[(CORBA::ULong) 0];
      scs::core::IComponent_var component = serviceOffer.member;
      CORBA::Object_var obj = component->getFacet("IDL:IRGSTest:1.0");
      TS_ASSERT(rgs->unregister(registryIdentifier));
      TS_ASSERT(rgs->unregister(registryIdentifier2));
     bus->setInterceptable("IDL:IRGSTest:1.0", "foo", false);
      iAccessControlService->logout(*(bus->getCredential()));
      CORBA::Request_var request = obj->_request("foo");
      request->invoke();
    }

    void testUnregister() {
      TS_ASSERT(!rgs->unregister((char*) "ID"));
    }
};

#endif

