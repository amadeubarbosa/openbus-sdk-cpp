
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
    const char* caller = conn.getCallerChain()->callers[0].entity;
    std::cout << "Hello from '" << caller << "'." << std::endl;
    servant_called = true;
  }  

  bool& servant_called;
};

int main(int argc, char** argv)
{
  openbus::ConnectionManager* manager = openbus::getConnectionManager
    (openbus::initORB(argc, argv));
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

  props.length(3);
  props[0].name  = "openbus.offer.entity";
  props[0].value = "demo";
  props[1].name  = "openbus.component.facet";
  props[1].value = "hello";
  props[2].name  = "offer.domain";
  props[2].value = "OpenBus Demos";
  openbus::idl_or::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
  assert (offers->length() == 1);
  CORBA::Object_var o = offers[0].service_ref->getFacetByName("hello");
  Hello* hello = Hello::_narrow(o);

  assert(!servant_called);

  hello->sayHello();

  assert(servant_called);
}
