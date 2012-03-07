#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/hello.h"
#include <CORBA.h>

struct HelloImpl : virtual public POA_Hello {
  void sayHello() throw (CORBA::SystemException) {
    std::cout << "Hello" << std::endl;
  };
};

int main(int argc, char** argv) {
  try {
    std::auto_ptr <openbus::Connection> conn = std::auto_ptr <openbus::Connection> 
      (openbus::connect("localhost", 2089));

    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    
    scs::core::ComponentContext* ctx = new scs::core::ComponentContext(conn->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl);
    ctx->addFacet("hello", "IDL:Hello:1.0", helloServant);
    
    openbus::openbusidl_offer_registry::ServicePropertySeq_var props =
      new openbus::openbusidl_offer_registry::ServicePropertySeq(1);
    props->length(1);
    openbus::openbusidl_offer_registry::ServiceProperty_var property = 
      new openbus::openbusidl_offer_registry::ServiceProperty();
    property->name = CORBA::String_var("offer.domain");
    property->value = "OpenBus Demos";
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
