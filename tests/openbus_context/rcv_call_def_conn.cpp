// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "helloS.h"
#pragma clang diagnostic pop

#include <scs/ComponentContext.h>
#include <config.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

struct hello_impl : public POA_Hello
{
  openbus::Connection& conn;

  hello_impl(openbus::Connection& conn)
    : conn(conn)
  {}

  void sayHello()
  {
    std::cout << "Hello" << std::endl;
  }  
};

void call_orb(CORBA::ORB_var orb)
{
  orb->run();
}

int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  if (cfg::openbus_test_verbose)
  {
    openbus::log()->set_level(openbus::debug_level);
  }

  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));

  boost::thread orb_thread(boost::bind(&call_orb, orb_ctx->orb()));

  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  bus_ctx->setDefaultConnection(conn);
  EVP_PKEY *priv_key(openbus::demo::openssl::read_priv_key(cfg::system_private_key));
  if (!priv_key)
  {
    std::cerr << "Chave privada invalida." << std::endl;
    std::abort();
  }
  conn->loginByCertificate(cfg::system_entity_name, priv_key);

  scs::core::ComponentId componentId;
  componentId.name = "Hello";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "";    
  CORBA::Object_var
    poa_obj(orb_ctx->orb()->resolve_initial_references("RootPOA"));
  PortableServer::POA_var poa = PortableServer::POA::_narrow(poa_obj);
  PortableServer::POAManager_var poa_manager = poa->the_POAManager();
  poa_manager->activate();

  scs::core::ComponentContext ctx(bus_ctx->orb(), componentId);
  
  hello_impl hello_servant (*conn);

  ctx.addFacet("hello", "IDL:Hello:1.0", &hello_servant);

  boost::uuids::uuid uuid = boost::uuids::random_generator()();
  openbus::idl::offers::ServicePropertySeq props;
  props.length(2);
  openbus::idl::offers::ServiceProperty property;
  property.name = "offer.domain";
  property.value = "OpenBus Demos";
  props[0] = property;
  props[1].name = "uuid";
  props[1].value = boost::uuids::to_string(uuid).c_str();
  bus_ctx->getOfferRegistry()->registerService(ctx.getIComponent(), props);

  props.length(4);
  props[0].name  = "openbus.offer.entity";
  props[0].value = cfg::system_entity_name.c_str();
  props[1].name  = "openbus.component.facet";
  props[1].value = "hello";
  props[2].name  = "offer.domain";
  props[2].value = "OpenBus Demos";
  props[3].name  = "uuid";
  props[3].value = boost::uuids::to_string(uuid).c_str();
  
  openbus::idl::offers::ServiceOfferDescSeq_var offers(
    bus_ctx->getOfferRegistry()->findServices(props));
  std::cout << offers->length() << std::endl;
  if (offers->length() != 1)
  {
    std::cerr << "offers->length() != 1" << std::endl;
    std::abort();
  }
  CORBA::Object_var o((*offers)[0u].service_ref->getFacetByName("hello"));
  Hello *hello(Hello::_narrow(o));
  hello->sayHello();

  conn->logout();
  conn.reset();

  orb_ctx->orb()->shutdown(true);
  orb_thread.join();
  return 0; //MSVC
}
