// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include "helloS.h"
#include <scs/ComponentContext.h>
#include <configuration.h>

struct hello_impl : public POA_Hello
{
  hello_impl(CORBA::ORB_var orb)
    : orb(orb)
  {}

  void sayHello()
  {
    openbus::OpenBusContext *bus_ctx = dynamic_cast<openbus::OpenBusContext *>
      (orb->resolve_initial_references("OpenBusContext"));
    const char *caller(bus_ctx->getCallerChain().caller().entity);
    std::cout << "Hello from '" << caller << "'." << std::endl;
  }  

  CORBA::ORB_var orb;
};

int main(int argc, char **argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));

  CORBA::Object_ptr o(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(o));
  std::auto_ptr <openbus::Connection> conn(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  bus_ctx->setDefaultConnection(conn.get());
  conn->loginByPassword(cfg.user(), cfg.password());

  scs::core::ComponentId componentId;
  componentId.name = "Hello";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "";    
  scs::core::ComponentContext ctx(bus_ctx->orb(), componentId);

  hello_impl hello_servant (orb);

  ctx.addFacet("hello", "IDL:Hello:1.0", &hello_servant);

  openbus::idl_or::ServicePropertySeq props;
  props.length(1);
  openbus::idl_or::ServiceProperty property;
  property.name = "offer.domain";
  property.value = "OpenBus Demos";
  props[static_cast<CORBA::ULong>(0)] = property;
  bus_ctx->getOfferRegistry()->registerService(ctx.getIComponent(), props);

  props.length(3);
  props[static_cast<CORBA::ULong>(0)].name  = "openbus.offer.entity";
  props[static_cast<CORBA::ULong>(0)].value = "test";
  props[static_cast<CORBA::ULong>(1)].name  = "openbus.component.facet";
  props[static_cast<CORBA::ULong>(1)].value = "hello";
  props[static_cast<CORBA::ULong>(2)].name  = "offer.domain";
  std::string key(cfg.key());
  props[static_cast<CORBA::ULong>(2)].value = key.c_str();
  openbus::idl_or::ServiceOfferDescSeq_var offers(
    bus_ctx->getOfferRegistry()->findServices(props));
  assert(offers->length() == 1);
  {
    CORBA::Object_var o((*offers)[0u].service_ref->getFacetByName("hello"));
    Hello_var hello(Hello::_narrow(o));
    hello->sayHello();
  }
  
  conn->logout();
  conn.reset();

  orb->run();
}
