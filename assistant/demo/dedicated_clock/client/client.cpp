#include <openbus/assistant.hpp>
#include <iostream>
#include <stubs/dedicated_clock.h>

#ifdef _WIN32
#include <windows.h>
#endif

void mysleep()
{
#ifndef _WIN32
  unsigned int t = 30u;
  do { t = sleep(t); } while(t);
#else
  Sleep(3000);
#endif
}

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace demo = tecgraf::openbus::demo;
namespace services = tecgraf::openbus::core::v2_0::services;

demo::Clock_ptr get_clock(offer_registry::ServiceOfferDescSeq const& offers)
{
  if (offers.length() == 0)
  {
    std::cout << "O servico Clock nao se encontra no barramento." << std::endl;
    return demo::Clock::_nil();
  }
  else if(offers.length() == 1)
  {
    CORBA::ULong i = 0;
    return demo::Clock::_narrow
      (offers[i].service_ref->getFacetByName("clock"));
  }
  else
  {
    std::cout << "Existe mais de um servico Clock no barramento. Tentaremos encontrar uma funcional." << std::endl;

    for(CORBA::ULong i = 0; i != offers.length(); ++i)
    {
      try
      {
        CORBA::Object_var o = offers[i].service_ref
          ->getFacetByName("clock");
        return demo::Clock::_narrow(o);
      }
      catch(CORBA::TRANSIENT const&) {}
      catch(CORBA::OBJECT_NOT_EXIST const&) {}
    }
    return demo::Clock::_nil();
  }
}

int main(int argc, char** argv)
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "demo", _password = "demo"
     , _argc = argc, _argv = argv);

  do
  {
    try
    {
      offer_registry::ServiceOfferDescSeq
        offers = assistant.findServices
        (assistant.createFacetAndEntityProperty("clock", "demo"), -1);
      // Pegando uma oferta valida
      demo::Clock_ptr clock = ::get_clock(offers);
      if(!CORBA::is_nil(clock))
      {
        // Chama a funcao
        std::cout << "Hora no servidor em ticks: " << clock->getTimeInTicks() << std::endl;
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

    mysleep();
  }
  while(true);
  return 0;
}
