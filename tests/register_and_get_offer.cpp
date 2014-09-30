// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include "helloS.h"
#include <scs/ComponentContext.h>
#include <configuration.h>
#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

struct hello_impl : public POA_Hello
{
  hello_impl(openbus::Connection &conn
             , bool &servant_called)
    : conn(conn), servant_called(servant_called)
  {}

  void sayHello()
  {
    std::cout << "Hello" << std::endl;
    servant_called = true;
  }  

  bool &servant_called;
  openbus::Connection &conn;
};

#ifdef OPENBUS_SDK_MULTITHREAD
void call_orb(CORBA::ORB_var orb)
{
  orb->run();
}
#endif

int main(int argc, char **argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&call_orb, orb));
#endif

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
  {
    CORBA::Object_var poa_obj(orb->resolve_initial_references("RootPOA"));
    PortableServer::POA_var poa(PortableServer::POA::_narrow(poa_obj));
    PortableServer::POAManager_var poa_manager(poa->the_POAManager());
    poa_manager->activate();

    scs::core::ComponentContext ctx(bus_ctx->orb(), componentId);

    bool servant_called(false);
    hello_impl hello_servant(*conn, servant_called);

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
    props[static_cast<CORBA::ULong>(2)].value = "OpenBus Demos";
    openbus::idl_or::ServiceOfferDescSeq_var offers(
      bus_ctx->getOfferRegistry()->findServices(props));
    std::cout << "Returned " << offers->length() << " offers" << std::endl;
    assert(offers->length() == 1);
    CORBA::Object_var o((*offers)[0u].service_ref->getFacetByName("hello"));
    Hello* hello(Hello::_narrow(o));

    assert(!servant_called);

    hello->sayHello();
    assert(servant_called);
  }

  conn->logout();
  conn.reset();

#ifdef OPENBUS_SDK_MULTITHREAD
  orb->shutdown(true);
  orb_thread.join();
#endif
}
