// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "helloC.h"
#pragma clang diagnostic pop
#include <util.hpp>
#include <openbus.hpp>
#include <log/output/file_output.h>

#include <tao/PortableServer/PortableServer.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/program_options.hpp>
#pragma clang diagnostic pop
#include <iostream>
#include <map>

const std::string entity("interop_multiplexing_cpp_client");
std::string domain;
struct bus
{
  std::string host;
  unsigned short port;
};
std::map<std::size_t, bus> buses;
bool debug;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("debug", po::value<bool>()->default_value(true) , "yes|no")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus")
    ("bus2.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to second OpenBus")
    ("bus2.host.port", po::value<unsigned short>()->default_value(3089), 
     "Port to second OpenBus")
    ("user.password.domain", po::value<std::string>()->default_value("testing"),
     "Password domain");
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
    buses[0].host = vm["bus.host.name"].as<std::string>();
  }
  if (vm.count("bus.host.port"))
  {
    buses[0].port = vm["bus.host.port"].as<unsigned short>();
  }
  if (vm.count("bus2.host.name"))
  {
    buses[1].host = vm["bus2.host.name"].as<std::string>();
  }
  if (vm.count("bus2.host.port"))
  {
    buses[1].port = vm["bus2.host.port"].as<unsigned short>();
  }
  if (vm.count("user.password.domain"))
  {
    domain = vm["user.password.domain"].as<std::string>();
  }
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
    for (std::size_t busIdx = 0; busIdx != 2; ++busIdx)
    {
      boost::shared_ptr<openbus::Connection> 
        conn(bus_ctx->connectByAddress(buses[busIdx].host, buses[busIdx].port));
      bus_ctx->setDefaultConnection(conn);
      conn->loginByPassword(entity, entity, domain);

      openbus::idl::offers::ServicePropertySeq props;
      props.length(2);
      props[static_cast<CORBA::ULong>(0)].name  = "openbus.component.interface";
      props[static_cast<CORBA::ULong>(0)].value = 
        "IDL:tecgraf/openbus/interop/simple/Hello:1.0";
      props[static_cast<CORBA::ULong>(1)].name  = "offer.domain";
      props[static_cast<CORBA::ULong>(1)].value = "Interoperability Tests";

      openbus::idl::offers::ServiceOfferDescSeq_var offers = 
        find_offers(bus_ctx, props);
      for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
      {
        CORBA::Object_var o = offers[idx].service_ref->getFacetByName("Hello");
        tecgraf::openbus::interop::simple::Hello_var hello = 
          tecgraf::openbus::interop::simple::Hello::_narrow(o);
        CORBA::String_var ret(hello->sayHello());
        std::string msg("Hello " + entity + "@" + conn->busid() + "!");
        if (msg != std::string(ret.in()))
        {
          std::cerr << "sayHello() nao retornou a string '"
            + msg + "'." << std::endl;
          std::abort();
        }          
      }
    }
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
  return 0; //MSVC
}
