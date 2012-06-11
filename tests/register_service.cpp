
#include <openbus.h>
#include "stubs/hello.h"
#include <scs/ComponentContext.h>

struct hello_impl : public POA_Hello
{
  openbus::Connection& conn;

  hello_impl(openbus::Connection& conn
             , bool& servant_called)
    : conn(conn), servant_called(servant_called)
  {}

  void sayHello()
  {
    const char* caller = conn.getCallerChain()->callers()[0].entity;
    std::cout << "Hello from '" << caller << "'." << std::endl;
    servant_called = true;
  }  

  bool servant_called;
};

int main(int argc, char** argv)
{
  CORBA::ORB_var orb = openbus::initORB(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenbusConnectionManager");
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>(obj_connection_manager);
  std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
  conn->loginByPassword("demo", "demo");
  
  scs::core::ComponentId componentId;
  componentId.name = "Hello";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "";    
  scs::core::ComponentContext ctx(manager->orb(), componentId);

  bool servant_called = false;
  hello_impl hello_servant (*conn, servant_called);

  ctx.addFacet("hello", "IDL:Hello:1.0", &hello_servant);

  openbus::idl_or::ServicePropertySeq props;
  props.length(1);
  openbus::idl_or::ServiceProperty property;
  property.name = "offer.domain";
  property.value = "OpenBus Demos";
  props[0] = property;
  conn->offers()->registerService(ctx.getIComponent(), props);
}
