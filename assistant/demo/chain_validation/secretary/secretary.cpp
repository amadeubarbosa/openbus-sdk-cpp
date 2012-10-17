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

struct MessageImpl : POA_Message
{
  MessageImpl(Message_var executive_message
              , CORBA::ORB_var orb)
    : executive_message(executive_message), orb(orb) {}

  void sendMessage(const char* message)
  {
    std::cout << "Relaying message " << message << std::endl;
    try
    {
      openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
        (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
      openbus::Connection& c = *manager->getRequester();
      c.joinChain(c.getCallerChain());
      executive_message->sendMessage(message);
      std::cout << "Execution succesful" << std::endl;
    }
    catch(::Unavailable const& e)
    {
      std::cout << "Unavaible was thrown" << std::endl;
      throw;
    }
  }

  Message_var executive_message;
  CORBA::ORB_var orb;
};

struct MeetingImpl : POA_Meeting
{
  CORBA::Long bookMeeting()
  {
    std::cout << "Meeting has been booked" << std::endl;
    return 0;
  }
};

::Message_ptr get_message(offer_registry::ServiceOfferDescSeq offers)
{
  if (offers.length() == 0)
  {
    std::cout << "O servico Hello nao se encontra no barramento." << std::endl;
    return ::Message::_nil();
  }
  else if(offers.length() == 1)
  {
    CORBA::ULong i = 0;
    return ::Message::_narrow
      (offers[i].service_ref->getFacetByName("message"));
  }
  else
  {
    std::cout << "Existe mais de um servico Hello no barramento. Tentaremos encontrar uma funcional." << std::endl;

    for(CORBA::ULong i = 0; i != offers.length(); ++i)
    {
      try
      {
        CORBA::Object_var o = offers[i].service_ref
          ->getFacetByName("message");
        return ::Message::_narrow(o);
      }
      catch(CORBA::TRANSIENT const&) {}
      catch(CORBA::OBJECT_NOT_EXIST const&) {}
    }
    return ::Message::_nil();
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
    using namespace openbus::assistant::keywords;
    openbus::assistant::Assistant assistant
      ("localhost", 2089, _username = "secretary", _password = "secretary"
       , _argc = argc, _argv = argv);

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&run_orb, orb));
#endif

    // Recebendo ofertas
    openbus::idl_or::ServicePropertySeq props;
    props.length(3);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "executive";
    props[1].name  = "openbus.component.facet";
    props[1].value = "message";
    offer_registry::ServiceOfferDescSeq offers = assistant.findOffers(props, -1);
    // Pegando uma oferta valida
    ::Message_var message_executive = ::get_message(offers);
    if(CORBA::is_nil(message_executive))
    {
      std::cout << "Error" << std::endl;
      return 1;
    }

    scs::core::ComponentId message_componentId = { "Message", '1', '0', '0', "" };
    scs::core::ComponentContext message_component
      (assistant.orb(), message_componentId);
    MessageImpl message_servant(message_executive, assistant.orb());
    message_component.addFacet
      ("message", ::_tc_Message->id(), &message_servant);
    
    offer_registry::ServicePropertySeq properties;
    properties.length(2);
    properties[0].name = "offer.domain";
    properties[0].value = "Demos";
    assistant.addOffer(message_component.getIComponent(), properties);

    scs::core::ComponentId meeting_componentId = { "Meeting", '1', '0', '0', "" };
    scs::core::ComponentContext meeting_component
      (assistant.orb(), meeting_componentId);
    MeetingImpl meeting_servant;
    meeting_component.addFacet
      ("meeting", ::_tc_Meeting->id(), &meeting_servant);
    
    assistant.addOffer(meeting_component.getIComponent(), properties);

    assistant.wait();
    // orb_thread.join();
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

