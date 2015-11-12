// -*- coding: iso-8859-1-unix -*-

#include "proxyS.h"
#include "helloC.h"
#include <util.hpp>
#include <tests/config.hpp>
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

namespace cfg = openbus::tests::config;

const std::string entity("interop_chaining_cpp_client");

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
    CORBA::Object_var o = orb_ctx->orb()->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    CORBA::Object_var
      obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
    openbus::OpenBusContext
      *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

    std::auto_ptr<openbus::Connection> conn(bus_ctx->createConnection(cfg::bus_host_name, 
                                                                  cfg::bus_host_port));
    bus_ctx->setDefaultConnection(conn.get());
    conn->loginByPassword(entity, entity);

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name  = "openbus.component.interface";
    props[static_cast<CORBA::ULong>(1)].value = 
      "IDL:tecgraf/openbus/interop/chaining/HelloProxy:1.0";

    openbus::idl_or::ServiceOfferDescSeq_var offers(
      find_offers(bus_ctx, props));
    for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
    {
      CORBA::Object_var	o(offers[idx].service_ref->getFacetByName("HelloProxy"));
      tecgraf::openbus::interop::chaining::HelloProxy *helloProxy = 
        tecgraf::openbus::interop::chaining::HelloProxy::_narrow(o);
      openbus::idl_or::ServicePropertySeq properties = offers[idx].properties;
      const char *loginId(0);
      for (CORBA::ULong idx = 0; idx != properties.length(); ++idx)
      {
        if (std::string(properties[idx].name) == "openbus.offer.login")
        {
          loginId = properties[idx].value.in();
          break;
        }
      }
      assert(loginId != 0);
      openbus::CallerChain chain = bus_ctx->makeChainFor(loginId);
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
