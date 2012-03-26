#include <openbus.h>
#include <iostream>
#include "stubs/hello.h"

int main(int argc, char** argv) {
  try {
    std::auto_ptr <openbus::Connection> conn = std::auto_ptr <openbus::Connection>
      (openbus::connect("localhost", 2089));
    conn->loginByPassword("demo", "demo");
    std::pair <openbus::idl_ac::LoginProcess*, unsigned char*> credential = 
      conn->startSingleSignOn();
    std::cout << "[LoginProcess]: " << conn->orb()->object_to_string(credential.first) 
      << std::endl;
    std::cout << "[secret]: " << credential.second << std::endl;
    openbus::idl_or::ServicePropertySeq props;
    props.length(3);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "demo";
    props[1].name  = "openbus.component.facet";
    props[1].value = "hello";
    props[2].name  = "offer.domain";
    props[2].value = "OpenBus Demos";
    openbus::idl_or::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
    if (offers->length()) {
      CORBA::Object_var o = offers[0].service_ref->getFacetByName("hello");
      Hello* hello = Hello::_narrow(o);
      hello->sayHello();
    } else
      std::cout << "nenhuma oferta encontrada." << std::endl;
    std::cout << "orb.run()" << std::endl;
    conn->orb()->run();
    conn->close();
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (const openbus::Connection::Exception& e) {
    std::cout << "[error (Connection::Exception)] " << e.name() << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;    
  }
  return 0;
}
