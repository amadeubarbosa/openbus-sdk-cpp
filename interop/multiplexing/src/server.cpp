#include <openbus/ORBInitializer.h>
#include <openbus/log.h>
#include <openbus/ConnectionManager.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>

#include "stubs/hello.h"
#include <CORBA.h>

const std::string entity("interop_multiplexing_cpp_server");

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello {
  HelloImpl(std::vector<openbus::Connection *> l) : _connVec(l) { }
  char * sayHello() {
    std::string msg;
    for (std::vector<openbus::Connection *>::iterator it = _connVec.begin(); it != _connVec.end(); 
      ++it) 
    {
      openbus::CallerChain *chain = (*it)->getCallerChain();
      if (chain) {
        std::cout << "Hello from " << chain->caller().entity.in() << "@" << chain->busid() 
          << std::endl;
        msg = "Hello " + std::string(chain->caller().entity.in()) + "@" + 
          std::string((*it)->busid()) + "!";
        break;
      }
    }
    CORBA::String_var r = CORBA::string_dup(msg.c_str());
    return r._retn();
  }
private:
  std::vector<openbus::Connection *> _connVec;
};

#ifdef OPENBUS_SDK_MULTITHREAD
class RunThread : public MICOMT::Thread {
public:
  RunThread(openbus::ConnectionManager *m) : _manager(m) {}
  void _run(void*) { _manager->orb()->run(); }
private:
  openbus::ConnectionManager *_manager;
};
#endif

class RegisterThread : public MICOMT::Thread {
public:
  RegisterThread(openbus::ConnectionManager *m, scs::core::ComponentContext &ctx, 
    openbus::Connection *c) : _manager(m), _conn(c), _ctx(ctx) {}
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
    } catch (const CORBA::Exception &e) {
      std::cout << "[thread: " << MICOMT::Thread::self() << "] error (CORBA::Exception): " << e 
      << std::endl;
    }
  }
private:
  openbus::ConnectionManager *_manager;
  openbus::Connection *_conn;
  scs::core::ComponentContext &_ctx;
};

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB *orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::ConnectionManager *manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    std::auto_ptr <openbus::Connection> conn1BusA (manager->createConnection("localhost", 2089));
    std::auto_ptr <openbus::Connection> conn2BusA (manager->createConnection("localhost", 2089));
    std::auto_ptr <openbus::Connection> conn3BusA (manager->createConnection("localhost", 2089));
    std::auto_ptr <openbus::Connection> connBusB (manager->createConnection("localhost", 3090));
    std::vector<openbus::Connection *> connVec;
    connVec.push_back(conn1BusA.get());
    connVec.push_back(conn2BusA.get());
    connVec.push_back(conn3BusA.get());
    connVec.push_back(connBusB.get());
    manager->setDefaultConnection(conn1BusA.get());

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
    scs::core::ComponentContext ctx(manager->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(connVec));
    ctx.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0", helloServant);
    
    conn1BusA->loginByPassword(entity.c_str(), entity.c_str());
    conn2BusA->loginByPassword(entity.c_str(), entity.c_str());
    conn3BusA->loginByPassword(entity.c_str(), entity.c_str());
    connBusB->loginByPassword(entity.c_str(), entity.c_str());

    manager->setDispatcher(*conn1BusA.get());
    manager->setDispatcher(*connBusB.get());

    RegisterThread *registerThread1 = new RegisterThread(manager, ctx, conn1BusA.get());
    RegisterThread *registerThread2 = new RegisterThread(manager, ctx, conn2BusA.get());
    RegisterThread *registerThread3 = new RegisterThread(manager, ctx, conn3BusA.get());
    RegisterThread *registerThread4 = new RegisterThread(manager, ctx, connBusB.get());
    registerThread1->start();
    registerThread2->start();
    registerThread3->start();
    registerThread4->start();
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
