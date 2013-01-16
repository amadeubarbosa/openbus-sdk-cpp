#include <openbus/assistant.hpp>

#include <iostream>

#include <stubs/independent_clock.h>

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
namespace access_control
 = tecgraf::openbus::core::v2_0::services::access_control;

template <typename F>
bool call_with_found_clock(offer_registry::ServiceOfferDescSeq offers, F f)
{
  try
  {
    if (offers.length() == 0)
    {
      std::cout << "O servico Clock nao se encontra no barramento." << std::endl;
      return false;
    }
    else if(offers.length() == 1)
    {
      CORBA::ULong i = 0;
      demo::Clock_var clock = demo::Clock::_narrow
        (offers[i].service_ref->getFacetByName("clock"));
      if(!CORBA::is_nil(clock))
      {
        f(clock);
        return true;
      }
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
          demo::Clock_var clock = demo::Clock::_narrow(o);
          if(!CORBA::is_nil(clock))
          {
            f(clock);
            return true;
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
  
  return false;
}

struct printTime
{
  printTime() {}

  typedef void result_type;
  result_type operator()(demo::Clock_var clock) const
  {
    std::cout << "Hora no servidor em ticks: " << clock->getTimeInTicks() << std::endl;
  }
};

int main(int argc, char** argv)
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "demo", _password = "demo"
     , _argc = argc, _argv = argv);

  do
  {
    bool s = false;
    try
    {
      offer_registry::ServiceOfferDescSeq offers
        = assistant.findServices(assistant.createFacetAndEntityProperty("clock", "demo"), -1);
      // Pegando uma oferta valida
      if(! ::call_with_found_clock(offers, printTime()))
      {
        std::cout << "Servidor fora do ar, hora atual " << time(0) << std::endl;
      }
      else
        s = true;
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
    catch (CORBA::COMM_FAILURE const&)
    {
      std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
        "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
    }

    if(!s)
    {
      std::cout << "Barramento fora do ar, hora atual " << time(0) << std::endl;
    }

    mysleep();
  }
  while(true);
}
