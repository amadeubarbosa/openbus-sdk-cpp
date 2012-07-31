#include <openbus/ORBInitializer.h>
#include <openbus/ConnectionManager.h>
#include <openbus/log.h>
#include <iostream>
#include <string>
#include "stubs/hello.h"

const std::string entity("interop_hello_cpp_client");

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
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    manager->setDefaultConnection(conn.get());
    conn->loginByPassword(entity.c_str(), entity.c_str());
    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    CORBA::ULong i = 0;
    props[i].name  = "offer.domain";
    props[i].value = "Interoperability Tests";
    props[i+1].name  = "openbus.component.interface";
    props[i+1].value = "IDL:tecgraf/openbus/interop/simple/Hello:1.0";
    openbus::idl_or::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
    for (CORBA::ULong idx = 0; idx < offers->length(); ++idx) {
      CORBA::Object_var o = offers[idx].service_ref->getFacetByName("Hello");
      tecgraf::openbus::interop::simple::Hello *hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      char *msg = hello->sayHello();
      std::string s = "Hello " + entity + "!";
      assert(!strcmp(msg, s.c_str()));
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
