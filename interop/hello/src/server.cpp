#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/hello.h"
#include <CORBA.h>

bool onInvalidLogin(const openbus::Connection* conn, const openbus::idl_ac::LoginInfo* login) {
  std::cout << "login [" << login->id << "," << login->entity << 
    "] terminated shutting the server down." << std::endl;
  return false;
}

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello {
  HelloImpl(openbus::Connection* c) : _conn(c) { }
  void sayHello() throw (CORBA::SystemException) {
    const char* caller = _conn->getCallerChain()->callers()[0].entity;
    std::cout << "Hello from '" << caller << "'." << std::endl;
  }
private:
  openbus::Connection* _conn;
};

#ifdef OPENBUS_SDK_MULTITHREAD
class RunThread : public MICOMT::Thread {
public:
  RunThread(openbus::ConnectionManager* m) : _manager(m) {}
  void _run(void*) { _manager->orb()->run(); }
private:
  openbus::ConnectionManager* _manager;
};
#endif

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    assert(!CORBA::is_nil(o));
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    manager->setDefaultConnection(conn.get());
    conn->onInvalidLogin(&onInvalidLogin);
    #ifdef OPENBUS_SDK_MULTITHREAD
    RunThread* runThread = new RunThread(manager);
    runThread->start();
    #endif
    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    scs::core::ComponentContext ctx(manager->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(conn.get()));
    ctx.addFacet("hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0", helloServant);
    
    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "Interoperability Tests";
    props[0] = property;

    conn->loginByPassword("demo", "demo");
    conn->offers()->registerService(ctx.getIComponent(), props);
    #ifdef OPENBUS_SDK_MULTITHREAD
    runThread->wait();
    #else
    manager->orb()->run();
    #endif
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
