#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <scs/ComponentContext.h>
#include <iostream>

#include "hello.h"
#include <CORBA.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace simple = tecgraf::openbus::interop::simple;
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control = tecgraf::openbus::core::v2_0::services::access_control;

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello
{
  HelloImpl(simple::Hello_var hello
            , openbus::ConnectionManager& manager)
    : hello(hello), manager(manager) {}

  void sayHello()
  {
    std::cout << "Hello called on proxy" << std::endl;
    openbus::Connection* c = manager.getRequester();
    openbus::CallerChain* chain = c->getCallerChain();
    c->joinChain(chain);
    if(chain)
    {
      std::cout << "Caller: " << chain->caller().entity << std::endl;
      access_control::LoginInfoSeq originators = chain->originators();
      for(std::size_t i = 0; i != originators.length(); ++i)
      {
        std::cout << "Originator: " << originators[i].entity << std::endl;
      }
    }
    hello->sayHello();
  }

  simple::Hello_var hello;
  openbus::ConnectionManager& manager;
};

simple::Hello_ptr get_hello(offer_registry::ServiceOfferDescSeq_var offers)
{
  if (offers->length() == 0)
  {
    std::cout << "O servico Hello nao se encontra no barramento." << std::endl;
    return simple::Hello::_nil();
  }
  else if(offers->length() == 1)
  {
    CORBA::ULong i = 0;
    return simple::Hello::_narrow
      (offers[i].service_ref->getFacetByName("hello"));
  }
  else
  {
    std::cout << "Existe mais de um servico Hello no barramento. Tentaremos encontrar uma funcional." << std::endl;

    for(CORBA::ULong i = 0; i != offers->length(); ++i)
    {
      try
      {
        CORBA::Object_var o = offers[i].service_ref
          ->getFacetByName("hello");
        return simple::Hello::_narrow(o);
      }
      catch(CORBA::TRANSIENT const&) {}
      catch(CORBA::OBJECT_NOT_EXIST const&) {}
      catch(CORBA::COMM_FAILURE const&) {}
    }
    return simple::Hello::_nil();
  }
}

#ifdef OPENBUS_SDK_MULTITHREAD
void run_orb(CORBA::ORB_var orb)
{
  orb->run();
}
#endif

int main(int argc, char** argv)
{
  try
  {
    // Inicializando CORBA e ativando o RootPOA
    CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&run_orb, orb));
#endif

    // Construindo e logando conexao
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    assert(manager != 0);
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    try
    {
      conn->loginByPassword("proxy", "proxy");
    }
    catch(openbus::AccessDenied const& e)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    manager->setDefaultConnection(conn.get());

    // Recebendo ofertas
    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "server";
    props[1].name  = "openbus.component.facet";
    props[1].value = "hello";
    offer_registry::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
    // Pegando uma oferta valida
    simple::Hello_ptr hello = ::get_hello(offers);
    if(!CORBA::is_nil(hello))
    {
      scs::core::ComponentId componentId;
      componentId.name = "Hello";
      componentId.major_version = '1';
      componentId.minor_version = '0';
      componentId.patch_version = '0';
      componentId.platform_spec = "";
      scs::core::ComponentContext hello_component
        (manager->orb(), componentId);
      HelloImpl hello_servant(hello, *manager);
      hello_component.addFacet
        ("hello", simple::_tc_Hello->id(), &hello_servant);
    
      offer_registry::ServicePropertySeq properties;
      properties.length(1);
      properties[0].name = "offer.domain";
      properties[0].value = "Demos";
      conn->offers()->registerService(hello_component.getIComponent(), properties);

      orb_thread.join();
    }
  }
  catch (services::ServiceFailure e)
  {
    std::cout << "Falha no serviço remoto. Causa: " << std::endl;
  }
  catch (CORBA::TRANSIENT const&)
  {
    std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
      "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
  }
  catch (CORBA::OBJECT_NOT_EXIST const&)
  {
    std::cout << "Objeto remoto nao existe mais. Verifique se o sistema se encontra disponivel" << std::endl;
  }
  return 1;
}
