#include <openbus.h>
#include <iostream>
#include "greetings.h"

namespace offer_registry
 = tecgraf::openbus::core::v2_00::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_00::services;

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
    CORBA::ORB_var orb = openbus::initORB(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    // Construindo e logando conexao
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    assert(manager != 0);
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    try
    {
      conn->loginByPassword("demo", "demo");
    }
    catch(openbus::AccessDenied const& e)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    manager->setDefaultConnection(conn.get());

    // Recebendo ofertas
    openbus::idl_or::ServicePropertySeq props;
    props.length(3);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "demo";
    props[1].name  = "openbus.component.facet";
    props[1].value = "greetings";

    const char* languages[] = { "english", "portuguese", "german" };
    for(const char** language = languages
          ; language != &languages[3]; ++language)
    {
      props[2].name  = "language";
      props[2].value = *language;
      offer_registry::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
      // Pegando uma oferta valida
      ::Greetings_ptr greetings = ::get_greetings(offers);
      if(!CORBA::is_nil(greetings))
      {
        // Chama a funcao
        std::cout << "for language " << *language << " we say: " << greetings->sayGreetings() << std::endl;
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
