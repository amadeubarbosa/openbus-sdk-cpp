#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/hello.h"
#include <CORBA.h>

scs::core::ComponentContext* ctx;

struct HelloImpl : virtual public POA_Hello {
  HelloImpl(openbus::Connection* c) : _conn(c) { }
  void sayHello() throw (CORBA::SystemException) {
    openbus::CallerChain* chain = _conn->getCallerChain();
    if (chain) 
      std::cout << "Hello from " << chain->callers[0].entity << "@" << chain->busid << std::endl;
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
  RegisterThread(openbus::ConnectionManager* m, openbus::Connection* c) : _manager(m), _conn(c) {}
  void _run(void*) {
    try {
      openbus::idl_or::ServicePropertySeq props;
      props.length(1);
      openbus::idl_or::ServiceProperty property;
      property.name = "offer.domain";
      property.value = "OpenBus Demos";
      props[0] = property;
      _manager->setThreadRequester(_conn);
      _conn->offers()->registerService(ctx->getIComponent(), props);
    } catch (const CORBA::Exception& e) {
      std::cout << "[thread: " << MICOMT::Thread::self() << "] error (CORBA::Exception): " << e << std::endl;
    }
  }
private:
  openbus::ConnectionManager* _manager;
  openbus::Connection* _conn;
};

int main(int argc, char** argv) {
  try {
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    openbus::ConnectionManager* manager = openbus::getConnectionManager(orb);
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
    ctx = new scs::core::ComponentContext(manager->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(connBusA.get()));
    ctx->addFacet("hello", "IDL:Hello:1.0", helloServant);
    
    connBusA->loginByPassword("demo", "demo");
    connBusB->loginByPassword("demo", "demo");

    manager->setDispatcher(connBusB.get());
    manager->setDispatcher(connBusA.get());

    RegisterThread* registerThreadA = new RegisterThread(manager, connBusA.get());
    RegisterThread* registerThreadB = new RegisterThread(manager, connBusB.get());
    registerThreadA->start();
    registerThreadB->start();
    runThread->wait();
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (const openbus::Connection::Exception& e) {
    std::cout << "[error (Connection::Exception)] " << e.name() << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
