// -*- coding: iso-8859-1-unix -*-

#include "helloS.h"
#include <util.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>
#include <scs/ComponentContext.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <fstream>
#include <typeinfo>

const std::string entity("interop_reloggedjoin_cpp_proxy");
std::string priv_key_filename;
std::string bus_host;
unsigned short bus_port;
bool debug;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Opcoes permitidas");
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
  HelloImpl(openbus::OpenBusContext &c) : bus_ctx(c) 
  { 
  }

  char *sayHello() 
  {
    try
    {
      openbus::Connection *const conn = bus_ctx.getCurrentConnection();
      conn->logout();
      EVP_PKEY *priv_key(
        openbus::demo::openssl::read_priv_key(priv_key_filename));
      if (!priv_key)
      {
        std::cerr << "Chave privada inválida." << std::endl;
        std::abort();
      }
      conn->loginByCertificate(entity, priv_key);
      openbus::CallerChain chain = bus_ctx.getCallerChain();
      if (chain == openbus::CallerChain())
      {
        std::abort();
      }
      std::string entity(chain.caller().entity);
      std::string msg("Hello " + entity + "!");

      openbus::idl::offers::ServicePropertySeq props;
      props.length(2);
      props[0].name  = "offer.domain";
      props[0].value = "Interoperability Tests";
      props[1].name = "reloggedjoin.role";
      props[1].value = "server";

      bus_ctx.joinChain(chain);

      openbus::idl::offers::ServiceOfferDescSeq_var offers(
        find_offers(&bus_ctx, props));
      if (offers->length() < 1)
      {
        std::cerr << "offers->length() != 0" << std::endl;
        std::abort();
      }
      for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
      {
        CORBA::Object_var o = offers[idx].service_ref->getFacetByName("Hello");
        tecgraf::openbus::interop::simple::Hello_var hello = 
          tecgraf::openbus::interop::simple::Hello::_narrow(o);
        return hello->sayHello();
      }
    }
    catch (const CORBA::SystemException &e)
    {
      std::cerr << "repid: " << e._rep_id() << "minor code: " << e.minor()
                << std::endl;
      std::abort();
    }
    return "";
  }
private:
  openbus::OpenBusContext &bus_ctx;
};

void loginAndRegister(
  const openbus::OpenBusContext &bus_ctx, scs::core::ComponentContext &comp,
  const openbus::idl::offers::ServicePropertySeq &props, openbus::Connection &conn)
{
  EVP_PKEY *priv_key(
    openbus::demo::openssl::read_priv_key(priv_key_filename));
  if (!priv_key)
  {
    std::cerr << "Chave privada inválida." << std::endl;
    return;
  }
  conn.loginByCertificate(entity, priv_key);
  bus_ctx.getOfferRegistry()->registerService(comp.getIComponent(), props);
}

void ORBRun(CORBA::ORB_var orb)
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
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    std::auto_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(bus_host, bus_port));
    bus_ctx->setDefaultConnection(conn.get());    
    boost::thread orb_run(boost::bind(ORBRun, bus_ctx->orb()));

    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "c++";
    scs::core::ComponentContext comp(bus_ctx->orb(), componentId);

    openbus::idl::offers::ServicePropertySeq props;
    props.length(2);
    props[0].name = "offer.domain";
    props[0].value = "Interoperability Tests";
    props[1].name = "reloggedjoin.role";
    props[1].value = "proxy";

    HelloImpl srv(*bus_ctx);
    comp.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0",&srv);
    loginAndRegister(*bus_ctx, comp, props, *conn);

    orb_run.join();
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
  return 0; //MSVC
}
