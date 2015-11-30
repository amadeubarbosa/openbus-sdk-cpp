// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "helloC.h"
#pragma clang diagnostic pop
#include <util.hpp>
#include <tests/config.hpp>
#include <openbus.hpp>

#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <cstdlib>

const std::string entity("interop_rellogedjoin_cpp_client");

namespace cfg = openbus::tests::config;

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
    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(cfg::bus_host_name, 
                                cfg::bus_host_port));
    bus_ctx->setDefaultConnection(conn);
    conn->loginByPassword(entity, entity, cfg::user_password_domain);

    openbus::idl::offers::ServicePropertySeq props;
    props.length(2);
    props[0u].name  = "offer.domain";
    props[0u].value = "Interoperability Tests";
    props[1u].name = "reloggedjoin.role";
    props[1u].value = "proxy";

    openbus::idl::offers::ServiceOfferDescSeq_var offers(
      find_offers(bus_ctx, props));
    if (offers->length() < 1)
    {
      std::cerr << "offers->length() != 0" << std::endl;
      return -1;
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
        std::cerr << "sayHello() nao retornou a string '"
          + msg + "'." << std::endl;
        return -1;
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
