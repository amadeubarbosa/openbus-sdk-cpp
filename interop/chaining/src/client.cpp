// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "proxyS.h"
#include "helloC.h"
#pragma clang diagnostic pop
#include <openbus.hpp>
#include <util.hpp>

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <cstdlib>

const std::string entity("interop_chaining_cpp_client");
std::string bus_host, domain;
unsigned short bus_port;
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
    bus_host = vm["bus.host.name"].as<std::string>();
  }
  if (vm.count("bus.host.port"))
  {
    bus_port = vm["bus.host.port"].as<unsigned short>();
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
    CORBA::Object_var o = orb_ctx->orb()->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    CORBA::Object_var
      obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
    openbus::OpenBusContext
      *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

    boost::shared_ptr<openbus::Connection> conn(bus_ctx->connectByAddress(bus_host, 
                                                                  bus_port));
    bus_ctx->setDefaultConnection(conn);
    conn->loginByPassword(entity, entity, domain);

    openbus::idl::offers::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name  = "openbus.component.interface";
    props[static_cast<CORBA::ULong>(1)].value = 
      "IDL:tecgraf/openbus/interop/chaining/HelloProxy:1.0";

    openbus::idl::offers::ServiceOfferDescSeq_var offers(
      find_offers(bus_ctx, props));
    for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
    {
      CORBA::Object_var	o(
        offers[idx].service_ref->getFacetByName("HelloProxy"));
      tecgraf::openbus::interop::chaining::HelloProxy *helloProxy = 
        tecgraf::openbus::interop::chaining::HelloProxy::_narrow(o);
      openbus::idl::offers::ServicePropertySeq properties = offers[idx].properties;
      const char *offer_entity(0);
      for (CORBA::ULong idx = 0; idx != properties.length(); ++idx)
      {
        if (std::string(properties[idx].name) == "openbus.offer.entity")
        {
          offer_entity = properties[idx].value.in();
          break;
        }
      }
      assert(offer_entity != 0);
      openbus::CallerChain chain = bus_ctx->makeChainFor(offer_entity);
      CORBA::OctetSeq encodedChain = bus_ctx->encodeChain(chain);
      const char *msg = helloProxy->fetchHello(
        tecgraf::openbus::interop::chaining::OctetSeq(
          encodedChain.maximum(),
          encodedChain.length(),
          const_cast<unsigned char *> (encodedChain.get_buffer())));
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
  return 0; //MSVC
}
