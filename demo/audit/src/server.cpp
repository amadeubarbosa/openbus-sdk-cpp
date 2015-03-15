// -*- coding: iso-8859-1-unix -*-

#include "helloS.h"
#include <openbus/OpenBusContext.hpp>
#include <scs/ComponentContext.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace simple = tecgraf::openbus::interop::simple;
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control =
  tecgraf::openbus::core::v2_0::services::access_control;

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello
{
  HelloImpl(openbus::OpenBusContext& bus_ctx)
    : bus_ctx(bus_ctx) {}

  void sayHello()
  {
    openbus::CallerChain chain(bus_ctx.getCallerChain());
    if(chain != openbus::CallerChain())
    {
      std::cout << "Caller: " << chain.caller().entity << std::endl;
      access_control::LoginInfoSeq originators = chain.originators();
      for(CORBA::ULong i(0); i != originators.length(); ++i)
      {
        std::cout << "Originator: " << originators[i].entity << std::endl;
      }
    }
    std::cout << "Hello" << std::endl;
  }

  openbus::OpenBusContext& bus_ctx;
};

#ifdef OPENBUS_SDK_MULTITHREAD
void run_orb(CORBA::ORB_var orb)
{
  orb->run();
}
#endif

int main(int argc, char** argv)
{
  try
  {
    CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
    CORBA::Object_var o(orb->resolve_initial_references("RootPOA"));
    PortableServer::POA_var poa(PortableServer::POA::_narrow(o));
    POA_Manager_var poa_mgr(poa->the_POAManager());
    poa_mgr->activate();

    boost::optional<openbus::PrivateKey> private_key;
    unsigned short bus_port(2089);
    std::string bus_host("localhost");
    {
      namespace po = boost::program_options;
      po::options_description desc("Allowed options");
      desc.add_options()
        ("help", "This help message")
        ("private-key", po::value<std::string>(), "Path to private key")
        ("bus-host", po::value<std::string>(),
         "Host to Openbus (default: localhost)")
        ("bus-port", po::value<unsigned int>(),
         "Host to Openbus (default: 2089)")
        ;
      po::variables_map vm;
      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);
      
      if(vm.count("help") || !vm.count("private-key"))
      {
        std::cout << desc << std::endl;
        return 0;
      }
      std::string private_key_filename = vm["private-key"].as<std::string>();
      private_key = openbus::PrivateKey(private_key_filename);

      if(vm.count("bus-host"))
        bus_host = vm["bus-host"].as<std::string>();
      if(vm.count("bus-port"))
        bus_port = vm["bus-port"].as<unsigned int>();
    }

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&run_orb, orb));
#endif

  openbus::OpenBusContext *bus_ctx(
    dynamic_cast<openbus::OpenBusContext*>(
      orb->resolve_initial_references("OpenBusContext")));
    assert(bus_ctx != 0);
    std::auto_ptr <openbus::Connection> conn(
      bus_ctx->createConnection(bus_host, bus_port));
    try
    {
      conn->loginByCertificate("server", *private_key);
    }
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const&)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    bus_ctx->setDefaultConnection(conn.get());

    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    scs::core::ComponentContext hello_component
      (bus_ctx->orb(), componentId);
    HelloImpl hello_servant(*bus_ctx);
    hello_component.addFacet
      ("hello", simple::_tc_Hello->id(), &hello_servant);
    
    offer_registry::ServicePropertySeq properties;
    properties.length(1);
    properties[0].name = "offer.domain";
    properties[0].value = "Demos";
    bus_ctx->getOfferRegistry()->registerService(
      hello_component.getIComponent(), properties);

#ifdef OPENBUS_SDK_MULTITHREAD
    orb_thread.join();
#else
    orb->run();
#endif
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
