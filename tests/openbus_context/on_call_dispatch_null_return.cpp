// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "helloS.h"
#pragma clang diagnostic pop

#include <config.hpp>
#include <demo/openssl.hpp>
#include <scs/ComponentContext.h>
#include <openbus.hpp>

#include <string>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

bool call_dispatch(false);

struct dispatcher
{
  typedef boost::shared_ptr<openbus::Connection> result_type;
  result_type r;
  dispatcher(result_type r) : r(r) {}

  result_type operator()(
    openbus::OpenBusContext &context,
    const std::string busId,
    const std::string loginId,
    const std::string operation) const
  {
    call_dispatch = true;
    return boost::shared_ptr<openbus::Connection>();
  }  
};

struct hello_impl : public POA_Hello
{
  boost::shared_ptr<openbus::Connection> conn;

  hello_impl(
    boost::shared_ptr<openbus::Connection> conn,
    bool &servant_called)
    : conn(conn)
    , servant_called(servant_called)
  {}

  void sayHello()
  {
    std::cout << "Hello" << std::endl;
    servant_called = true;
  }  

  bool &servant_called;
};

void call_orb(CORBA::ORB_var orb)
{
  orb->run();
}

int main(int argc, char* argv[])
{
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));

  boost::thread orb_thread(boost::bind(&call_orb, orb_ctx->orb()));

  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  EVP_PKEY *priv_key(openbus::demo::openssl::read_priv_key(cfg::system_private_key));
  if (!priv_key)
  {
    std::cerr << "Chave privada invalida." << std::endl;
    std::abort();
  }
  conn->loginByCertificate(cfg::system_entity_name, priv_key);
  bus_ctx->onCallDispatch(dispatcher(conn));
  
  scs::core::ComponentId componentId;
  componentId.name = "Hello";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "";
  
  CORBA::Object_var
    poa_obj(orb_ctx->orb()->resolve_initial_references("RootPOA"));
  PortableServer::POA_var poa(PortableServer::POA::_narrow(poa_obj));
  PortableServer::POAManager_var poa_manager(poa->the_POAManager());
  poa_manager->activate();

  scs::core::ComponentContext ctx(bus_ctx->orb(), componentId);

  bool servant_called(false);
  hello_impl hello_servant(conn, servant_called);

  ctx.addFacet("hello", "IDL:Hello:1.0", &hello_servant);

  openbus::idl::offers::ServicePropertySeq props;
  props.length(2);
  openbus::idl::offers::ServiceProperty property;
  property.name = "offer.domain";
  property.value = "OpenBus Demos";
  props[0] = property;
  openbus::idl::offers::ServiceProperty offer_id_prop;
  offer_id_prop.name = "offer.id";
  offer_id_prop.value = conn->login()->id.in();
  props[1] = offer_id_prop;
  bus_ctx->setDefaultConnection(conn);
  bus_ctx->getOfferRegistry()->registerService(ctx.getIComponent(), props);

  props.length(4);
  props[0].name  = "openbus.offer.entity";
  props[0].value = cfg::system_entity_name.c_str();
  props[1].name  = "openbus.component.facet";
  props[1].value = "hello";
  props[2].name  = "offer.domain";
  props[2].value = "OpenBus Demos";
  props[3].name  = "offer.id";
  props[3].value = conn->login()->id.in();
  openbus::idl::offers::ServiceOfferDescSeq_var offers(
    bus_ctx->getOfferRegistry()->findServices(props));

  if(offers->length() == 1)
  {
    CORBA::Object_var
      hello_obj((*offers)[0u].service_ref->getFacetByName("hello"));
    ::Hello_var hello = ::Hello::_narrow(hello_obj);
    hello->sayHello();
  }
  else
  {    
    std::cerr << "offers->length() == "
              << offers->length() << std::endl;      
    std::abort();
  }
  if (!call_dispatch)
  {
    std::cerr << "call_dispatch == false" << std::endl;
    std::abort();
  }
  orb_ctx->orb()->shutdown(true);
  orb_thread.join();
  return 0; //MSVC
}
