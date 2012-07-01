#include <openbus/ORBInitializer.h>
#include <openbus/log.h>
#include <openbus/ConnectionManager.h>
#include <iostream>
#include "stubs/hello.h"

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB* orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    manager->setDefaultConnection(conn.get());
    conn->loginByPassword("demo", "demo");
    std::pair <openbus::idl_ac::LoginProcess*, const unsigned char*> credential = 
      conn->startSharedAuth();
    std::cout << "[LoginProcess]: " << manager->orb()->object_to_string(credential.first) 
      << std::endl;
    std::cout << "[secret]: " << credential.second << std::endl;
    openbus::idl_or::ServicePropertySeq props;
    props.length(3);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "demo";
    props[1].name  = "openbus.component.facet";
    props[1].value = "hello";
    props[2].name  = "offer.domain";
    props[2].value = "Interoperability Tests";
    openbus::idl_or::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
    if (offers->length()) {
      CORBA::Object_var o = offers[static_cast<CORBA::ULong> (0)].service_ref->getFacetByName(
        "hello");
      tecgraf::openbus::interop::simple::Hello* hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      hello->sayHello();
    } else std::cout << "nenhuma oferta encontrada." << std::endl;
    std::cout << "orb.run()" << std::endl;
    manager->orb()->run();
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;    
  }
  return 0;
}
