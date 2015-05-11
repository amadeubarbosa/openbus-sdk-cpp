// -*- coding: iso-8859-1-unix -*-

#include "chain_validationC.h"
#include <openbus/OpenBusContext.hpp>
#include <boost/program_options.hpp>
#include <iostream>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_0::services;

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

int main(int argc, char** argv)
{
  try
  {
    openbus::orb_ctx orb_ctx(openbus::ORBInitializer(argc, argv));

    unsigned short bus_port(2089);
    std::string bus_host("localhost");
    {
      namespace po = boost::program_options;
      po::options_description desc("Allowed options");
      desc.add_options()
        ("help", "This help message")
        ("bus-host", po::value<std::string>(),
         "Host to Openbus (default: localhost)")
        ("bus-port", po::value<unsigned short>(),
         "Host to Openbus (default: 2089)")
        ;
      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);
      
      if(vm.count("help"))
      {
        std::cout << desc << std::endl;
        return 0;
      }
   
      if(vm.count("bus-host"))
        bus_host = vm["bus-host"].as<std::string>();
      if(vm.count("bus-port"))
        bus_port = vm["bus-port"].as<unsigned short>();
    }

    openbus::OpenBusContext *openbusContext(
      dynamic_cast<openbus::OpenBusContext*>
      (orb_ctx.orb()->resolve_initial_references("OpenBusContext")));
    assert(openbusContext != 0);
    std::auto_ptr <openbus::Connection> conn(
      openbusContext->createConnection(bus_host, bus_port));
    try
    {
      conn->loginByPassword("demo", "demo");
    }
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const&)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    openbusContext->setDefaultConnection(conn.get());

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "secretary";
    props[1].name  = "openbus.component.facet";
    props[1].value = "message";
    offer_registry::ServiceOfferDescSeq_var offers(
      openbusContext->getOfferRegistry()->findServices(props));
    ::Message_var message = ::get_message(offers);
    if(!CORBA::is_nil(message))
    {
      message->sendMessage("Message");
      return 0;
    }

    props[0].name  = "openbus.offer.entity";
    props[0].value = "executive";
    props[1].name  = "openbus.component.facet";
    props[1].value = "message";
    offers = openbusContext->getOfferRegistry()->findServices(props);
    message = ::get_message(offers);
    if(!CORBA::is_nil(message))
    {
      try
      {
        message->sendMessage("Message");
        std::cout << "Chamada nao deveria ter sido aceita" << std::endl;
      }
      catch(Unavailable const&)
      {
        std::cout << "Chamada foi recusada como esperado" << std::endl;
      }
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
  return 1;
}
