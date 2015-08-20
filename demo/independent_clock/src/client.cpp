// -*- coding: iso-8859-1-unix -*-

#include "independent_clockC.h"

#include <openbus.hpp>

#include <boost/program_options.hpp>
#include <iostream>
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
 = tecgraf::openbus::core::v2_1::services::offer_registry;
namespace demo = tecgraf::openbus::demo;
namespace services = tecgraf::openbus::core::v2_1::services;
namespace access_control
 = tecgraf::openbus::core::v2_1::services::access_control;

template <typename F>
bool call_with_found_clock(offer_registry::ServiceOfferDescSeq_var offers, F f)
{
  if (offers->length() == 0)
  {
    std::cout << "O servico Clock nao se encontra no barramento." << std::endl;
    return false;
  }
  else if(offers->length() == 1)
  {
    CORBA::ULong i = 0;
    demo::Clock_var hello = demo::Clock::_narrow
      (offers[i].service_ref->getFacetByName("clock"));
    if(!CORBA::is_nil(hello))
      f(hello);
  }
  else
  {
    std::cout << "Existe mais de um servico Clock no barramento. Tentaremos encontrar uma funcional." << std::endl;

    for(CORBA::ULong i = 0; i != offers->length(); ++i)
    {
      try
      {
        CORBA::Object_var o = offers[i].service_ref
          ->getFacetByName("clock");
        demo::Clock_var clock = demo::Clock::_narrow(o);
        if(!CORBA::is_nil(clock))
        {
          f(clock);
          return false;
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
    return false;
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
      catch(tecgraf::openbus::core::v2_1::services::access_control::AccessDenied const&)
      {
        std::cout << "Falha ao tentar realizar o login por senha no barramento: "
          "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
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
      catch (CORBA::COMM_FAILURE const&)
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
  }
};

int main(int argc, char** argv)
{
  boost::shared_ptr<openbus::orb_ctx> 
    orb_ctx(openbus::ORBInitializer(argc, argv));

  unsigned short bus_port = 2089;
  std::string bus_host = "localhost";
  {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "This help message")
      ("bus-host", po::value<std::string>(), "Host to Openbus (default: localhost)")
      ("bus-port", po::value<unsigned short>(), "Host to Openbus (default: 2089)")
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

  // Construindo e logando conexao
  openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
    (orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  assert(openbusContext != 0);
  std::auto_ptr <openbus::Connection> conn;

  do
  {
    try
    {
      conn = openbusContext->connectByAddress(bus_host, bus_port);
      conn->onInvalidLogin( ::onReloginCallback());
      conn->loginByPassword("demo", "demo");
      openbusContext->setDefaultConnection(conn.get());
      break;
    }
    catch(tecgraf::openbus::core::v2_1::services::access_control::AccessDenied const&)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
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
    catch (CORBA::COMM_FAILURE const&)
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

  // Recebendo ofertas
  openbus::idl::offers::ServicePropertySeq props;
  props.length(2);
  props[0].name  = "openbus.offer.entity";
  props[0].value = "demo";
  props[1].name  = "openbus.component.facet";
  props[1].value = "clock";
  
  do
  {
    try
    {
      offer_registry::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
      // Pegando uma oferta valida
      if(! ::call_with_found_clock(offers, printTime()))
      {
        std::cout << "Servidor fora do ar, hora atual " << time(0) << std::endl;
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
    catch (CORBA::COMM_FAILURE const&)
    {
      std::cout << "Erro de comunicacao. Verifique se o sistema se encontra "
        "ainda disponivel ou se sua conexao com o mesmo foi interrompida" << std::endl;
    }
    mysleep();
  }
  while(true);
}
