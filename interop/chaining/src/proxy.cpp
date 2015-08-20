// -*- coding: iso-8859-1-unix -*-

#include "proxyS.h"
#include "helloC.h"
#include <demo/openssl.hpp>
#include <util.hpp>
#include <openbus.hpp>
#include <scs/ComponentContext.h>

#include <iostream>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/bind.hpp>
#include <boost/program_options.hpp>

const std::string entity("interop_chaining_cpp_proxy");
std::string priv_key_filename;
std::string bus_host;
unsigned short bus_port;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("private-key", po::value<std::string>()->default_value(entity + ".key"),
     "Path to private key")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) 
  {
    std::cout << desc << std::endl;
    std::exit(1);
  }
  if (vm.count("bus.host.name"))
  {
    bus_host = vm["bus.host.name"].as<std::string>();
  }
  if (vm.count("bus.host.port"))
  {
    bus_port = vm["bus.host.port"].as<unsigned short>();
  }
  if (vm.count("private-key"))
  {
    priv_key_filename = vm["private-key"].as<std::string>();
  }
}

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

    openbus::idl::offers::ServiceOfferDescSeq_var offers = 
      ctx.getOfferRegistry()->findServices(props);
    for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
    {
      if (offers[idx].service_ref->_non_existent())
      {
        continue;
      }
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
  EVP_PKEY *priv_key(openbus::demo::openssl::read_priv_key(priv_key_filename));
  if (!priv_key)
  {
    std::cout << "Chave privada inválida." << std::endl;
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
    load_options(argc, argv);
    openbus::log().set_level(openbus::debug_level);

    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    std::auto_ptr<openbus::Connection>
      conn(bus_ctx->connectByAddress(bus_host, bus_port));
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

    openbus::idl::offers::ServicePropertySeq props;
    props.length(1);
    props[static_cast<CORBA::ULong>(0)].name = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";

    conn->onInvalidLogin(on_invalid_login(*bus_ctx, comp, props, *conn));

    HelloProxyImpl proxy(*bus_ctx);
    comp.addFacet("HelloProxy",
		  "IDL:tecgraf/openbus/interop/simple/HelloProxy:1.0",
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
