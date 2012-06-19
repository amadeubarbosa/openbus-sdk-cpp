#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/hello.h"
#include <CORBA.h>

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello {
  HelloImpl(openbus::Connection* c) : _conn(c) { }
  void sayHello() throw (CORBA::SystemException) {
    openbus::CallerChain* chain = _conn->getCallerChain();
    if (chain)
      std::cout << "Hello from " 
      << (chain->callers())[0].entity << "@" << chain->busid() << std::endl;
    else std::cout << "Nao foi possivel obter uma CallerChain." << std::endl;
  }
private:
  openbus::Connection* _conn;
};

class RunThread : public MICOMT::Thread {
public:
  RunThread(openbus::ConnectionManager* m) : _manager(m) {}
  void _run(void*) { _manager->orb()->run(); }
private:
  openbus::ConnectionManager* _manager;
};

class RegisterThread : public MICOMT::Thread {
public:
  RegisterThread(openbus::ConnectionManager* m, scs::core::ComponentContext& ctx, 
    openbus::Connection* c) : _manager(m), _conn(c), _ctx(ctx) {}
  void _run(void*) {
    try {
      openbus::idl_or::ServicePropertySeq props;
      props.length(1);
      openbus::idl_or::ServiceProperty property;
      property.name = "offer.domain";
      property.value = "Interoperability Tests";
      props[0] = property;
      _manager->setRequester(_conn);
      _conn->offers()->registerService(_ctx.getIComponent(), props);
    } catch (const CORBA::Exception& e) {
      std::cout << "[thread: " << MICOMT::Thread::self() << "] error (CORBA::Exception): " << e 
      << std::endl;
    }
  }
private:
  openbus::ConnectionManager* _manager;
  openbus::Connection* _conn;
  scs::core::ComponentContext& _ctx;
};

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    std::auto_ptr <openbus::Connection> connBusA (manager->createConnection("localhost", 2089));
    std::auto_ptr <openbus::Connection> connBusB (manager->createConnection("localhost", 3090));
    manager->setDefaultConnection(connBusA.get());

    RunThread* runThread = new RunThread(manager);
    runThread->start();

    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    scs::core::ComponentContext ctx(manager->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(connBusA.get()));
    ctx.addFacet("hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0", helloServant);
    
    connBusA->loginByPassword("demo", "demo");
    connBusB->loginByPassword("demo", "demo");

    manager->setDispatcher(connBusB.get());
    manager->setDispatcher(connBusA.get());

    RegisterThread* registerThreadA = new RegisterThread(manager, ctx, connBusA.get());
    RegisterThread* registerThreadB = new RegisterThread(manager, ctx, connBusB.get());
    registerThreadA->start();
    registerThreadB->start();
    runThread->wait();
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
