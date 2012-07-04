#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <scs/ComponentContext.h>
#include <iostream>

#include "chain_validation.h"
#include <CORBA.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_0::services;

struct MessageImpl : public POA_Message
{
  MessageImpl(openbus::ConnectionManager& manager)
    : manager(manager) {}

  void sendMessage(const char* message)
  {
    openbus::Connection* c = manager.getRequester();
    openbus::CallerChain* chain = c->getCallerChain();
    if(chain && !std::strcmp(chain->caller().entity, "secretary"))
    {
      std::cout << "Hello" << std::endl;
    }
    else
    {
      const char* name = "[unknown]";
      if(chain)
        name = chain->caller().entity;
      std::cout << "I'm unavailable to " << name << std::endl;
      throw ::Unavailable();
    }
  }

  openbus::ConnectionManager& manager;
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
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    try
    {
      conn->loginByPassword("executive", "executive");
    }
    catch(openbus::AccessDenied const& e)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    manager->setDefaultConnection(conn.get());

    scs::core::ComponentId componentId = { "Message", '1', '0', '0', "" };
    scs::core::ComponentContext message_component
      (manager->orb(), componentId);
    MessageImpl message_servant(*manager);
    message_component.addFacet
      ("message", ::_tc_Message->id(), &message_servant);
    
    offer_registry::ServicePropertySeq properties;
    properties.length(2);
    properties[0].name = "offer.domain";
    properties[0].value = "Demos";
    conn->offers()->registerService(message_component.getIComponent(), properties);

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

