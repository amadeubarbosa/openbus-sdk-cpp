// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "proxyC.h"
#pragma clang diagnostic pop
#include <openbus.hpp>
#include <util.hpp>
#include <tests/config.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#pragma clang diagnostic pop
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <cstdlib>

namespace cfg = openbus::tests::config;

const std::string entity("interop_chaining_cpp_client");

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
    CORBA::Object_var o = orb_ctx->orb()->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    CORBA::Object_var
      obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
    openbus::OpenBusContext
      *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(cfg::bus_host_name, 
                                cfg::bus_host_port));
    bus_ctx->setDefaultConnection(conn);
    conn->loginByPassword(entity, entity, cfg::user_password_domain);

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
    std::cerr << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
  return 0; //MSVC
}
