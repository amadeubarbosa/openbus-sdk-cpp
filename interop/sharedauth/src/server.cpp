// -*- coding: iso-8859-1-unix -*-

#include "helloS.h"
#include <util.hpp>
#include <openbus/ORBInitializer.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>
#include <openbus/log.hpp>
#include <scs/ComponentContext.h>

#include <iostream>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

const std::string entity("interop_sharedauth_cpp_server");
std::string private_key;
std::string bus_host;
unsigned short bus_port;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("private-key", po::value<std::string>()->default_value(entity + ".key"),
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
    openbus::CallerChain chain = ctx.getCallerChain();
    assert(chain != openbus::CallerChain());
    std::string msg = "Hello " + std::string(chain.caller().entity) + "!";
    std::cout << msg << std::endl;
    CORBA::String_var ret(msg.c_str());
    return ret._retn();
  }
private:
  openbus::OpenBusContext &ctx;
};

void login_register(
  const openbus::OpenBusContext &ctx, scs::core::ComponentContext &comp,
  const openbus::idl_or::ServicePropertySeq &props, openbus::Connection &conn)
{
  try 
  {
    conn.loginByCertificate(entity, openbus::PrivateKey(private_key));
  }
  catch(const openbus::InvalidPrivateKey &e)
  {
    std::cout << e.what() << std::endl;
  }
  ctx.getOfferRegistry()->registerService(comp.getIComponent(), props);
}

struct on_invalid_login
{
  typedef void result_type;
  on_invalid_login(
    const openbus::OpenBusContext &ctx, scs::core::ComponentContext &comp, 
    const openbus::idl_or::ServicePropertySeq &props, 
    openbus::Connection  &conn) 
    : ctx(ctx), comp(comp), props(props), conn(conn)
  {
  }

  result_type operator()(openbus::Connection &c, openbus::idl_ac::LoginInfo l) 
  {
    try 
    {
      std::cout << "invalid login: " << l.id.in() << std::endl; 
      login_register(ctx, comp, props, conn);
    } 
    catch (const CORBA::Exception &e) 
    {
      std::cout << "[error (CORBA::Exception)] " << e << std::endl;    
    }
  }
private:
  const openbus::OpenBusContext &ctx;
  scs::core::ComponentContext &comp;
  const openbus::idl_or::ServicePropertySeq &props;
  openbus::Connection &conn;
};

#ifdef OPENBUS_SDK_MULTITHREAD
void ORBRun(CORBA::ORB_ptr orb)
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
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    std::auto_ptr<openbus::Connection> conn = bus_ctx->connectByAddress(
      bus_host, bus_port);
    bus_ctx->setDefaultConnection(conn.get());
    
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread orb_run(boost::bind(ORBRun, bus_ctx->orb()));
#endif
    
    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "c++";
    scs::core::ComponentContext comp(bus_ctx->orb(), componentId);

    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    props[static_cast<CORBA::ULong>(0)].name = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";

    conn->onInvalidLogin(on_invalid_login(*bus_ctx, comp, props, *conn));

    HelloImpl srv(*bus_ctx);
    comp.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0",&srv);
    login_register(*bus_ctx, comp, props, *conn);

#ifdef OPENBUS_SDK_MULTITHREAD
    orb_run.join();
#else
    bus_ctx->orb()->run();
#endif
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    throw;
  }
  return 0; //MSVC
}
