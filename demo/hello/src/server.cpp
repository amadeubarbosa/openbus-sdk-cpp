#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/hello.h"
#include <CORBA.h>

bool onInvalidLogin(const openbus::Connection* conn, const char* login) {
  std::cout << "login [" << login << "] terminated shutting the server down." << std::endl;
//  conn->close();
  return false;
}

struct HelloImpl : virtual public POA_Hello {
  HelloImpl(openbus::Connection* c) : _conn(c) { }
  void sayHello() throw (CORBA::SystemException) {
    const char* caller = _conn->getCallerChain()->callers[0].entity;
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
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    openbus::ConnectionManager* manager = openbus::getConnectionManager(orb);
    std::auto_ptr <openbus::Connection> conn = std::auto_ptr <openbus::Connection> 
      (manager->createConnection("localhost", 2089));
    manager->setDefaultConnection(conn.get());
    conn->onInvalidLoginCallback(&onInvalidLogin);
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
    scs::core::ComponentContext* ctx = new scs::core::ComponentContext(manager->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(conn.get()));
    ctx->addFacet("hello", "IDL:Hello:1.0", helloServant);
    
    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "OpenBus Demos";
    props[0] = property;

    conn->loginByPassword("demo", "demo");
    conn->offers()->registerService(ctx->getIComponent(), props);
    manager->orb()->run();
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
