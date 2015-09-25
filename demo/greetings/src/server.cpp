// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "greetingsS.h"
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
namespace services = tecgraf::openbus::core::v2_1::services;
namespace access_control
 = tecgraf::openbus::core::v2_1::services::access_control;

struct GreetingsImpl : virtual public ::POA_Greetings
{
  GreetingsImpl(std::string const& greeting)
    : greeting(greeting) {}
  char* sayGreetings()
  {
    return CORBA::string_dup(greeting.c_str());
  }
  std::string greeting;
};

void run_orb(CORBA::ORB_var orb)
{
  orb->run();
}

struct onReloginCallback
{
  typedef void result_type;
  result_type operator()(openbus::Connection& c, access_control::LoginInfo info
                         , EVP_PKEY * priv_key) const
  {
    do
    {
      try
      {
        c.loginByCertificate("demo", priv_key);
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

bool registerService(scs::core::IComponent_var component, offer_registry::ServicePropertySeq const& properties
                     , offer_registry::OfferRegistry_var offers)
{
  try
  {
    do
    {
      try
      {
        offers->registerService(component, properties);
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
    return true;
  }
  catch(offer_registry::InvalidService const&)
  {
    std::cout << "Barramento diz que o servico ofertado nao e um componente valido" << std::endl;
  }
  catch(offer_registry::InvalidProperties const&)
  {
    std::cout << "Barramento diz que o servico ofertado possui ofertas" << std::endl;
  }
  return false;
}

int main(int argc, char** argv)
{
  // Inicializando CORBA e ativando o RootPOA
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
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if(vm.count("help") || !vm.count("private-key"))
    {
      std::cout << desc << std::endl;
      return 0;
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
      conn->onInvalidLogin( boost::bind(::onReloginCallback(), _1, _2, priv_key));
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

  scs::core::ComponentId componentId = { "Greetings", '1', '0', '0', "" };
  scs::core::ComponentContext english_greetings_component(orb_ctx->orb(), componentId);
  GreetingsImpl english_greetings_servant("Hello");
  english_greetings_component.addFacet
    ("greetings", _tc_Greetings->id(), &english_greetings_servant);

  scs::core::ComponentContext portuguese_greetings_component(orb_ctx->orb(), componentId);
  GreetingsImpl portuguese_greetings_servant("Ola");
  portuguese_greetings_component.addFacet
    ("greetings", _tc_Greetings->id(), &portuguese_greetings_servant);

  scs::core::ComponentContext german_greetings_component(orb_ctx->orb(), componentId);
  GreetingsImpl german_greetings_servant("Guten Tag");
  german_greetings_component.addFacet
    ("greetings", _tc_Greetings->id(), &german_greetings_servant);
    
  offer_registry::ServicePropertySeq properties;
  properties.length(2);
  properties[0].name = "offer.domain";
  properties[0].value = "Demos";
  properties[1].name = "language";
  properties[1].value = "english";
  if(!registerService(english_greetings_component.getIComponent(), properties
                      , openbusContext->getOfferRegistry()))
    return -1;

  properties[1].name = "language";
  properties[1].value = "portuguese";
  if(!registerService(portuguese_greetings_component.getIComponent(), properties
                      , openbusContext->getOfferRegistry()))
    return -1;

  properties[1].name = "language";
  properties[1].value = "german";
  if(!registerService(german_greetings_component.getIComponent(), properties
                      , openbusContext->getOfferRegistry()))
    return -1;

  orb_thread.join();
}

