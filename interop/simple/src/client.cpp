#include <openbus/ORBInitializer.h>
#include <openbus/OpenBusContext.h>
#include <openbus/log.h>
#include <iostream>
#include <fstream>
#include <string>
#include "stubs/hello.h"

const std::string entity("interop_hello_cpp_client");

int main(int argc, char** argv) {
  try {
    std::string host("localhost");
    short port = 2089;
    std::ifstream config("test.properties", std::ifstream::in);
    if (!config) assert(0);
    std::string s;
    while (config >> s) {
      if (s == std::string("bus.host.name")) {
        char c;
        config >> c;
        if (c == '=') {
          if (!(config >> host)) assert(0);
        } else assert(0);
      } else if (s == std::string("bus.host.port")) {
        char c;
        config >> c;
        if (c == '=') {
          if (!(config >> port)) assert(0);
        } else assert(0);        
      }
    }
    config.close();

    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB *orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::OpenBusContext *openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references(OPENBUS_CONTEXT_ID));
    std::auto_ptr <openbus::Connection> conn (openbusContext->createConnection(host.c_str(), port));
    openbusContext->setDefaultConnection(conn.get());
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
