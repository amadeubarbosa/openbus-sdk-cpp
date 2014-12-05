// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include "stubs/hello.h"
#include <scs/ComponentContext.hpp>
#include <configuration.h>
#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

struct hello_impl : public POA_Hello
{
  CORBA::ORB_var orb;

  hello_impl(CORBA::ORB_var orb
             , bool& servant_called)
    : orb(orb), servant_called(servant_called)
  {}

  void sayHello()
  {
    openbus::OpenBusContext& openbusContext = *dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    const char* caller = openbusContext.getCallerChain().caller().entity;
    std::cout << "Hello from '" << caller << "'." << std::endl;
    servant_called = true;
    orb->shutdown(true);
  }  

  bool servant_called;
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
  conn->loginByPassword(cfg.user().c_str(), cfg.password().c_str());
  openbusContext->setDefaultConnection(conn.get());
  
  bool servant_called = false;
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

    hello_impl hello_servant (orb, servant_called);

    ctx.addFacet("hello", "IDL:Hello:1.0", &hello_servant);

    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    property.name = "offer.domain";
    std::string key = cfg.key();
    property.value = key.c_str();
    props[0] = property;
    openbusContext->getOfferRegistry()->registerService(ctx.getIComponent(), props);
  }

  conn->logout();
  conn.reset();

#ifdef OPENBUS_SDK_MULTITHREAD
  orb_thread.join();

  assert(servant_called == true);
#endif
}
