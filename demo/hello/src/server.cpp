// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "helloS.h"
#pragma clang diagnostic pop
#include <demo/openssl.hpp>
#include <openbus.hpp>
#include <scs/ComponentContext.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/program_options.hpp>
#pragma clang diagnostic pop


#include <boost/bind.hpp>
#include <fstream>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

namespace offer_registry
 = tecgraf::openbus::core::v2_1::services::offer_registry;
namespace simple = tecgraf::openbus::interop::simple;
namespace services = tecgraf::openbus::core::v2_1::services;
namespace access_control
 = tecgraf::openbus::core::v2_1::services::access_control;

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello
{
  void sayHello()
  {
    std::cout << "Hello" << std::endl;
  }
};

void run_orb(CORBA::ORB_var orb)
{
  orb->run();
}

struct onReloginCallback
{
  typedef void result_type;
  result_type operator()(boost::shared_ptr<openbus::Connection> c,
                         access_control::LoginInfo info
                         , EVP_PKEY * priv_key) const
  {
    do
    {
      try
      {
        c->loginByCertificate("demo", priv_key);
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
  CORBA::Object_var o = orb_ctx->orb()->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
  assert(!CORBA::is_nil(poa));
  PortableServer::POAManager_var poa_manager = poa->the_POAManager();
  poa_manager->activate();

  EVP_PKEY *priv_key(0);
  unsigned short bus_port = 2089;
  std::string bus_host = "localhost";
  {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "This help message")
      ("private-key", po::value<std::string>(), "Path to private key")
      ("bus-host", po::value<std::string>(), "Host to Openbus (default: localhost)")
      ("bus-port", po::value<unsigned int>(), "Host to Openbus (default: 2089)")
      ;

    try
    {
      po::variables_map vm;
      po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
      po::store(parsed, vm);
      po::notify(vm);
    
      if(vm.count("help") || !vm.count("private-key"))
      {
        std::cout << desc << std::endl;
        return 1;
      }
      std::string priv_key_filename = vm["private-key"].as<std::string>();
      priv_key = openbus::demo::openssl::read_priv_key(priv_key_filename);
      if (!priv_key)
      {
        std::cout << "Chave privada invalida." << std::endl;
        return 1;
      }
      
      if(vm.count("bus-host"))
        bus_host = vm["bus-host"].as<std::string>();
      if(vm.count("bus-port"))
        bus_port = vm["bus-port"].as<unsigned int>();
    }
    catch(std::exception const&)
    {
      std::cout << desc << std::endl;
      return 1;
    }
  }

  boost::thread orb_thread(boost::bind(&run_orb, orb_ctx->orb()));

  // Construindo e logando conexao
  openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
    (orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  assert(openbusContext != 0);
  boost::shared_ptr<openbus::Connection> conn;
  do
  {
    try
    {
      conn = openbusContext->connectByAddress(bus_host, bus_port);
      conn->onInvalidLogin( boost::bind(::onReloginCallback(), _1, _2, priv_key) );
      conn->loginByCertificate("demo", priv_key);
      openbusContext->setDefaultConnection(conn);
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

  scs::core::ComponentId componentId;
  componentId.name = "Hello";
  componentId.major_version = '1';
  componentId.minor_version = '0';
  componentId.patch_version = '0';
  componentId.platform_spec = "";
  scs::core::ComponentContext hello_component
    (openbusContext->orb(), componentId);
  HelloImpl hello_servant;
  hello_component.addFacet
    ("hello", simple::_tc_Hello->id(), &hello_servant);
  
  offer_registry::ServicePropertySeq properties;
  properties.length(1);
  properties[0].name = "offer.domain";
  properties[0].value = "Demos";

  try
  {
    do
    {
      try
      {
        openbusContext->getOfferRegistry()->registerService(hello_component.getIComponent(), properties);
        break;
      }
      catch(tecgraf::openbus::core::v2_1::services::access_control::AccessDenied const&)
      {
        std::cout << "Falha ao tentar realizar o login por senha no barramento: "
          "a entidade ja esta com o login realizado. Esta falha sera ignorada." << std::endl;
      }
      catch (services::ServiceFailure const&)
      {
        std::cout << "Falha no servico remoto. Causa: " << std::endl;
      }
      catch (offer_registry::UnauthorizedFacets const&)
      {
        std::cout << "Faceta nao autorizada no barramento: " << std::endl;
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
  
    orb_thread.join();
  }
  catch(offer_registry::InvalidService const&)
  {
    std::cout << "Barramento diz que o servico ofertado nao e um componente valido" << std::endl;
  }
  catch(offer_registry::InvalidProperties const&)
  {
    std::cout << "Barramento diz que o servico ofertado possui ofertas" << std::endl;
  }
}

