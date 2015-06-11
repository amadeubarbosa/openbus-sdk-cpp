// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include "helloS.h"
#include <scs/ComponentContext.h>
#include <configuration.h>
#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#endif

struct hello_impl : public POA_Hello
{
  openbus::Connection& conn;

  hello_impl(openbus::Connection& conn)
    : conn(conn)
  {}

  void sayHello()
  {
    std::cout << "Hello" << std::endl;
  }  
};

#ifdef OPENBUS_SDK_MULTITHREAD
void call_orb(CORBA::ORB_var orb)
{
  orb->run();
}
#endif

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&call_orb, orb_ctx->orb()));
#endif

  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

  std::auto_ptr <openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  bus_ctx->setDefaultConnection(conn.get());
  conn->loginByPassword(cfg.user().c_str(), cfg.password().c_str());

  scs::core::ComponentId componentId;
  componentId.name = "Hello";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "";    
  CORBA::Object_var
    poa_obj(orb_ctx->orb()->resolve_initial_references("RootPOA"));
  PortableServer::POA_var poa = PortableServer::POA::_narrow(poa_obj);
  PortableServer::POAManager_var poa_manager = poa->the_POAManager();
  poa_manager->activate();

  scs::core::ComponentContext ctx(bus_ctx->orb(), componentId);
  
  hello_impl hello_servant (*conn);

  ctx.addFacet("hello", "IDL:Hello:1.0", &hello_servant);

  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  openbus::idl_or::ServicePropertySeq props;
  props.length(2);
  openbus::idl_or::ServiceProperty property;
  property.name = "offer.domain";
  property.value = "OpenBus Demos";
  props[0] = property;
  props[1].name = "uuid";
  props[1].value = boost::uuids::to_string(uuid).c_str();
  bus_ctx->getOfferRegistry()->registerService(ctx.getIComponent(), props);

  props.length(4);
  props[0].name  = "openbus.offer.entity";
  props[0].value = "test";
  props[1].name  = "openbus.component.facet";
  props[1].value = "hello";
  props[2].name  = "offer.domain";
  props[2].value = "OpenBus Demos";
  props[3].name  = "uuid";
  props[3].value = boost::uuids::to_string(uuid).c_str();
  
  openbus::idl_or::ServiceOfferDescSeq_var offers(
    bus_ctx->getOfferRegistry()->findServices(props));
  std::cout << offers->length() << std::endl;
  if (offers->length() != 1)
  {
    std::cerr << "offers->length() != 1" << std::endl;
    std::abort();
  }
  CORBA::Object_var o((*offers)[0u].service_ref->getFacetByName("hello"));
  Hello *hello(Hello::_narrow(o));
  hello->sayHello();

  conn->logout();
  conn.reset();

#ifdef OPENBUS_SDK_MULTITHREAD
  orb_ctx->orb()->shutdown(true);
  orb_thread.join();
#endif
  return 0; //MSVC
}
