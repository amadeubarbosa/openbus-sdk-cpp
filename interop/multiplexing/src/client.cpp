// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "helloC.h"
#pragma clang diagnostic pop
#include <util.hpp>
#include <tests/config.hpp>
#include <openbus.hpp>
#include <log/output/file_output.h>

#include <tao/PortableServer/PortableServer.h>
#include <iostream>
#include <map>

const std::string entity("interop_multiplexing_cpp_client");
namespace cfg = openbus::tests::config;
std::map<std::size_t, CORBA::Object_var> buses;

int main(int argc, char **argv) 
{
  try 
  {
    cfg::load_options(argc, argv);
    if (cfg::openbus_test_verbose)
    {
      openbus::log()->set_level(openbus::debug_level);
    }
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    buses[0] = cfg::get_bus_ref(orb_ctx->orb());
    buses[1] = cfg::get_bus2_ref(orb_ctx->orb());
    for (std::size_t busIdx = 0; busIdx != 2; ++busIdx)
    {
      boost::shared_ptr<openbus::Connection> conn(
        bus_ctx->connectByReference(buses[busIdx]));
      bus_ctx->setDefaultConnection(conn);
      conn->loginByPassword(entity, entity, cfg::user_password_domain);

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
          return -1;
        }          
      }
    }
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cerr << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
  return 0; //MSVC
}
