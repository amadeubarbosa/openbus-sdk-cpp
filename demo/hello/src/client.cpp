#include <openbus.h>
#include <iostream>
#include "stubs/hello.h"

int main(int argc, char** argv) {
  try {
    std::auto_ptr <openbus::Connection> conn = std::auto_ptr <openbus::Connection>
      (openbus::connect("localhost", 2089));
    conn->loginByPassword("demo", "demo");
    openbus::openbusidl_offer_registry::ServiceOfferDescSeq_var offers =
      conn->offer_registry()->getServices();
    CORBA::Object_var o = offers[0].service_ref->getFacetByName("hello");
    Hello* hello = Hello::_narrow(o);
    hello->sayHello();
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
