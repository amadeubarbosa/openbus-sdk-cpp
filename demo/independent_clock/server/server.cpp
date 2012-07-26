#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <scs/ComponentContext.h>
#include <iostream>

#include "independent_clock.h"
#include <CORBA.h>
#include <time.h>

#ifndef OPENBUS_SDK_MULTITHREAD
#error This demo requires multithread
#endif

#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace demo = tecgraf::openbus::demo;
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control = tecgraf::openbus::core::v2_0::services::access_control;

struct ClockImpl : public POA_tecgraf::openbus::demo::Clock
{
  CORBA::Long getTimeInTicks()
  {
    return time(0);
  }
};

void run_orb(CORBA::ORB_var orb)
{
  orb->run();
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
      catch(openbus::AccessDenied const& e)
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

void clock_loop()
{
  while(true)
  {
    std::cout << "Hora local atual em ticks: " << time(0) << std::endl;
    unsigned int t = 30u;
    do { t = sleep(t); } while(t);
  }
}

int main(int argc, char** argv)
{
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
  assert(!CORBA::is_nil(poa));
  PortableServer::POAManager_var poa_manager = poa->the_POAManager();
  poa_manager->activate();

  boost::thread orb_thread(boost::bind(&run_orb, orb));

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
    catch(openbus::AccessDenied const& e)
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

  scs::core::ComponentId componentId
    = { "IndependentClock", '1', '0', '0', ""};
  scs::core::ComponentContext clock_component
    (manager->orb(), componentId);
  ClockImpl clock_servant;
  clock_component.addFacet
    ("clock", demo::_tc_Clock->id(), &clock_servant);
  offer_registry::ServicePropertySeq properties;
  properties.length(1);
  properties[0].name = "offer.domain";
  properties[0].value = "Demos";

  boost::thread local_clock_thread(& ::clock_loop);

  try
  {
    do
    {
      try
      {
        conn->offers()->registerService(clock_component.getIComponent(), properties);
        break;
      }
      catch(openbus::AccessDenied const& e)
      {
        std::cout << "Falha ao tentar realizar o login por senha no barramento: "
          "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      }
      catch (services::ServiceFailure const& e)
      {
        std::cout << "Falha no servico remoto. Causa: " << std::endl;
      }
      catch (offer_registry::UnauthorizedFacets const& e)
      {
        std::cout << "Faceta nao autorizada no barramento: " << std::endl;
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
  
    orb_thread.join();
    local_clock_thread.join();
  }
  catch(offer_registry::InvalidService const&)
  {
    std::cout << "Barramento diz que o servico ofertado nao e um componente valido" << std::endl;
  }
  catch(offer_registry::InvalidProperties const&)
  {
    std::cout << "Barramento diz que o servico ofertado possui ofertas" << std::endl;
  }
}
