#include <openbus/OpenBusContext.h>
#include <openbus/ORBInitializer.h>
#include <iostream>
#include <stubs/chain_validation.h>

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
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const& e)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    openbusContext->setDefaultConnection(conn.get());

    // Recebendo oferta de secretaria
    openbus::idl_or::ServicePropertySeq props;
    props.length(3);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "secretary";
    props[1].name  = "openbus.component.facet";
    props[1].value = "message";
    offer_registry::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
    // Pegando uma oferta valida
    ::Message_var message = ::get_message(offers);
    if(!CORBA::is_nil(message))
    {
      // Chama a funcao
      message->sendMessage("Message");
      return 0;
    }

    // Recebendo oferta de executivo
    props[0].name  = "openbus.offer.entity";
    props[0].value = "executive";
    props[1].name  = "openbus.component.facet";
    props[1].value = "message";
    offers = openbusContext->getOfferRegistry()->findServices(props);
    // Pegando uma oferta valida
    message = ::get_message(offers);
    if(!CORBA::is_nil(message))
    {
      try
      {
        // Chama a funcao
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
