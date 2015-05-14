// -*- coding: iso-8859-1-unix -*-

#include "helloC.h"
#include <util.hpp>
#include <openbus/ORBInitializer.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>
#include <openbus/log.hpp>

#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <boost/program_options.hpp>

const std::string entity("interop_rellogedjoin_cpp_client");
std::string bus_host;
unsigned short bus_port;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Opcoes permitidas");
  desc.add_options()
    ("help", "Help")
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
}

int main(int argc, char **argv) 
{
  try
  {
    load_options(argc, argv);
    openbus::log().set_level(openbus::debug_level);
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    std::auto_ptr<openbus::Connection> conn(bus_ctx->createConnection(bus_host, 
                                                                  bus_port));
    bus_ctx->setDefaultConnection(conn.get());
    conn->loginByPassword(entity, entity);

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[0u].name  = "offer.domain";
    props[0u].value = "Interoperability Tests";
    props[1u].name = "reloggedjoin.role";
    props[1u].value = "proxy";

    openbus::idl_or::ServiceOfferDescSeq_var offers(
      find_offers(bus_ctx, props));
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
      CORBA::String_var ret(hello->sayHello());
      std::string msg("Hello " + entity + "!");
      if (!(msg == std::string(ret.in())))
      {
        std::cerr << "sayHello() não retornou a string '"
          + msg + "'." << std::endl;
        std::abort();
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
