#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <iostream>
#include <stubs/greetings.h>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control
 = tecgraf::openbus::core::v2_0::services::access_control;

template <typename F>
void try_call_with_found_reference(offer_registry::ServiceOfferDescSeq_var offers, F f)
{
  if (offers->length() == 0)
  {
    std::cout << "O servico Hello nao se encontra no barramento." << std::endl;
    return;
  }
  else if(offers->length() == 1)
  {
    CORBA::ULong i = 0;
    ::Greetings_var greetings = ::Greetings::_narrow
      (offers[i].service_ref->getFacetByName("greetings"));
    if(!CORBA::is_nil(greetings))
    {
      f(greetings);
      return;
    }
  }
  else
  {
    std::cout << "Existe mais de um servico Hello no barramento. Tentaremos encontrar uma funcional." << std::endl;

    for(CORBA::ULong i = 0; i != offers->length(); ++i)
    {
      try
      {
        CORBA::Object_var o = offers[i].service_ref
          ->getFacetByName("greetings");
        ::Greetings_var greetings = ::Greetings::_narrow(o);
        if(!CORBA::is_nil(greetings))
        {
          f(greetings);
          return;
        }
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
      catch (CORBA::COMM_FAILURE const&)
      {
        std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
          "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
      }
    }
  }
}

struct sayGreetings
{
  sayGreetings(bool& try_again, const char* language)
    : try_again(&try_again), language(language) {}

  typedef void result_type;
  result_type operator()(::Greetings_var greetings) const
  {
    std::cout << "for language " << language << " we say: " << greetings->sayGreetings() << std::endl;
    *try_again = false;
  }

  bool* try_again;
  const char* language;
};

struct onReloginCallback
{
  typedef void result_type;
  result_type operator()(openbus::Connection& c, access_control::LoginInfo info) const
  {
    do
    {
      try
      {
        c.loginByPassword("demo", "demo");
        break;
      }
      catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const& e)
      {
        std::cout << "Falha ao tentar realizar o login por senha no barramento: "
          "a entidade j� est� com o login realizado. Esta falha ser� ignorada." << std::endl;
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
      catch (CORBA::COMM_FAILURE const&)
      {
        std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
          "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
      }
      catch (CORBA::OBJECT_NOT_EXIST const&)
      {
        std::cout << "Objeto remoto nao existe mais. Verifique se o sistema se encontra disponivel" << std::endl;
      }
      unsigned int t = 30u;
      do { t = sleep(t); } while(t);
    }
    while(true);
  }
};

int main(int argc, char** argv)
{
  // Inicializando CORBA e ativando o RootPOA
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
  assert(!CORBA::is_nil(poa));
  PortableServer::POAManager_var poa_manager = poa->the_POAManager();
  poa_manager->activate();

  // Construindo e logando conexao
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
    (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
  assert(manager != 0);
  std::auto_ptr <openbus::Connection> conn;

  do
  {
    try
    {
      conn = manager->createConnection("localhost", 2089);
      conn->onInvalidLogin( ::onReloginCallback());
      conn->loginByPassword("demo", "demo");
      manager->setDefaultConnection(conn.get());
      break;
    }
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const& e)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade j� est� com o login realizado. Esta falha ser� ignorada." << std::endl;
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
    catch (CORBA::COMM_FAILURE const&)
    {
      std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
        "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
    }
    catch (CORBA::OBJECT_NOT_EXIST const&)
    {
      std::cout << "Objeto remoto nao existe mais. Verifique se o sistema se encontra disponivel" << std::endl;
    }
    unsigned int t = 30u;
    do { t = sleep(t); } while(t);
  }
  while(true);

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
    
    bool try_again = true;
    do
    {
      try
      {
        offer_registry::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
        // Pegando uma oferta valida
        ::try_call_with_found_reference(offers, sayGreetings(try_again, *language));
        continue;
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
      catch (CORBA::COMM_FAILURE const&)
      {
        std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
          "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
      }
      unsigned int t = 30u;
      do { t = sleep(t); } while(t);
    }
    while(!try_again);
  }
}