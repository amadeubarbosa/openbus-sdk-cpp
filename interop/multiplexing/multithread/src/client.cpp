#include <openbus/ORBInitializer.h>
#include <openbus/log.h>
#include <openbus/ConnectionManager.h>
#include <iostream>
#include "stubs/hello.h"

struct Bus {
  std::string host;
  short port;
};

std::vector<Bus> busVec;
const std::string entity("interop_multiplexing_cpp_client");

int main(int argc, char** argv) {
  try {
    Bus busA, busB;
    busA.host = "localhost";
    busA.port = 2089;
    busVec.push_back(busA);
    busB.host = "localhost";
    busB.port = 3090;
    busVec.push_back(busB);
    
    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB *orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::ConnectionManager *manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    
    for (std::vector<Bus>::iterator it = busVec.begin(); it != busVec.end(); ++it) {
      std::auto_ptr <openbus::Connection> 
        conn (manager->createConnection((*it).host.c_str(), (*it).port));
      manager->setDefaultConnection(conn.get());
      conn->loginByPassword(entity.c_str(), entity.c_str());
      openbus::idl_or::ServicePropertySeq props;
      props.length(2);
      CORBA::ULong i = 0;
      props[i].name  = "openbus.component.interface";
      props[i].value = "IDL:tecgraf/openbus/interop/simple/Hello:1.0";
      props[i+1].name  = "offer.domain";
      props[i+1].value = "Interoperability Tests";
      openbus::idl_or::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
      for (CORBA::ULong idx = 0; idx < offers->length(); ++idx) {
        CORBA::Object_var o = offers[idx].service_ref->getFacetByName("hello");
        tecgraf::openbus::interop::simple::Hello *hello = 
          tecgraf::openbus::interop::simple::Hello::_narrow(o);
        char *msg = hello->sayHello();
        std::string s = "Hello " + entity + "!";
        assert(!strcmp(msg, s.c_str()));
      }
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
