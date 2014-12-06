// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include "stubs/hello.h"
#include <scs/ComponentContext.hpp>
#include <configuration.h>
#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

struct hello_impl : public POA_Hello
{
  openbus::Connection& conn;

  hello_impl(openbus::Connection& conn
             , bool& servant_called)
    : conn(conn), servant_called(servant_called)
  {}

  void sayHello()
  {
    std::cout << "Hello" << std::endl;
    servant_called = true;
  }  

  bool& servant_called;
};

#ifdef OPENBUS_SDK_MULTITHREAD
void call_orb(CORBA::ORB_var orb)
{
  orb->run();
}
#endif

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&call_orb, orb));
#endif

  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::auto_ptr <openbus::Connection> conn (openbusContext->createConnection(cfg.host(), cfg.port()));
  openbusContext->setDefaultConnection(conn.get());
  conn->loginByPassword(cfg.user().c_str(), cfg.password().c_str());

  scs::core::ComponentId componentId;
  componentId.name = "Hello";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "";
  {
    CORBA::Object_var poa_obj = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(poa_obj);
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    scs::core::ComponentContext ctx(openbusContext->orb(), componentId);

    bool servant_called = false;
    hello_impl hello_servant (*conn, servant_called);

    ctx.addFacet("hello", "IDL:Hello:1.0", &hello_servant);

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    openbus::idl_or::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "OpenBus Demos";
    props[0] = property;
    openbus::idl_or::ServiceProperty offer_id_prop;
    offer_id_prop.name  = "offer.id";
    offer_id_prop.value = conn->login()->id.in();
    props[1] = offer_id_prop;
    openbusContext->getOfferRegistry()->registerService(ctx.getIComponent(), props);

    props.length(4);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "test";
    props[1].name  = "openbus.component.facet";
    props[1].value = "hello";
    props[2].name  = "offer.domain";
    props[2].value = "OpenBus Demos";
    props[3].name  = "offer.id";
    props[3].value = conn->login()->id.in();
    openbus::idl_or::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
    if (offers->length() < 1)
    {
      std::cerr << "offers->length() < 1" << std::endl;      
      std::abort();
    }
    bool found(false);
    for (CORBA::ULong o(0); o < offers->length(); ++o)
    {
      for (CORBA::ULong p(0); p < offers[o].properties.length(); ++p)
      {
        if (offers[o].properties[p].name.in()
            == std::string("offer.id"))
        {
          if (offers[o].properties[p].value.in()
              == std::string(conn->login()->id.in()))
          {
            found = true;
          }
        }
      }
    }
    if (!found)
    {
      std::cerr << "Nenhuma oferta encontrada com a propriedade 'offer.id' == '"
                << conn->login()->id.in() << "'."
                << std::endl;
      std::abort();
    }
    CORBA::Object_var o = (*offers)[0u].service_ref->getFacetByName("hello");
    Hello* hello = Hello::_narrow(o);

    assert(!servant_called);

    hello->sayHello();
    assert(servant_called);
  }

  conn->logout();
  conn.reset();

#ifdef OPENBUS_SDK_MULTITHREAD
  orb->shutdown(true);
  orb_thread.join();
#endif

}
