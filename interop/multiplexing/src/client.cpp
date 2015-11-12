// -*- coding: iso-8859-1-unix -*-

#include "helloC.h"
#include <util.hpp>
#include <tests/config.hpp>
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>

#include <tao/PortableServer/PortableServer.h>
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
namespace cfg = openbus::tests::config;

int main(int argc, char **argv) 
{
  try 
  {
    cfg::load_options(argc, argv);
    if (cfg::openbus_test_verbose)
    {
      openbus::log().set_level(openbus::debug_level);
    }
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    buses[0].host = cfg::bus_host_name;
    buses[0].port = cfg::bus_host_port;
    buses[1].host = cfg::bus2_host_name;
    buses[1].port = cfg::bus2_host_port;
    for (std::size_t busIdx = 0; busIdx != 2; ++busIdx)
    {
      std::auto_ptr<openbus::Connection> 
        conn(bus_ctx->createConnection(buses[busIdx].host, buses[busIdx].port));
      bus_ctx->setDefaultConnection(conn.get());
      conn->loginByPassword(entity, entity);

      openbus::idl_or::ServicePropertySeq props;
      props.length(2);
      props[static_cast<CORBA::ULong>(0)].name  = "openbus.component.interface";
      props[static_cast<CORBA::ULong>(0)].value = 
        "IDL:tecgraf/openbus/interop/simple/Hello:1.0";
      props[static_cast<CORBA::ULong>(1)].name  = "offer.domain";
      props[static_cast<CORBA::ULong>(1)].value = "Interoperability Tests";

      openbus::idl_or::ServiceOfferDescSeq_var offers = 
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
          std::cerr << "sayHello() não retornou a string '"
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
