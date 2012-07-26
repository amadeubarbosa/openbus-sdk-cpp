#include <openbus/ORBInitializer.h>
#include <openbus/log.h>
#include <openbus/ConnectionManager.h>
#include <iostream>
#include "stubs/hello.h"

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB *orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::ConnectionManager *manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    {
      std::auto_ptr <openbus::Connection> connBusA (manager->createConnection("localhost", 2089));
      manager->setDefaultConnection(connBusA.get());
      connBusA->loginByPassword("demo", "demo");
      openbus::idl_or::ServicePropertySeq propsA;
      propsA.length(2);
      CORBA::ULong i = 0;
      propsA[i].name  = "openbus.component.interface";
      propsA[i].value = "IDL:tecgraf/openbus/interop/simple/Hello:1.0";
      propsA[i+1].name  = "offer.domain";
      propsA[i+1].value = "Interoperability Tests";
      openbus::idl_or::ServiceOfferDescSeq_var offersA = connBusA->offers()->findServices(propsA);
      if (offersA->length()) {
        CORBA::Object_var o = offersA[1].service_ref->getFacetByName("hello");
        tecgraf::openbus::interop::simple::Hello *hello = 
          tecgraf::openbus::interop::simple::Hello::_narrow(o);
        hello->sayHello();
      } else std::cout << "nenhuma oferta encontrada." << std::endl;
    }
    {
      std::auto_ptr <openbus::Connection> connBusB (manager->createConnection("localhost", 3090));
      manager->setDefaultConnection(connBusB.get());
      connBusB->loginByPassword("demo", "demo");
      openbus::idl_or::ServicePropertySeq propsB;
      propsA.length(2);
      CORBA::ULong i = 0;
      propsA[i].name  = "openbus.component.interface";
      propsA[i].value = "IDL:tecgraf/openbus/interop/simple/Hello:1.0";
      propsA[i+1].name  = "offer.domain";
      propsA[i+1].value = "Interoperability Tests";
      openbus::idl_or::ServiceOfferDescSeq_var offersA = connBusA->offers()->findServices(propsA);
      if (offersA->length()) {
        CORBA::Object_var o = offersA[1].service_ref->getFacetByName("hello");
        tecgraf::openbus::interop::simple::Hello *hello = 
          tecgraf::openbus::interop::simple::Hello::_narrow(o);
        hello->sayHello();
      } else std::cout << "nenhuma oferta encontrada." << std::endl;
    }
  } catch (const CORBA::Exception &e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
