#include <openbus/assistant.h>
#include <scs/ComponentContext.h>
#include <iostream>

#include <stubs/chain_validation.h>
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
  MessageImpl(CORBA::ORB_var orb)
    : orb(orb)
  {}

  void sendMessage(const char* message)
  {
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    openbus::Connection* c = manager->getRequester();
    openbus::CallerChain chain = c->getCallerChain();
    if(chain != openbus::CallerChain()
       && !std::strcmp(chain.caller().entity, "secretary"))
    {
      std::cout << "Hello" << std::endl;
    }
    else
    {
      const char* name = "[unknown]";
      if(chain != openbus::CallerChain())
        name = chain.caller().entity;
      std::cout << "I'm unavailable to " << name << std::endl;
      throw ::Unavailable();
    }
  }

  CORBA::ORB_var orb;
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
    using namespace openbus::assistant::keywords;
    openbus::assistant::Assistant assistant
      ("localhost", 2089, _username = "executive", _password = "executive"
       , _argc = argc, _argv = argv);

#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread orb_thread(boost::bind(&run_orb, assistant.orb()));
#endif

    scs::core::ComponentId componentId = { "Message", '1', '0', '0', "" };
    scs::core::ComponentContext message_component
      (assistant.orb(), componentId);
    MessageImpl message_servant(assistant.orb());
    message_component.addFacet
      ("message", ::_tc_Message->id(), &message_servant);
    
    offer_registry::ServicePropertySeq properties;
    properties.length(2);
    properties[0].name = "offer.domain";
    properties[0].value = "Demos";
    assistant.addOffer(message_component.getIComponent(), properties);

    assistant.wait();
    // orb_thread.join();
  }
  catch (services::ServiceFailure e)
  {
    std::cout << "Falha no servi�o remoto. Causa: " << std::endl;
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
