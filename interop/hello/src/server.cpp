#include <openbus/ORBInitializer.h>
#include <openbus/ConnectionManager.h>
#include <openbus/log.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/hello.h"
#include <CORBA.h>

openbus::ConnectionManager *manager;
std::auto_ptr<openbus::Connection> conn;
scs::core::ComponentContext *ctx;
openbus::idl_or::ServicePropertySeq props;

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello {
  HelloImpl(openbus::Connection *c) : _conn(c) { }
  void sayHello() throw (CORBA::SystemException) {
    std::cout << "Hello from '" <<  _conn->getCallerChain()->caller().entity << "'." << std::endl;
  }
private:
  openbus::Connection *_conn;
};

void loginAndRegister() {
  conn->loginByPassword("demo", "demo");
  conn->offers()->registerService(ctx->getIComponent(), props);
}

void onInvalidLogin(openbus::Connection &c, openbus::idl_ac::LoginInfo l, const char *busid) {
  try {
    std::cout << "invalid login: " << l.id.in() << std::endl; 
    loginAndRegister();
  } catch(CORBA::Exception &e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;    
  }
}

#ifdef OPENBUS_SDK_MULTITHREAD
class RunThread : public MICOMT::Thread {
public:
  RunThread(openbus::ConnectionManager *m) : _manager(m) {}
  void _run(void*) { _manager->orb()->run(); }
private:
  openbus::ConnectionManager *_manager;
};
#endif

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB *orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    conn = manager->createConnection("localhost", 2089);
    manager->setDefaultConnection(conn.get());
    conn->onInvalidLogin(&onInvalidLogin);
    #ifdef OPENBUS_SDK_MULTITHREAD
    RunThread *runThread = new RunThread(manager);
    runThread->start();
    #endif
    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    ctx = new scs::core::ComponentContext(manager->orb(), componentId);

    props.length(1);
    openbus::idl_or::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "Interoperability Tests";
    props[0] = property;

    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(conn.get()));
    ctx->addFacet("hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0", helloServant);
    
    loginAndRegister();
    #ifdef OPENBUS_SDK_MULTITHREAD
    runThread->wait();
    #else
    manager->orb()->run();
    #endif
  } catch (const CORBA::Exception &e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
