
#include <openbus.h>
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

  hello_impl(openbus::Connection& conn
             , bool& servant_called)
    : conn(conn), servant_called(servant_called)
  {}

  void sayHello()
  {
    const char* caller = conn.getCallerChain()->caller().entity;
    std::cout << "Hello from '" << caller << "'." << std::endl;
    servant_called = true;
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
  CORBA::ORB_var orb = openbus::initORB(argc, argv);

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&call_orb, orb));
#endif

  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenbusConnectionManager");
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>(obj_connection_manager);
  std::auto_ptr <openbus::Connection> conn (manager->createConnection(cfg.host().c_str(), cfg.port()));
  conn->loginByPassword(cfg.user().c_str(), cfg.password().c_str());
  manager->setDefaultConnection(conn.get());
  
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

#ifdef OPENBUS_SDK_MULTITHREAD
  orb->shutdown(true);
  orb_thread.join();
#endif
}
