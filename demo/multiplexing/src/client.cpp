// -*- coding: iso-8859-1-unix -*-
#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <iostream>
#include <greetingsC.h>
#include <tao/PortableServer/PortableServer.h>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_0::services;

::Greetings_ptr get_greetings(offer_registry::ServiceOfferDescSeq_var offers)
{
  if (offers->length() == 0)
  {
    std::cout << "O servico Greetings nao se encontra no barramento." << std::endl;
    return ::Greetings::_nil();
  }
  else if(offers->length() == 1)
  {
    CORBA::ULong i = 0;
    return ::Greetings::_narrow
      (offers[i].service_ref->getFacetByName("greetings"));
  }
  else
  {
    std::cout << "Existe mais de um servico Greetings no barramento. Tentaremos encontrar uma funcional." << std::endl;

    for(CORBA::ULong i = 0; i != offers->length(); ++i)
    {
      try
      {
        CORBA::Object_var o = offers[i].service_ref
          ->getFacetByName("greetings");
        return ::Greetings::_narrow(o);
      }
      catch(CORBA::TRANSIENT const&) {}
      catch(CORBA::OBJECT_NOT_EXIST const&) {}
    }
    return ::Greetings::_nil();
  }
}

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

    // Construindo e logando conexao
    openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    assert(openbusContext != 0);
    std::auto_ptr <openbus::Connection> conn (openbusContext->createConnection("localhost", 2089));
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

    // Recebendo ofertas
    openbus::idl_or::ServicePropertySeq props;
    props.length(3);
    props[0].name  = "openbus.component.facet";
    props[0].value = "greetings";

    const char* entities[] = { "demo1", "demo2", "demo3" };
    const char* languages[] = { "english", "portuguese", "german" };
    for(std::size_t i = 0; i != 3; ++i)
    {
      props[1].name  = "openbus.offer.entity";
      props[1].value = entities[i];
      props[2].name  = "language";
      props[2].value = languages[i];
      offer_registry::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
      // Pegando uma oferta valida
      ::Greetings_ptr greetings = ::get_greetings(offers);
      if(!CORBA::is_nil(greetings))
      {
        // Chama a funcao
        std::cout << "for language " << languages[i] << " we say: " << greetings->sayGreetings() << std::endl;
      }
    }
    return 0;
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
