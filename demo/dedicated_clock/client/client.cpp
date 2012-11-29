#include <openbus/OpenBusContext.h>
#include <openbus/ORBInitializer.h>
#include <iostream>

#include <stubs/dedicated_clock.h>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace demo = tecgraf::openbus::demo;
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control
 = tecgraf::openbus::core::v2_0::services::access_control;

demo::Clock_ptr get_clock(offer_registry::ServiceOfferDescSeq_var offers)
{
  if (offers->length() == 0)
  {
    std::cout << "O servico Clock nao se encontra no barramento." << std::endl;
    return demo::Clock::_nil();
  }
  else if(offers->length() == 1)
  {
    CORBA::ULong i = 0;
    return demo::Clock::_narrow
      (offers[i].service_ref->getFacetByName("clock"));
  }
  else
  {
    std::cout << "Existe mais de um servico Clock no barramento. Tentaremos encontrar uma funcional." << std::endl;

    for(CORBA::ULong i = 0; i != offers->length(); ++i)
    {
      try
      {
        CORBA::Object_var o = offers[i].service_ref
          ->getFacetByName("clock");
        return demo::Clock::_narrow(o);
      }
      catch(CORBA::TRANSIENT const&) {}
      catch(CORBA::OBJECT_NOT_EXIST const&) {}
    }
    return demo::Clock::_nil();
  }
}

struct onReloginCallback
{
  typedef void result_type;
  result_type operator()(openbus::Connection& c, access_control::LoginInfo info) const
  {
    do
    {
      try
      {
        c.loginByPassword("demo", "demo");
        break;
      }
      catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const& e)
      {
        std::cout << "Falha ao tentar realizar o login por senha no barramento: "
          "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
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
      catch (CORBA::COMM_FAILURE const&)
      {
        std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
          "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
      }
      catch (CORBA::OBJECT_NOT_EXIST const&)
      {
        std::cout << "Objeto remoto nao existe mais. Verifique se o sistema se encontra disponivel" << std::endl;
      }
      unsigned int t = 30u;
      do { t = sleep(t); } while(t);
    }
    while(true);
  }
};

int main(int argc, char** argv)
{
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
  assert(!CORBA::is_nil(poa));
  PortableServer::POAManager_var poa_manager = poa->the_POAManager();
  poa_manager->activate();

  // Construindo e logando conexao
  openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
    (orb->resolve_initial_references("OpenBusContext"));
  assert(openbusContext != 0);
  std::auto_ptr <openbus::Connection> conn;

  do
  {
    try
    {
      conn = openbusContext->createConnection("localhost", 2089);
      conn->onInvalidLogin( ::onReloginCallback());
      conn->loginByPassword("demo", "demo");
      openbusContext->setDefaultConnection(conn.get());
      break;
    }
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const& e)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
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
    catch (CORBA::COMM_FAILURE const&)
    {
      std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
        "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
    }
    catch (CORBA::OBJECT_NOT_EXIST const&)
    {
      std::cout << "Objeto remoto nao existe mais. Verifique se o sistema se encontra disponivel" << std::endl;
    }
    unsigned int t = 30u;
    do { t = sleep(t); } while(t);
  }
  while(true);

  // Recebendo ofertas
  openbus::idl_or::ServicePropertySeq props;
  props.length(2);
  props[0].name  = "openbus.offer.entity";
  props[0].value = "demo";
  props[1].name  = "openbus.component.facet";
  props[1].value = "clock";

  do
  {
    offer_registry::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
    // Pegando uma oferta valida
    demo::Clock_ptr clock = ::get_clock(offers);
    if(!CORBA::is_nil(clock))
    {
      // Chama a funcao
      std::cout << "Hora no servidor em ticks: " << clock->getTimeInTicks() << std::endl;
    }

    unsigned int t = 30u;
    do { t = sleep(t); } while(t);
  }
  while(true);
}
