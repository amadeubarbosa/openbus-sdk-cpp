// -*- coding: iso-8859-1-unix -*-
#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <scs/ComponentContext.h>
#include <iostream>

#include <stubs/chain_validation.h>
#include <CORBA.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <fstream>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_0::services;

struct MessageImpl : POA_Message
{
  MessageImpl(Message_var executive_message
              , openbus::OpenBusContext& openbusContext)
    : executive_message(executive_message), openbusContext(openbusContext) {}

  void sendMessage(const char* message)
  {
    std::cout << "Relaying message " << message << std::endl;
    try
    {
      openbusContext.joinChain(openbusContext.getCallerChain());
      executive_message->sendMessage(message);
      std::cout << "Execution succesful" << std::endl;
    }
    catch(::Unavailable const&)
    {
      std::cout << "Unavaible was thrown" << std::endl;
      throw;
    }
  }

  Message_var executive_message;
  openbus::OpenBusContext& openbusContext;
};

struct MeetingImpl : POA_Meeting
{
  CORBA::Long bookMeeting()
  {
    std::cout << "Meeting has been booked" << std::endl;
    return 0;
  }
};

::Message_ptr get_message(offer_registry::ServiceOfferDescSeq_var offers)
{
  if (offers->length() == 0)
  {
    std::cout << "O servico Hello nao se encontra no barramento." << std::endl;
    return ::Message::_nil();
  }
  else if(offers->length() == 1)
  {
    CORBA::ULong i = 0;
    return ::Message::_narrow
      (offers[i].service_ref->getFacetByName("message"));
  }
  else
  {
    std::cout << "Existe mais de um servico Hello no barramento. Tentaremos encontrar uma funcional." << std::endl;

    for(CORBA::ULong i = 0; i != offers->length(); ++i)
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
    CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    boost::optional<openbus::PrivateKey> private_key;
    unsigned short bus_port = 2089;
    std::string bus_host = "localhost";
    {
      namespace po = boost::program_options;
      po::options_description desc("Allowed options");
      desc.add_options()
        ("help", "This help message")
        ("private-key", po::value<std::string>(), "Path to private key")
        ("bus-host", po::value<std::string>(), "Host to Openbus (default: localhost)")
        ("bus-port", po::value<unsigned int>(), "Host to Openbus (default: 2089)")
        ;
      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);
      
      if(vm.count("help") || !vm.count("private-key"))
      {
        std::cout << desc << std::endl;
        return 0;
      }
      std::string private_key_filename = vm["private-key"].as<std::string>();
      private_key = openbus::PrivateKey(private_key_filename);

      if(vm.count("bus-host"))
        bus_host = vm["bus-host"].as<std::string>();
      if(vm.count("bus-port"))
        bus_port = vm["bus-port"].as<unsigned int>();
    }

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&run_orb, orb));
#endif

    // Construindo e logando conexao
    openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    assert(openbusContext != 0);
    std::auto_ptr <openbus::Connection> conn (openbusContext->createConnection(bus_host, bus_port));
    try
    {
      conn->loginByCertificate("secretary", *private_key);
    }
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const&)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    openbusContext->setDefaultConnection(conn.get());

    // Recebendo ofertas
    openbus::idl_or::ServicePropertySeq props;
    props.length(3);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "executive";
    props[1].name  = "openbus.component.facet";
    props[1].value = "message";
    offer_registry::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
    // Pegando uma oferta valida
    ::Message_var message_executive = ::get_message(offers);
    if(CORBA::is_nil(message_executive))
    {
      std::cout << "Error" << std::endl;
      return 1;
    }

    scs::core::ComponentId message_componentId = { "Message", '1', '0', '0', "" };
    scs::core::ComponentContext message_component
      (openbusContext->orb(), message_componentId);
    MessageImpl message_servant(message_executive, *openbusContext);
    message_component.addFacet
      ("message", ::_tc_Message->id(), &message_servant);
    
    offer_registry::ServicePropertySeq properties;
    properties.length(2);
    properties[0].name = "offer.domain";
    properties[0].value = "Demos";
    openbusContext->getOfferRegistry()->registerService(message_component.getIComponent(), properties);

    scs::core::ComponentId meeting_componentId = { "Meeting", '1', '0', '0', "" };
    scs::core::ComponentContext meeting_component
      (openbusContext->orb(), meeting_componentId);
    MeetingImpl meeting_servant;
    meeting_component.addFacet
      ("meeting", ::_tc_Meeting->id(), &meeting_servant);
    
    openbusContext->getOfferRegistry()->registerService(meeting_component.getIComponent(), properties);

#ifdef OPENBUS_SDK_MULTITHREAD
    orb_thread.join();
#else
    orb->run();
#endif
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

