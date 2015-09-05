// -*- coding: iso-8859-1-unix -*-

#include "helloS.h"
#include <demo/openssl.hpp>
#include <scs/ComponentContext.h>
#include <openbus.hpp>

#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/regex.hpp>

const std::string entity("interop_chaining_cpp_server");
std::string priv_key_filename;
std::string bus_host;
unsigned short bus_port;
bool debug;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("debug", po::value<bool>()->default_value(true) , "yes|no")
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
  if (vm.count("debug"))
  {
    debug = vm["debug"].as<bool>();
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
    priv_key_filename = vm["private-key"].as<std::string>();
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
    std::string msg;
    if (boost::regex_match(chain.caller().entity.in(),
                           boost::regex("interop_chaining_.+_proxy")))
    {
       msg = "Hello " + std::string(chain.originators()[0].entity) + "!";
       std::cout << msg << std::endl;
       CORBA::String_var r = CORBA::string_dup(msg.c_str());
       return r._retn();
    }
    throw CORBA::NO_PERMISSION();
  }
private:
  openbus::OpenBusContext &ctx;
};

void login_register(
  const openbus::OpenBusContext &ctx, scs::core::ComponentContext &comp,
  const openbus::idl::offers::ServicePropertySeq &props, openbus::Connection &conn)
{
  EVP_PKEY *priv_key(
    openbus::demo::openssl::read_priv_key(priv_key_filename));
  if (!priv_key)
  {
    std::cout << "Chave privada inv�lida." << std::endl;
    return;
  }
  conn.loginByCertificate(entity, priv_key); 
  ctx.getOfferRegistry()->registerService(comp.getIComponent(), props);
}

struct on_invalid_login
{
  typedef void result_type;
  on_invalid_login(
    const openbus::OpenBusContext &ctx, scs::core::ComponentContext &comp, 
    const openbus::idl::offers::ServicePropertySeq &props, 
    openbus::Connection  &conn) 
    : ctx(ctx), comp(comp), props(props), conn(conn)
  {
  }

  result_type operator()(openbus::Connection &c, openbus::idl::access::LoginInfo l) 
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
  const openbus::idl::offers::ServicePropertySeq &props;
  openbus::Connection &conn;
};

void ORBRun(CORBA::ORB_ptr orb)
{
  orb->run();
}

int main(int argc, char **argv)
{
  try
  {
    load_options(argc, argv);
    if (debug)
    {
      openbus::log().set_level(openbus::debug_level);
    }
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    CORBA::Object_var o = orb_ctx->orb()->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    CORBA::Object_var
      obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
    openbus::OpenBusContext
      *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
    std::auto_ptr<openbus::Connection>
      conn(bus_ctx->connectByAddress(bus_host, bus_port));
    bus_ctx->setDefaultConnection(conn.get());

    boost::thread orb_run(boost::bind(ORBRun, bus_ctx->orb()));

    scs::core::ComponentId componentId;
    componentId.name = "RestrictedHello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "c++";
    scs::core::ComponentContext comp(bus_ctx->orb(), componentId);

    openbus::idl::offers::ServicePropertySeq props;
    props.length(1);
    props[0u].name = "offer.domain";
    props[0u].value = "Interoperability Tests";

    conn->onInvalidLogin(on_invalid_login(*bus_ctx, comp, props, *conn));

    HelloImpl srv(*bus_ctx);
    comp.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0",
                  &srv);
    login_register(*bus_ctx, comp, props, *conn);
    orb_run.join();
  }
  catch (const CORBA::Exception &e)
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
  return 0; //MSVC
}
