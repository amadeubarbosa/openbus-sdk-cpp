// -*- coding: iso-8859-1-unix -*-

#include "stubs/hello.h"
#include <openbus/ORBInitializer.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>
#include <openbus/log.hpp>
#include <scs/ComponentContext.hpp>

#include <CORBA.h>
#include <iostream>
#include <fstream>
#include <typeinfo>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

const std::string entity("interop_reloggedjoin_cpp_proxy");
std::string private_key;
std::string bus_host;
unsigned short bus_port;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Opcoes permitidas");
  desc.add_options()
    ("help", "Help")
    ("private-key", po::value<std::string>()->default_value("admin/" + entity
                                                            + ".key"),
     "Path to private key")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) 
  {
    std::cout << desc << std::endl;
    std::exit(1);
  }
  if (vm.count("bus.host.name"))
  {
    bus_host = vm["bus.host.name"].as<std::string>();
  }
  if (vm.count("bus.host.port"))
  {
    bus_port = vm["bus.host.port"].as<unsigned short>();
  }
  if (vm.count("private-key"))
  {
    private_key = vm["private-key"].as<std::string>();
  }
}

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello 
{
  HelloImpl(openbus::OpenBusContext &c) : ctx(c) 
  { 
  }

  char *sayHello() 
  {
    openbus::Connection *const conn = ctx.getCurrentConnection();
    conn->logout();
    try 
    {
      conn->loginByCertificate(entity, openbus::PrivateKey(private_key));
    }
    catch(const openbus::InvalidPrivateKey &e)
    {
      std::cout << e.what() << std::endl;
    }
    openbus::CallerChain chain = ctx.getCallerChain();
    assert(chain != openbus::CallerChain());
    std::string entity(chain.caller().entity);
    std::string msg = "Hello " + entity + "!";

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name = "reloggedjoin.role";
    props[static_cast<CORBA::ULong>(1)].value = "server";

    openbus::idl_or::ServiceOfferDescSeq_var offers = 
      ctx.getOfferRegistry()->findServices(props);
    for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
    {
      CORBA::Object_var o = offers[idx].service_ref->getFacetByName("Hello");
      tecgraf::openbus::interop::simple::Hello_var hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      ctx.joinChain(chain);
      return hello->sayHello();
    }
  }
private:
  openbus::OpenBusContext &ctx;
};

void loginAndRegister(
  const openbus::OpenBusContext &ctx, scs::core::ComponentContext &comp,
  const openbus::idl_or::ServicePropertySeq &props, openbus::Connection &conn)
{
  conn.loginByCertificate(entity, openbus::PrivateKey(private_key));
  ctx.getOfferRegistry()->registerService(comp.getIComponent(), props);
}

#ifdef OPENBUS_SDK_MULTITHREAD
void ORBRun(CORBA::ORB_var orb)
{
  orb->run();
}
#endif

int main(int argc, char **argv) 
{
  try 
  {
    load_options(argc, argv);
    openbus::log().set_level(openbus::debug_level);

    CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    
    openbus::OpenBusContext *const ctx = dynamic_cast<openbus::OpenBusContext *>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr<openbus::Connection> conn = ctx->createConnection(bus_host, 
                                                                    bus_port);
    ctx->setDefaultConnection(conn.get());    
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread orb_run(boost::bind(ORBRun, ctx->orb()));
#endif

    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "c++";
    scs::core::ComponentContext comp(ctx->orb(), componentId);

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name = "reloggedjoin.role";
    props[static_cast<CORBA::ULong>(1)].value = "proxy";

    HelloImpl srv(*ctx);
    comp.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0",&srv);
    loginAndRegister(*ctx, comp, props, *conn);
#ifdef OPENBUS_SDK_MULTITHREAD
    orb_run.join();
#else
    ctx->orb()->run();
#endif
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } 
}
