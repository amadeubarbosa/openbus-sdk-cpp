// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "proxyS.h"
#include "helloC.h"
#pragma clang diagnostic pop
#include <demo/openssl.hpp>
#include <util.hpp>
#include <tests/config.hpp>
#include <openbus.hpp>
#include <scs/ComponentContext.h>

#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace cfg = openbus::tests::config;

const std::string entity("interop_chaining_cpp_proxy");

struct HelloProxyImpl :
  virtual public POA_tecgraf::openbus::interop::chaining::HelloProxy
{
  HelloProxyImpl(openbus::OpenBusContext &c) : ctx(c)
  {
  }

  char *fetchHello(
    const ::tecgraf::openbus::interop::chaining::OctetSeq &encodedChain)
  {
    openbus::CallerChain chain = ctx.decodeChain(
      CORBA::OctetSeq(encodedChain.maximum(), encodedChain.length(),
                      const_cast<unsigned char *>(encodedChain.get_buffer())));
    assert(chain != openbus::CallerChain());
    ctx.joinChain(chain);
    
    openbus::idl::offers::ServicePropertySeq props;
    props.length(3);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name = "openbus.component.name";
    props[static_cast<CORBA::ULong>(1)].value = "RestrictedHello";
    props[static_cast<CORBA::ULong>(2)].name  = "openbus.component.interface";
    props[static_cast<CORBA::ULong>(2)].value = 
      "IDL:tecgraf/openbus/interop/simple/Hello:1.0";

    openbus::idl::offers::ServiceOfferDescSeq_var offers(
      find_offers(&ctx, props));
    for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
    {
      CORBA::Object_var o = offers[idx].service_ref->getFacetByName("Hello");
      tecgraf::openbus::interop::simple::Hello *hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);

      return hello->sayHello();
    }

    return 0;
  }
private:
  openbus::OpenBusContext &ctx;
};

void login_register(
  const openbus::OpenBusContext &ctx, scs::core::ComponentContext &comp,
  const openbus::idl::offers::ServicePropertySeq &props, openbus::Connection &conn)
{
  EVP_PKEY *priv_key(openbus::demo::openssl::read_priv_key(cfg::system_private_key));
  if (!priv_key)
  {
    std::cerr << "Chave privada invalida." << std::endl;
    return;
  }
  conn.loginByCertificate(entity, priv_key);
  ctx.getOfferRegistry()->registerService(comp.getIComponent(), props);
}

struct on_invalid_login
{
  typedef void result_type;
  on_invalid_login(
    const openbus::OpenBusContext &ctx, scs::core::ComponentContext &comp, 
    const openbus::idl::offers::ServicePropertySeq &props, 
    openbus::Connection  &conn) 
    : ctx(ctx), comp(comp), props(props), conn(conn)
  {
  }

  result_type operator()(openbus::Connection &c, openbus::idl::access::LoginInfo l) 
  {
    try 
    {
      std::cout << "invalid login: " << l.id.in() << std::endl; 
      login_register(ctx, comp, props, conn);
    } 
    catch (const CORBA::Exception &e) 
    {
      std::cerr << "[error (CORBA::Exception)] " << e << std::endl;    
    }
  }
private:
  const openbus::OpenBusContext &ctx;
  scs::core::ComponentContext &comp;
  const openbus::idl::offers::ServicePropertySeq &props;
  openbus::Connection &conn;
};

void ORBRun(CORBA::ORB_ptr orb)
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
    boost::shared_ptr<openbus::Connection>
      conn(bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
    bus_ctx->setDefaultConnection(conn);

    boost::thread orb_run(boost::bind(ORBRun, bus_ctx->orb()));

    scs::core::ComponentId componentId;
    componentId.name = "HelloProxy";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "c++";
    scs::core::ComponentContext comp(bus_ctx->orb(), componentId);

    openbus::idl::offers::ServicePropertySeq props;
    props.length(1);
    props[static_cast<CORBA::ULong>(0)].name = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";

    conn->onInvalidLogin(on_invalid_login(*bus_ctx, comp, props, *conn));

    HelloProxyImpl proxy(*bus_ctx);
    comp.addFacet("HelloProxy",
		  "IDL:tecgraf/openbus/interop/chaining/HelloProxy:1.0",
                  &proxy);
    login_register(*bus_ctx, comp, props, *conn);
    orb_run.join();
  }
  catch (const CORBA::Exception &e)
  {
    std::cerr << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
  return 0; //MSVC
}
