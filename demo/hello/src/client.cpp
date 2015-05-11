// -*- coding: iso-8859-1-unix -*-

#include "helloC.h"

#include <openbus/OpenBusContext.hpp>
#include <iostream>

#include <boost/program_options.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace simple = tecgraf::openbus::interop::simple;
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control
 = tecgraf::openbus::core::v2_0::services::access_control;

template <typename F>
void call_with_found_hello(offer_registry::ServiceOfferDescSeq_var offers, F f)
{
  if (offers->length() == 0)
  {
    std::cout << "O servico Hello nao se encontra no barramento." << std::endl;
    return;
  }
  else if(offers->length() == 1)
  {
    CORBA::ULong i = 0;
    simple::Hello_var hello = simple::Hello::_narrow
      (offers[i].service_ref->getFacetByName("hello"));
    if(!CORBA::is_nil(hello))
    {
      f(hello);
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
          ->getFacetByName("hello");
        simple::Hello_var hello = simple::Hello::_narrow(o);
        if(!CORBA::is_nil(hello))
        {
          f(hello);
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

struct callSayHello
{
  callSayHello(bool& try_again)
    : try_again(&try_again) {}

  typedef void result_type;
  result_type operator()(simple::Hello_var hello) const
  {
    hello->sayHello();
    *try_again = false;
  }

  bool* try_again;
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
      catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const&)
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
#ifndef _WIN32
      unsigned int t = 30u;
      do { t = sleep(t); } while(t);
#else
      Sleep(3000);
#endif
    }
    while(true);
  }
};

int main(int argc, char** argv)
{
  // Inicializando CORBA e ativando o RootPOA
  openbus::orb_ctx orb_ctx(openbus::ORBInitializer(argc, argv));

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
    (orb_ctx.orb()->resolve_initial_references("OpenBusContext"));
  assert(openbusContext != 0);
  std::auto_ptr <openbus::Connection> conn;

  do
  {
    try
    {
      conn = openbusContext->createConnection(bus_host, bus_port);
      conn->onInvalidLogin( ::onReloginCallback());
      conn->loginByPassword("demo", "demo");
      openbusContext->setDefaultConnection(conn.get());
      break;
    }
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const&)
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
#ifndef _WIN32
      unsigned int t = 30u;
      do { t = sleep(t); } while(t);
#else
      Sleep(3000);
#endif
  }
  while(true);

  // Recebendo ofertas
  openbus::idl_or::ServicePropertySeq props;
  props.length(2);
  props[0].name  = "openbus.offer.entity";
  props[0].value = "demo";
  props[1].name  = "openbus.component.facet";
  props[1].value = "hello";
  
  bool try_again = true;
  do
  {
    try
    {
      offer_registry::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
      // Pegando uma oferta valida
      ::call_with_found_hello(offers, callSayHello(try_again));
      continue;
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
#ifndef _WIN32
    unsigned int t = 30u;
    do { t = sleep(t); } while(t);
#else
    Sleep(3000);
#endif
  }
  while(try_again);
}
