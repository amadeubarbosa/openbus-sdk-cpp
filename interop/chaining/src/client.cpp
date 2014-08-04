// -*- coding: iso-8859-1-unix -*-

#include "stubs/proxy.h"
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

const std::string entity("interop_chaining_cpp_client");
std::string bus_host;
unsigned short bus_port;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus");
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

    CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    openbus::OpenBusContext *const ctx = dynamic_cast<openbus::OpenBusContext *>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr<openbus::Connection> conn(ctx->createConnection(bus_host, 
                                                                  bus_port));
    ctx->setDefaultConnection(conn.get());
    conn->loginByPassword(entity, entity);

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name  = "openbus.component.interface";
    props[static_cast<CORBA::ULong>(1)].value = 
      "IDL:tecgraf/openbus/interop/simple/HelloProxy:1.0";

    openbus::idl_or::ServiceOfferDescSeq_var offers = 
      ctx->getOfferRegistry()->findServices(props);
    for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
    {
      if (offers[idx].service_ref->_non_existent())
      {
        continue;
      }
      CORBA::Object_var o = offers[idx].service_ref->getFacetByName("HelloProxy");
      tecgraf::openbus::interop::simple::HelloProxy *helloProxy = 
        tecgraf::openbus::interop::simple::HelloProxy::_narrow(o);
      openbus::idl_or::ServicePropertySeq properties = offers[idx].properties;
      char *loginId = 0;
      for (CORBA::ULong idx = 0; idx != properties.length(); ++idx)
      {
        if (std::string(properties[idx].name) == "openbus.offer.login")
        {
          loginId = properties[idx].value;
          break;
        }
      }
      assert(loginId != 0);
      openbus::CallerChain chain = ctx->makeChainFor(loginId);
      CORBA::OctetSeq encodedChain = ctx->encodeChain(chain);
      const char *msg = helloProxy->fetchHello(encodedChain);
      std::string s = "Hello " + entity + "!";
      if (!(msg == s))
      {
        throw std::runtime_error("msg != s");
      }
    }
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
}