#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>

#include <iostream>

#include <independent_clock.h>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace demo = tecgraf::openbus::demo;
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control
 = tecgraf::openbus::core::v2_0::services::access_control;

template <typename F>
bool call_with_found_clock(offer_registry::ServiceOfferDescSeq_var offers, F f)
{
  if (offers->length() == 0)
  {
    std::cout << "O servico Clock nao se encontra no barramento." << std::endl;
    return false;
  }
  else if(offers->length() == 1)
  {
    CORBA::ULong i = 0;
    demo::Clock_var hello = demo::Clock::_narrow
      (offers[i].service_ref->getFacetByName("hello"));
    if(!CORBA::is_nil(hello))
      f(hello);
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
        demo::Clock_var clock = demo::Clock::_narrow(o);
        if(!CORBA::is_nil(clock))
        {
          f(clock);
          return false;
        }
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
      catch (CORBA::COMM_FAILURE const&)
      {
        std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
          "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
      }
    }
    return false;
  }
  return false;
}

struct printTime
{
  printTime() {}

  typedef void result_type;
  result_type operator()(demo::Clock_var clock) const
  {
    std::cout << "Hora no servidor em ticks: " << clock->getTimeInTicks() << std::endl;
  }
};

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
  // Inicializando CORBA e ativando o RootPOA
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
  assert(!CORBA::is_nil(poa));
  PortableServer::POAManager_var poa_manager = poa->the_POAManager();
  poa_manager->activate();

  // Construindo e logando conexao
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
    (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
  assert(manager != 0);
  std::auto_ptr <openbus::Connection> conn;

  do
  {
    try
    {
      conn = manager->createConnection("localhost", 2089);
      conn->onInvalidLogin( ::onReloginCallback());
      conn->loginByPassword("demo", "demo");
      manager->setDefaultConnection(conn.get());
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
    try
    {
      offer_registry::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
      // Pegando uma oferta valida
      if(! ::call_with_found_clock(offers, printTime()))
      {
        std::cout << "Servidor fora do ar, hora atual " << time(0) << std::endl;
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
    catch (CORBA::COMM_FAILURE const&)
    {
      std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
        "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
    }
    unsigned int t = 30u;
    do { t = sleep(t); } while(t);
  }
  while(true);
}
