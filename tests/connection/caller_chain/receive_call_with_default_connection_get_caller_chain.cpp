// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include "stubs/hello.h"
#include <scs/ComponentContext.hpp>
#include <configuration.h>

struct hello_impl : public POA_Hello
{
  CORBA::ORB_var orb;

  hello_impl(CORBA::ORB_var orb)
    : orb(orb)
  {}

  void sayHello()
  {
    openbus::OpenBusContext& openbusContext = *dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    const char* caller = openbusContext.getCallerChain().caller().entity;
    std::cout << "Hello from '" << caller << "'." << std::endl;
  }  
};

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);

  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::auto_ptr <openbus::Connection> conn (openbusContext->createConnection(cfg.host(), cfg.port()));
  openbusContext->setDefaultConnection(conn.get());
  conn->loginByPassword(cfg.user().c_str(), cfg.password().c_str());

  scs::core::ComponentId componentId;
  componentId.name = "Hello";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "";    
  scs::core::ComponentContext ctx(openbusContext->orb(), componentId);

  hello_impl hello_servant (orb);

  ctx.addFacet("hello", "IDL:Hello:1.0", &hello_servant);

  openbus::idl_or::ServicePropertySeq props;
  props.length(1);
  openbus::idl_or::ServiceProperty property;
  property.name = "offer.domain";
  property.value = "OpenBus Demos";
  props[0] = property;
  openbusContext->getOfferRegistry()->registerService(ctx.getIComponent(), props);

  props.length(3);
  props[0].name  = "openbus.offer.entity";
  props[0].value = "test";
  props[1].name  = "openbus.component.facet";
  props[1].value = "hello";
  props[2].name  = "offer.domain";
  std::string key = cfg.key();
  props[2].value = key.c_str();
  openbus::idl_or::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
  assert (offers->length() == 1);
  CORBA::Object_var o = (*offers)[0u].service_ref->getFacetByName("hello");
  Hello* hello = Hello::_narrow(o);

  conn->logout();
  conn.reset();

  orb->run();
}
