// -*- coding: iso-8859-1-unix -*-

#include "proxyS.h"
#include "helloC.h"
#include <util.hpp>
#include <tests/config.hpp>
#include <openbus/ORBInitializer.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>
#include <openbus/log.hpp>
#include <scs/ComponentContext.h>

#include <iostream>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/bind.hpp>
#include <boost/program_options.hpp>

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
    
    openbus::idl_or::ServicePropertySeq props;
    props.length(3);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name = "openbus.component.name";
    props[static_cast<CORBA::ULong>(1)].value = "RestrictedHello";
    props[static_cast<CORBA::ULong>(2)].name  = "openbus.component.interface";
    props[static_cast<CORBA::ULong>(2)].value = 
      "IDL:tecgraf/openbus/interop/simple/Hello:1.0";

    openbus::idl_or::ServiceOfferDescSeq_var offers(
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
  const openbus::idl_or::ServicePropertySeq &props, openbus::Connection &conn)
{
  try 
  {
    conn.loginByCertificate(entity, openbus::PrivateKey(cfg::system_private_key));
  }
  catch(const openbus::InvalidPrivateKey &e)
  {
    std::cout << e.what() << std::endl;
  }
  ctx.getOfferRegistry()->registerService(comp.getIComponent(), props);
}

struct on_invalid_login
{
  typedef void result_type;
  on_invalid_login(
    const openbus::OpenBusContext &ctx, scs::core::ComponentContext &comp, 
    const openbus::idl_or::ServicePropertySeq &props, 
    openbus::Connection  &conn) 
    : ctx(ctx), comp(comp), props(props), conn(conn)
  {
  }

  result_type operator()(openbus::Connection &c, openbus::idl_ac::LoginInfo l) 
  {
    try 
    {
      std::cout << "invalid login: " << l.id.in() << std::endl; 
      login_register(ctx, comp, props, conn);
    } 
    catch (const CORBA::Exception &e) 
    {
      std::cout << "[error (CORBA::Exception)] " << e << std::endl;    
    }
  }
private:
  const openbus::OpenBusContext &ctx;
  scs::core::ComponentContext &comp;
  const openbus::idl_or::ServicePropertySeq &props;
  openbus::Connection &conn;
};

#ifdef OPENBUS_SDK_MULTITHREAD
void ORBRun(CORBA::ORB_ptr orb)
{
  orb->run();
}
#endif

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
    std::auto_ptr<openbus::Connection>
      conn(bus_ctx->createConnection(cfg::bus_host_name, cfg::bus_host_port));
    bus_ctx->setDefaultConnection(conn.get());

#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread orb_run(boost::bind(ORBRun, bus_ctx->orb()));
#endif

    scs::core::ComponentId componentId;
    componentId.name = "HelloProxy";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "c++";
    scs::core::ComponentContext comp(bus_ctx->orb(), componentId);

    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    props[static_cast<CORBA::ULong>(0)].name = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";

    conn->onInvalidLogin(on_invalid_login(*bus_ctx, comp, props, *conn));

    HelloProxyImpl proxy(*bus_ctx);
    comp.addFacet("HelloProxy",
		  "IDL:tecgraf/openbus/interop/chaining/HelloProxy:1.0",
                  &proxy);
    login_register(*bus_ctx, comp, props, *conn);
#ifdef OPENBUS_SDK_MULTITHREAD
    orb_run.join();
#else
    bus_ctx->orb()->run();
#endif
  }
  catch (const CORBA::Exception &e)
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
  return 0; //MSVC
}
