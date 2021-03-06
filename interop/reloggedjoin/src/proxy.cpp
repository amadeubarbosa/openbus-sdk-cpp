// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "helloS.h"
#pragma clang diagnostic pop
#include <util.hpp>
#include <tests/config.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>
#include <scs/ComponentContext.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#pragma clang diagnostic pop
#include <iostream>
#include <fstream>
#include <typeinfo>

const std::string entity("interop_reloggedjoin_cpp_proxy");

namespace cfg = openbus::tests::config;

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello 
{
  HelloImpl(openbus::OpenBusContext &c) : bus_ctx(c) 
  { 
  }

  char *sayHello() 
  {
    try
    {
      boost::shared_ptr<openbus::Connection> conn(
        bus_ctx.getCurrentConnection());
      conn->logout();
      EVP_PKEY *priv_key(
        openbus::demo::openssl::read_priv_key(cfg::system_private_key));
      if (!priv_key)
      {
        std::cerr << "Chave privada invalida." << std::endl;
        std::exit(-1);
      }
      conn->loginByCertificate(entity, priv_key);
      openbus::CallerChain chain = bus_ctx.getCallerChain();
      if (chain == openbus::CallerChain())
      {
        std::exit(-1);
      }
      std::string entity(chain.caller().entity);
      std::string msg("Hello " + entity + "!");

      openbus::idl::offers::ServicePropertySeq props;
      props.length(2);
      props[0].name  = "offer.domain";
      props[0].value = "Interoperability Tests";
      props[1].name = "reloggedjoin.role";
      props[1].value = "server";

      bus_ctx.joinChain(chain);

      openbus::idl::offers::ServiceOfferDescSeq_var offers(
        find_offers(&bus_ctx, props));
      if (offers->length() < 1)
      {
        std::cerr << "offers->length() != 0" << std::endl;
        std::exit(-1);
      }
      for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
      {
        CORBA::Object_var o = offers[idx].service_ref->getFacetByName("Hello");
        tecgraf::openbus::interop::simple::Hello_var hello = 
          tecgraf::openbus::interop::simple::Hello::_narrow(o);
        return hello->sayHello();
      }
    }
    catch (const CORBA::SystemException &e)
    {
      std::cerr << "repid: " << e._rep_id() << "minor code: " << e.minor()
                << std::endl;
      std::exit(-1);
    }
    return CORBA::string_dup("");
  }
private:
  openbus::OpenBusContext &bus_ctx;
};

void loginAndRegister(
  const openbus::OpenBusContext &bus_ctx, scs::core::ComponentContext &comp,
  const openbus::idl::offers::ServicePropertySeq &props,
  boost::shared_ptr<openbus::Connection> conn)
{
  EVP_PKEY *priv_key(
    openbus::demo::openssl::read_priv_key(cfg::system_private_key));
  if (!priv_key)
  {
    std::cerr << "Chave privada invalida." << std::endl;
    return;
  }
  conn->loginByCertificate(entity, priv_key);
  bus_ctx.getOfferRegistry()->registerService(comp.getIComponent(), props);
}

void ORBRun(CORBA::ORB_var orb)
{
  orb->run();
}

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
    CORBA::Object_var bus_ref(cfg::get_bus_ref(orb_ctx->orb()));
    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByReference(bus_ref));
    bus_ctx->setDefaultConnection(conn);    
    boost::thread orb_run(boost::bind(ORBRun, bus_ctx->orb()));

    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "c++";
    scs::core::ComponentContext comp(bus_ctx->orb(), componentId);

    openbus::idl::offers::ServicePropertySeq props;
    props.length(2);
    props[0].name = "offer.domain";
    props[0].value = "Interoperability Tests";
    props[1].name = "reloggedjoin.role";
    props[1].value = "proxy";

    HelloImpl srv(*bus_ctx);
    comp.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0",&srv);
    loginAndRegister(*bus_ctx, comp, props, conn);

    orb_run.join();
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cerr << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
  return 0; //MSVC
}
