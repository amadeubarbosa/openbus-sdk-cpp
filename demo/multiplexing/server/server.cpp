#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <scs/ComponentContext.h>
#include <iostream>

#include "greetings.h"
#include <CORBA.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_0::services;

struct GreetingsImpl : virtual public ::POA_Greetings
{
  GreetingsImpl(std::string const& greeting)
    : greeting(greeting) {}
  char* sayGreetings()
  {
    return CORBA::string_dup(greeting.c_str());
  }
  std::string greeting;
};

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
    std::auto_ptr <openbus::Connection> conn1 (manager->createConnection("localhost", 2089));
    std::auto_ptr <openbus::Connection> conn2 (manager->createConnection("localhost", 2089));
    std::auto_ptr <openbus::Connection> conn3 (manager->createConnection("localhost", 2089));
    try
    {
      conn1->loginByPassword("demo1", "demo1");
      conn2->loginByPassword("demo2", "demo2");
      conn3->loginByPassword("demo3", "demo3");
    }
    catch(openbus::AccessDenied const& e)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    manager->setDispatcher(*conn2);

    scs::core::ComponentId componentId = { "Greetings", '1', '0', '0', "" };
    scs::core::ComponentContext english_greetings_component(orb, componentId);
    GreetingsImpl english_greetings_servant("Hello");
    english_greetings_component.addFacet
      ("greetings", _tc_Greetings->id(), &english_greetings_servant);

    scs::core::ComponentContext portuguese_greetings_component(orb, componentId);
    GreetingsImpl portuguese_greetings_servant("OlÃ¡");
    portuguese_greetings_component.addFacet
      ("greetings", _tc_Greetings->id(), &portuguese_greetings_servant);

    scs::core::ComponentContext german_greetings_component(orb, componentId);
    GreetingsImpl german_greetings_servant("OlÃ¡ em alemao");
    german_greetings_component.addFacet
      ("greetings", _tc_Greetings->id(), &german_greetings_servant);
    
    manager->setRequester(conn1.get());
    offer_registry::ServicePropertySeq properties;
    properties.length(2);
    properties[0].name = "offer.domain";
    properties[0].value = "Demos";
    properties[1].name = "language";
    properties[1].name = "english";
    conn1->offers()->registerService(english_greetings_component.getIComponent(), properties);

    manager->setRequester(conn2.get());
    properties[1].name = "language";
    properties[1].name = "portuguese";
    conn2->offers()->registerService(portuguese_greetings_component.getIComponent(), properties);

    manager->setRequester(conn3.get());
    properties[1].name = "language";
    properties[1].name = "german";
    conn3->offers()->registerService(german_greetings_component.getIComponent(), properties);

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

