#include <openbus/ORBInitializer.h>
#include <openbus/ConnectionManager.h>
#include <openbus/log.h>
#include <openbus/util/OpenSSL.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <sstream>
#include <typeinfo>

#include "stubs/hello.h"
#include <CORBA.h>

openbus::ConnectionManager *manager;
std::auto_ptr<openbus::Connection> conn;
scs::core::ComponentContext *ctx;
openbus::idl_or::ServicePropertySeq props;
const std::string entity("interop_hello_cpp_server");

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello {
  HelloImpl(openbus::Connection *c) : _conn(c) { }
  char * sayHello() {
    const char *s = "";
    openbus::CallerChain *chain = _conn->getCallerChain();
    if (chain) s = chain->caller().entity;
    std::cout << "Hello from '" << s << "'." << std::endl;
    std::stringstream ss;
    ss << "Hello " << s << "!";
    CORBA::String_var r = CORBA::string_dup(ss.str().c_str());
    return r._retn();
  }
private:
  openbus::Connection *_conn;
};

void loginAndRegister() {
  // FILE* privateKeyFile = fopen("DemoCppHello.key", "r");
  // if (!privateKeyFile) throw openbus::CorruptedPrivateKey();
  // EVP_PKEY* privateKey = PEM_read_PrivateKey(privateKeyFile, 0, 0, 0);
  // fclose(privateKeyFile);
  // if (!privateKey) throw openbus::CorruptedPrivateKey();
  // 
  // conn->loginByCertificate("interop_hello_cpp_server", openbus::openssl::PrvKey2byteSeq(privateKey));
  conn->loginByPassword(entity.c_str(), entity.c_str());
  conn->offers()->registerService(ctx->getIComponent(), props);
}

void onInvalidLogin(openbus::Connection &c, openbus::idl_ac::LoginInfo l) {
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
    componentId.platform_spec = "c++";
    ctx = new scs::core::ComponentContext(manager->orb(), componentId);

    props.length(1);
    openbus::idl_or::ServiceProperty property;
    CORBA::ULong i = 0;
    property.name = "offer.domain";
    property.value = "Interoperability Tests";
    props[i] = property;

    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(conn.get()));
    ctx->addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0", helloServant);
    
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
