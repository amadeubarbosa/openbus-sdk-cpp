// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "helloS.h"
#pragma clang diagnostic pop
#include <demo/openssl.hpp>
#include <scs/ComponentContext.h>
#include <openbus.hpp>

#include <iostream>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/regex.hpp>
#pragma clang diagnostic pop

namespace cfg = openbus::test::config;

const std::string entity("interop_chaining_cpp_server");

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello 
{
  HelloImpl(openbus::OpenBusContext &c) : ctx(c) 
  { 
  }

  char *sayHello() 
  {
    openbus::CallerChain chain = ctx.getCallerChain();
    assert(chain != openbus::CallerChain());
    std::string msg;
    if (boost::regex_match(chain.caller().entity.in(),
                           boost::regex("interop_chaining_.+_proxy")))
    {
       msg = "Hello " + std::string(chain.originators()[0].entity) + "!";
       std::cout << msg << std::endl;
       CORBA::String_var r = CORBA::string_dup(msg.c_str());
       return r._retn();
    }
    throw CORBA::NO_PERMISSION();
  }
private:
  openbus::OpenBusContext &ctx;
};

void login_register(
  const openbus::OpenBusContext &ctx, scs::core::ComponentContext &comp,
  const openbus::idl::offers::ServicePropertySeq &props, openbus::Connection &conn)
{
  EVP_PKEY *priv_key(
    openbus::demo::openssl::read_priv_key(cfg::system_private_key));
  if (!priv_key)
  {
    std::cout << "Chave privada invalida." << std::endl;
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
      std::cout << "[error (CORBA::Exception)] " << e << std::endl;    
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
    boost::shared_ptr<openbus::Connection>
      conn(bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
    bus_ctx->setDefaultConnection(conn);

    boost::thread orb_run(boost::bind(ORBRun, bus_ctx->orb()));

    scs::core::ComponentId componentId;
    componentId.name = "RestrictedHello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "c++";
    scs::core::ComponentContext comp(bus_ctx->orb(), componentId);

    openbus::idl::offers::ServicePropertySeq props;
    props.length(1);
    props[0u].name = "offer.domain";
    props[0u].value = "Interoperability Tests";

    conn->onInvalidLogin(on_invalid_login(*bus_ctx, comp, props, *conn));

    HelloImpl srv(*bus_ctx);
    comp.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0",
                  &srv);
    login_register(*bus_ctx, comp, props, *conn);
    orb_run.join();
  }
  catch (const CORBA::Exception &e)
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
  return 0; //MSVC
}
