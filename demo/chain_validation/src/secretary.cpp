// -*- coding: iso-8859-1-unix -*-

#include "chain_validationS.h"
#include <openbus.hpp>
#include <scs/ComponentContext.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif
#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>

namespace offer_registry
 = tecgraf::openbus::core::v2_1::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_1::services;

struct MessageImpl : POA_Message
{
  MessageImpl(Message_var executive_message
              , openbus::OpenBusContext& bus_ctx)
    : executive_message(executive_message), bus_ctx(bus_ctx) {}

  void sendMessage(const char* message)
  {
    std::cout << "Relaying message " << message << std::endl;
    try
    {
      bus_ctx.joinChain(bus_ctx.getCallerChain());
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
  openbus::OpenBusContext& bus_ctx;
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
    std::cout << "O servico Message nao se encontra no barramento." << std::endl;
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

    for(CORBA::ULong i(0); i != offers->length(); ++i)
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
    boost::shared_ptr<openbus::orb_ctx> 
      orb_ctx(openbus::ORBInitializer(argc, argv));
    CORBA::Object_var o(orb_ctx->orb()->resolve_initial_references("RootPOA"));
    PortableServer::POA_var poa(PortableServer::POA::_narrow(o));
    PortableServer::POAManager_var poa_mgr(poa->the_POAManager());
    poa_mgr->activate();

    boost::optional<openbus::PrivateKey> private_key;
    unsigned short bus_port = 2089;
    std::string bus_host = "localhost";
    {
      namespace po = boost::program_options;
      po::options_description desc("Allowed options");
      desc.add_options()
        ("help", "This help message")
        ("private-key", po::value<std::string>(), "Path to private key")
        ("bus-host", po::value<std::string>(),
         "Host to Openbus (default: localhost)")
        ("bus-port", po::value<unsigned int>(),
         "Host to Openbus (default: 2089)")
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
  boost::thread orb_thread(boost::bind(&run_orb, orb_ctx->orb()));
#endif

  openbus::OpenBusContext *bus_ctx(
    dynamic_cast<openbus::OpenBusContext*>(
      orb_ctx->orb()->resolve_initial_references("OpenBusContext")));
    assert(bus_ctx != 0);
    std::auto_ptr <openbus::Connection> conn(
      bus_ctx->connectByAddress(bus_host, bus_port));
    try
    {
      conn->loginByCertificate("secretary", *private_key);
    }
    catch(tecgraf::openbus::core::v2_1::services::access_control::AccessDenied const&)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    bus_ctx->setDefaultConnection(conn.get());

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "executive";
    props[1].name  = "openbus.component.facet";
    props[1].value = "message";
    offer_registry::ServiceOfferDescSeq_var offers(
      bus_ctx->getOfferRegistry()->findServices(props));
    ::Message_var message_executive = ::get_message(offers);
    if(CORBA::is_nil(message_executive))
    {
      std::cout << "Error" << std::endl;
      return 1;
    }

    scs::core::ComponentId message_componentId = {
      "Message", '1', '0', '0', "" };
    scs::core::ComponentContext message_component
      (bus_ctx->orb(), message_componentId);
    MessageImpl message_servant(message_executive, *bus_ctx);
    message_component.addFacet
      ("message", ::_tc_Message->id(), &message_servant);
    
    offer_registry::ServicePropertySeq properties;
    properties.length(2);
    properties[0].name = "offer.domain";
    properties[0].value = "Demos";
    bus_ctx->getOfferRegistry()->registerService(
      message_component.getIComponent(), properties);

    scs::core::ComponentId meeting_componentId = {
      "Meeting", '1', '0', '0', "" };
    scs::core::ComponentContext meeting_component
      (bus_ctx->orb(), meeting_componentId);
    MeetingImpl meeting_servant;
    meeting_component.addFacet
      ("meeting", ::_tc_Meeting->id(), &meeting_servant);
    
    bus_ctx->getOfferRegistry()->registerService(
      meeting_component.getIComponent(), properties);

#ifdef OPENBUS_SDK_MULTITHREAD
    orb_thread.join();
#else
    orb_ctx->orb()->run();
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

