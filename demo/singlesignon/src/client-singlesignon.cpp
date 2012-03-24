#include <openbus.h>
#include <iostream>
#include "stubs/hello.h"

int main(int argc, char** argv) {
  try {
    openbus::Connection* conn = openbus::connect("localhost", 2089);
    CORBA::Object_var o = conn->orb()->string_to_object(argv[1]);
    openbus::idl_accesscontrol::LoginProcess_var loginProcess = 
      openbus::idl_accesscontrol::LoginProcess::_narrow(o);
    unsigned char* secret = (unsigned char*) argv[2];
    conn->loginBySingleSignOn(loginProcess, secret);
    openbus::idl_offerregistry::ServicePropertySeq props;
    props.length(3);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "demo";
    props[1].name  = "openbus.component.facet";
    props[1].value = "hello";
    props[2].name  = "offer.domain";
    props[2].value = "OpenBus Demos";
    openbus::idl_offerregistry::ServiceOfferDescSeq_var offers =
      conn->offer_registry()->findServices(props);
    if (offers->length()) {
      CORBA::Object_var o = offers[0].service_ref->getFacetByName("hello");
      Hello* hello = Hello::_narrow(o);
      hello->sayHello();
    } else
      std::cout << "nenhuma oferta encontrada." << std::endl;
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