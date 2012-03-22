#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/hello.h"
#include <CORBA.h>

struct HelloImpl : virtual public POA_Hello {
  openbus::Connection* _conn;
  HelloImpl(openbus::Connection* c) : _conn(c) { }
  void sayHello() throw (CORBA::SystemException) {
    const char* caller = _conn->getCallerChain()->callers[0].entity;
    std::cout << "Hello from '" << caller << "'." << std::endl;
  };
};

int main(int argc, char** argv) {
  try {
    std::auto_ptr <openbus::Connection> conn (openbus::connect("localhost", 2089));
    
    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";    
    scs::core::ComponentContext* ctx = new scs::core::ComponentContext(conn->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(conn.get()));
    ctx->addFacet("hello", "IDL:Hello:1.0", helloServant);
    
    openbus::openbusidl_offer_registry::ServicePropertySeq props;
    props.length(1);
    openbus::openbusidl_offer_registry::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "OpenBus Demos";
    props[0] = property;

    conn->loginByPassword("demo", "demo");
    conn->offer_registry()->registerService(ctx->getIComponent(), props);
    conn->orb()->run();
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
