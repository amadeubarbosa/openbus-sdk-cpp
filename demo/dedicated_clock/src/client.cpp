// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <dedicated_clockC.h>
#pragma clang diagnostic pop
#include <openbus.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/program_options.hpp>
#pragma clang diagnostic pop
#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>

namespace offer_registry
 = tecgraf::openbus::core::v2_1::services::offer_registry;
namespace demo = tecgraf::openbus::demo;
namespace services = tecgraf::openbus::core::v2_1::services;
namespace access_control
 = tecgraf::openbus::core::v2_1::services::access_control;

demo::Clock_ptr get_clock(offer_registry::ServiceOfferDescSeq_var offers)
{
  if (offers->length() == 0)
  {
    std::cout << "O servico Clock nao se encontra no barramento." << std::endl;
    return demo::Clock::_nil();
  }
  else if(offers->length() == 1)
  {
    CORBA::ULong i = 0;
    return demo::Clock::_narrow
      (offers[i].service_ref->getFacetByName("clock"));
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
        return demo::Clock::_narrow(o);
      }
      catch(CORBA::TRANSIENT const&) {}
      catch(CORBA::OBJECT_NOT_EXIST const&) {}
    }
    return demo::Clock::_nil();
  }
}

struct onReloginCallback
{
  typedef void result_type;
  result_type operator()(boost::shared_ptr<openbus::Connection> c,
                         access_control::LoginInfo info) const
  {
    do
    {
      try
      {
        c->loginByPassword("demo", "demo");
        break;
      }
      catch(tecgraf::openbus::core::v2_1::services::access_control::AccessDenied const&)
      {
        std::cout << "Falha ao tentar realizar o login por senha no barramento: "
          "a entidade ja esta com o login realizado. Esta falha sera ignorada." << std::endl;
      }
      catch (services::ServiceFailure e)
      {
        std::cout << "Falha no servico remoto. Causa: " << std::endl;
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
  std::auto_ptr<openbus::orb_ctx> 
    orb_ctx(openbus::ORBInitializer(argc, argv));

  unsigned short bus_port = 2089;
  std::string bus_host = "localhost";
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

  openbus::OpenBusContext *bus_ctx(
    dynamic_cast<openbus::OpenBusContext*>
    (orb_ctx->orb()->resolve_initial_references("OpenBusContext")));
  assert(bus_ctx != 0);
  boost::shared_ptr<openbus::Connection> conn;

  do
  {
    try
    {
      conn = bus_ctx->connectByAddress(bus_host, bus_port);
      conn->onInvalidLogin( ::onReloginCallback());
      conn->loginByPassword("demo", "demo");
      bus_ctx->setDefaultConnection(conn);
      break;
    }
    catch(tecgraf::openbus::core::v2_1::services::access_control::AccessDenied const&)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade ja esta com o login realizado. Esta falha sera ignorada." << std::endl;
    }
    catch (services::ServiceFailure e)
    {
      std::cout << "Falha no servico remoto. Causa: " << std::endl;
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

  openbus::idl::offers::ServicePropertySeq props;
  props.length(2);
  props[0].name  = "openbus.offer.entity";
  props[0].value = "demo";
  props[1].name  = "openbus.component.facet";
  props[1].value = "clock";

  do
  {
    offer_registry::ServiceOfferDescSeq_var offers = bus_ctx->getOfferRegistry()->findServices(props);
    demo::Clock_ptr clock = ::get_clock(offers);
    if(!CORBA::is_nil(clock))
    {
      std::cout << "Hora no servidor em ticks: " << clock->getTimeInTicks() << std::endl;
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
