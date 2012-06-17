#include <openbus.h>
#include <iostream>
#include "stubs/hello.h"

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    {
      std::auto_ptr <openbus::Connection> connBusA (manager->createConnection("localhost", 2089));
      manager->setDefaultConnection(connBusA.get());
      connBusA->loginByPassword("demo", "demo");
      openbus::idl_or::ServicePropertySeq propsA;
      propsA.length(3);
      propsA[0].name  = "openbus.offer.entity";
      propsA[0].value = "demo";
      propsA[1].name  = "openbus.component.facet";
      propsA[1].value = "hello";
      propsA[2].name  = "offer.domain";
      propsA[2].value = "Interoperability Tests";
      openbus::idl_or::ServiceOfferDescSeq_var offersA = connBusA->offers()->findServices(propsA);
      if (offersA->length()) {
        CORBA::Object_var o = offersA[0].service_ref->getFacetByName("hello");
        tecgraf::openbus::interop::simple::Hello* hello = 
          tecgraf::openbus::interop::simple::Hello::_narrow(o);
        hello->sayHello();
      } else std::cout << "nenhuma oferta encontrada." << std::endl;
    }
    {
      std::auto_ptr <openbus::Connection> connBusB (manager->createConnection("localhost", 3090));
      manager->setDefaultConnection(connBusB.get());
      connBusB->loginByPassword("demo", "demo");
      openbus::idl_or::ServicePropertySeq propsB;
      propsB.length(3);
      propsB[0].name  = "openbus.offer.entity";
      propsB[0].value = "demo";
      propsB[1].name  = "openbus.component.facet";
      propsB[1].value = "hello";
      propsB[2].name  = "offer.domain";
      propsB[2].value = "Interoperability Tests";
      openbus::idl_or::ServiceOfferDescSeq_var offersB = connBusB->offers()->findServices(propsB);
      if (offersB->length()) {
        CORBA::Object_var o = offersB[0].service_ref->getFacetByName("hello");
        tecgraf::openbus::interop::simple::Hello* hello = 
          tecgraf::openbus::interop::simple::Hello::_narrow(o);
        hello->sayHello();
      } else std::cout << "nenhuma oferta encontrada." << std::endl;
    }
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
