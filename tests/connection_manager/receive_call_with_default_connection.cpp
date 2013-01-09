
#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include "stubs/hello.h"
#include <scs/ComponentContext.h>
#include <configuration.h>
#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
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
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&call_orb, orb));
#endif

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
  {
    CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(poa_obj);
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    scs::core::ComponentContext ctx(openbusContext->orb(), componentId);
  
    hello_impl hello_servant (*conn);

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
    props[2].value = "OpenBus Demos";
    openbus::idl_or::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
    assert (offers->length() == 1);
    CORBA::Object_var o = offers[0u].service_ref->getFacetByName("hello");
    Hello* hello = Hello::_narrow(o);
    hello->sayHello();
  }

  conn->logout();
  conn.reset();

#ifdef OPENBUS_SDK_MULTITHREAD
  orb->shutdown(true);
  orb_thread.join();
#endif
}
