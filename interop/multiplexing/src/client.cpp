// -*- coding: iso-8859-1-unix -*-

#include "stubs/hello.h"
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>

#include <iostream>
#include <map>
#include <log/output/file_output.h>
#include <boost/program_options.hpp>

const std::string entity("interop_multiplexing_cpp_client");
struct bus
{
  std::string host;
  unsigned short port;
};
std::map<std::size_t, bus> buses;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus")
    ("bus2.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to second OpenBus")
    ("bus2.host.port", po::value<unsigned short>()->default_value(3089), 
     "Port to second OpenBus");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::store(po::parse_config_file<char>("test.properties", desc), vm);
  po::notify(vm);
  if (vm.count("help")) 
  {
    std::cout << desc << std::endl;
    std::exit(1);
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
}

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

    openbus::OpenBusContext *const ctx = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    
    for (std::size_t busIdx = 0; busIdx != 2; ++busIdx)
    {
      std::auto_ptr<openbus::Connection> 
        conn(ctx->createConnection(buses[busIdx].host, buses[busIdx].port));
      ctx->setDefaultConnection(conn.get());
      conn->loginByPassword(entity, entity);

      openbus::idl_or::ServicePropertySeq props;
      props.length(2);
      props[static_cast<CORBA::ULong>(0)].name  = "openbus.component.interface";
      props[static_cast<CORBA::ULong>(0)].value = 
        "IDL:tecgraf/openbus/interop/simple/Hello:1.0";
      props[static_cast<CORBA::ULong>(1)].name  = "offer.domain";
      props[static_cast<CORBA::ULong>(1)].value = "Interoperability Tests";

      openbus::idl_or::ServiceOfferDescSeq_var offers = 
        ctx->getOfferRegistry()->findServices(props);
      for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
      {
        CORBA::Object_var o = offers[idx].service_ref->getFacetByName("Hello");
        tecgraf::openbus::interop::simple::Hello *hello = 
          tecgraf::openbus::interop::simple::Hello::_narrow(o);
        std::string msg(hello->sayHello());
        std::string s = "Hello " + entity + "@" + conn->busid() + "!";
        assert(s == msg);
      }
    }
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
}
