#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>

#include "independent_clock.h"
#include <CORBA.h>
#include <time.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

namespace offer_registry
 = tecgraf::openbus::core::v2_00::services::offer_registry;
namespace demo = tecgraf::openbus::demo;
namespace services = tecgraf::openbus::core::v2_00::services;
namespace access_control = tecgraf::openbus::core::v2_00::services::access_control;

struct ClockImpl : public POA_tecgraf::openbus::demo::Clock
{
  CORBA::Long getTimeInTicks()
  {
    return time(0);
  }
};

#ifdef OPENBUS_SDK_MULTITHREAD
void run_orb(CORBA::ORB_var orb)
{
  orb->run();
}
#endif

void login(openbus::Connection& conn)
{
  do
  {
    try
    {
      conn.loginByPassword("demo", "demo");
      return;
    }
    catch(CORBA::TRANSIENT const&) {}
    catch(CORBA::OBJECT_NOT_EXIST const&) {}
  } while(true);
}

bool invalid_login_callback(openbus::Connection& conn, access_control::LoginInfo const& i)
{
  try
  {
    login(conn);
    return true;
  }
  catch(openbus::AccessDenied const& e)
  {
    std::cout << "Falha ao tentar realizar o login por senha no barramento: "
      "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
    return false;
  }
}

int main(int argc, char** argv)
{
  try
  {
    // Inicializando CORBA e ativando o RootPOA
    CORBA::ORB_var orb = openbus::initORB(argc, argv);
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
    login(*conn);
    manager->setDefaultConnection(conn.get());

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
    conn->offers()->registerService(clock_component.getIComponent(), properties);

    orb_thread.join();
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
