#include <openbus/ORBInitializer.h>
#include <openbus/log.h>
#include <openbus/OpenBusContext.h>
#include <openbus/Connection.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <vector>

#include "stubs/hello.h"
#include <CORBA.h>
#include "properties_reader.h"
#include <log/output/file_output.h>

const std::string entity("interop_multiplexing_cpp_server");

class CallDispatchCallback {
public:
  CallDispatchCallback(openbus::Connection *c1, openbus::Connection *c2) {
    _vconn.push_back(c1);
    _vconn.push_back(c2);
  }

  openbus::Connection * operator()(openbus::OpenBusContext &context, const std::string busId, 
                                   const std::string loginId, const std::string operation)
  {
    for (std::vector<openbus::Connection *>::const_iterator it = _vconn.begin(); 
         it != _vconn.end(); ++it) 
    {
      if (busId == (*it)->busid()) {
        return *it;
      }
    }
    std::cerr << "Conexao de despacho nao encontrada." << std::endl;
    return 0;
  }
private:
  std::vector<openbus::Connection *> _vconn;
};

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello {
  HelloImpl(openbus::OpenBusContext &c) : _ctx(c) { }
  char * sayHello() {
    openbus::CallerChain chain = _ctx.getCallerChain();
    assert(chain != openbus::CallerChain());
    std::string msg = "Hello " + std::string(chain.caller().entity) + "@"
      + chain.busid() + "!";
    std::cout << msg << std::endl;
    CORBA::String_var r = CORBA::string_dup(msg.c_str());
    return r._retn();
  }
private:
  openbus::OpenBusContext &_ctx;
};

#ifdef OPENBUS_SDK_MULTITHREAD
class RunThread : public MICOMT::Thread {
public:
  RunThread(openbus::OpenBusContext *m) : _openbusContext(m) {}
  void _run(void*) { _openbusContext->orb()->run(); }
private:
  openbus::OpenBusContext *_openbusContext;
};

class RegisterThread : public MICOMT::Thread {
public:
  RegisterThread(openbus::OpenBusContext *m, scs::core::ComponentContext &ctx, 
    openbus::Connection *c) : _openbusContext(m), _conn(c), _ctx(ctx) {}
  void _run(void*) {
#else
class Register {
public:
  Register(openbus::OpenBusContext *m, scs::core::ComponentContext &ctx,
           openbus::Connection *c) : _openbusContext(m), _conn(c), _ctx(ctx) 
  {
#endif
    try {
      openbus::idl_or::ServicePropertySeq props;
      props.length(1);
      openbus::idl_or::ServiceProperty property;
      property.name = "offer.domain";
      property.value = "Interoperability Tests";
      props[0] = property;
      _openbusContext->setCurrentConnection(_conn);
      _openbusContext->getOfferRegistry()->registerService(_ctx.getIComponent(), props);
    } catch (const CORBA::Exception &e) {
      #ifdef OPENBUS_SDK_MULTITHREAD
      std::cout << "[thread: " << MICOMT::Thread::self() << "] error (CORBA::Exception): " 
                << e << std::endl;
      #else
      std::cout << "error (CORBA::Exception): " << e << std::endl;
      #endif
    }
  }
private:
  openbus::OpenBusContext *_openbusContext;
  openbus::Connection *_conn;
  scs::core::ComponentContext &_ctx;
};

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::debug_level);

    ::properties properties_file;
    if(!properties_file.openbus_log_file.empty())
    {
      std::auto_ptr<logger::output_base> output
        (new logger::output::file_output(properties_file.openbus_log_file.c_str()
                                         , std::ios::out));
      openbus::log.add_output(output);
    }
    
    if(properties_file.buses.size() < 2)
      throw std::runtime_error("There should be 2 buses configured in properties file");

    std::vector< ::properties::bus> buses = properties_file.buses;

    CORBA::ORB *orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::OpenBusContext *openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr <openbus::Connection> connBusB
      (openbusContext->createConnection(buses[0].host, buses[0].port));
    std::auto_ptr <openbus::Connection> conn1BusA
      (openbusContext->createConnection(buses[1].host, buses[1].port));
    std::auto_ptr <openbus::Connection> conn2BusA
      (openbusContext->createConnection(buses[1].host, buses[1].port));
    std::auto_ptr <openbus::Connection> conn3BusA
      (openbusContext->createConnection(buses[1].host, buses[1].port));
    std::vector<openbus::Connection *> connVec;
    connVec.push_back(conn1BusA.get());
    connVec.push_back(conn2BusA.get());
    connVec.push_back(conn3BusA.get());
    connVec.push_back(connBusB.get());
    
    #ifdef OPENBUS_SDK_MULTITHREAD
    RunThread *runThread = new RunThread(openbusContext);
    runThread->start();
    #endif
    
    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    scs::core::ComponentContext ctx(openbusContext->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> helloServant(new HelloImpl(*openbusContext));
    ctx.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0", helloServant);
    
    const openbus::PrivateKey pKey(entity + ".key");

    conn1BusA->loginByCertificate(entity, pKey);
    conn2BusA->loginByCertificate(entity, pKey);
    conn3BusA->loginByCertificate(entity, pKey);
    connBusB->loginByCertificate(entity, pKey);
    
    openbusContext->onCallDispatch(CallDispatchCallback(conn1BusA.get(), connBusB.get()));

    #ifdef OPENBUS_SDK_MULTITHREAD
    RegisterThread *registerThread1 = new RegisterThread(openbusContext, ctx, conn1BusA.get());
    RegisterThread *registerThread2 = new RegisterThread(openbusContext, ctx, conn2BusA.get());
    RegisterThread *registerThread3 = new RegisterThread(openbusContext, ctx, conn3BusA.get());
    RegisterThread *registerThread4 = new RegisterThread(openbusContext, ctx, connBusB.get());
    registerThread1->start();
    registerThread2->start();
    registerThread3->start();
    registerThread4->start();
    runThread->wait();
    #else
    Register(openbusContext, ctx, conn1BusA.get());
    Register(openbusContext, ctx, conn2BusA.get());
    Register(openbusContext, ctx, conn3BusA.get());
    Register(openbusContext, ctx, connBusB.get());
    openbusContext->orb()->run();
    #endif
  } catch(std::exception const& e) {
    std::cout << "[error (std::exception)] " << e.what() << std::endl;
  } catch (const CORBA::Exception &e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
