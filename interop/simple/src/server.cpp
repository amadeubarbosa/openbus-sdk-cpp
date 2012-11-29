#include <openbus/ORBInitializer.h>
#include <openbus/OpenBusContext.h>
#include <openbus/log.h>
#include <openbus/util/OpenSSL.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>

#include "stubs/hello.h"
#include <CORBA.h>

std::auto_ptr<openbus::Connection> conn;
scs::core::ComponentContext *ctx;
openbus::idl_or::ServicePropertySeq props;
const std::string entity("interop_hello_cpp_server");

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello {
  HelloImpl(openbus::OpenBusContext &c) : _ctx(c) { }
  char * sayHello() {
    openbus::CallerChain chain = _ctx.getCallerChain();
    assert(chain != openbus::CallerChain());
    std::string msg = "Hello " + std::string(chain.caller().entity) + "!";
    std::cout << msg << std::endl;
    CORBA::String_var r = CORBA::string_dup(msg.c_str());
    return r._retn();
  }
private:
  openbus::OpenBusContext &_ctx;
};

void loginAndRegister() {
  // FILE* privateKeyFile = fopen("DemoCppHello.key", "r");
  // if (!privateKeyFile) throw openbus::InvalidPrivateKey();
  // EVP_PKEY* privateKey = PEM_read_PrivateKey(privateKeyFile, 0, 0, 0);
  // fclose(privateKeyFile);
  // if (!privateKey) throw openbus::InvalidPrivateKey();
  // 
  // conn->loginByCertificate("interop_hello_cpp_server", openbus::openssl::PrvKey2byteSeq(privateKey));
  conn->loginByPassword(entity.c_str(), entity.c_str());
  openbusContext->getOfferRegistry()->registerService(ctx->getIComponent(), props);
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
  RunThread(openbus::OpenBusContext *m) : _openbusContext(m) {}
  void _run(void*) { _openbusContext->orb()->run(); }
private:
  openbus::OpenBusContext *_openbusContext;
};
#endif

int main(int argc, char** argv) {
  try {
    std::string host("localhost");
    short port = 2089;
    std::ifstream config("test.properties", std::ifstream::in);
    if (!config) assert(0);
    std::string s;
    while (config >> s) {
      if (s == std::string("bus.host.name")) {
        char c;
        config >> c;
        if (c == '=') {
          if (!(config >> host)) assert(0);
        } else assert(0);
      } else if (s == std::string("bus.host.port")) {
        char c;
        config >> c;
        if (c == '=') {
          if (!(config >> port)) assert(0);
        } else assert(0);        
      }
    }
    config.close();
    
    openbus::log.set_level(openbus::debug_level);
    CORBA::ORB *orb = openbus::ORBInitializer(argc, argv);
    
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    
    openbus::OpenBusContext *openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    conn = openbusContext->createConnection(host.c_str(), port);
    openbusContext->setDefaultConnection(conn.get());
    conn->onInvalidLogin(&onInvalidLogin);
    
    #ifdef OPENBUS_SDK_MULTITHREAD
    RunThread *runThread = new RunThread(openbusContext);
    runThread->start();
    #endif
    
    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "c++";
    ctx = new scs::core::ComponentContext(openbusContext->orb(), componentId);

    props.length(1);
    openbus::idl_or::ServiceProperty property;
    CORBA::ULong i = 0;
    property.name = "offer.domain";
    property.value = "Interoperability Tests";
    props[i] = property;

    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(*openbusContext));
    ctx->addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0", helloServant);
    
    loginAndRegister();
    
    #ifdef OPENBUS_SDK_MULTITHREAD
    runThread->wait();
    #else
    openbusContext->orb()->run();
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
