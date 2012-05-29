#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/hello.h"
#include <CORBA.h>

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::hello::IHello {
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

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    std::auto_ptr <openbus::Connection> connBusA (manager->createConnection("localhost", 2089));
    std::auto_ptr <openbus::Connection> connBusB (manager->createConnection("localhost", 3090));
    manager->setDefaultConnection(connBusA.get());

    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    scs::core::ComponentContext* ctx = new scs::core::ComponentContext(manager->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(connBusA.get()));
    ctx->addFacet("hello", "IDL:tecgraf/openbus/interop/hello/IHello:1.0", helloServant);
    
    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "Interoperability Tests";
    props[0] = property;

    connBusA->loginByPassword("demo", "demo");
    connBusB->loginByPassword("demo", "demo");

    manager->setDispatcher(connBusB.get());
    manager->setDispatcher(connBusA.get());

    connBusA->offers()->registerService(ctx->getIComponent(), props);
    manager->setRequester(connBusB.get());
    connBusB->offers()->registerService(ctx->getIComponent(), props);

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
