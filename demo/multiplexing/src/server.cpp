// -*- coding: iso-8859-1-unix -*-
#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <scs/ComponentContext.h>
#include <iostream>

#include <stubs/greetings.h>
#include <CORBA.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/bind.hpp>
#include <fstream>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_0::services;

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

#ifdef OPENBUS_SDK_MULTITHREAD
void run_orb(CORBA::ORB_var orb)
{
  orb->run();
}
#endif

struct call_dispatcher
{
  openbus::Connection* c;

  call_dispatcher(openbus::Connection* c)
    : c(c) {}

  typedef openbus::Connection* result_type;
  result_type operator()(openbus::OpenBusContext& context, const std::string bus_id
                         , const std::string login_id, const std::string operation) const
  {
    return c;
  }
};

int main(int argc, char** argv)
{
  try
  {
    // Inicializando CORBA e ativando o RootPOA
    CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    boost::optional<openbus::PrivateKey> private_key;
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

    // Construindo e logando conexao
    openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    assert(openbusContext != 0);
    std::auto_ptr <openbus::Connection> conn1 (openbusContext->createConnection(bus_host, bus_port));
    std::auto_ptr <openbus::Connection> conn2 (openbusContext->createConnection(bus_host, bus_port));
    std::auto_ptr <openbus::Connection> conn3 (openbusContext->createConnection(bus_host, bus_port));
    try
    {
      conn1->loginByCertificate("demo1", *private_key);
      conn2->loginByCertificate("demo2", *private_key);
      conn3->loginByCertificate("demo3", *private_key);
    }
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const& e)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    openbusContext->onCallDispatch(call_dispatcher(conn2.get()));

    scs::core::ComponentId componentId = { "Greetings", '1', '0', '0', "" };
    scs::core::ComponentContext english_greetings_component(orb, componentId);
    GreetingsImpl english_greetings_servant("Hello");
    english_greetings_component.addFacet
      ("greetings", _tc_Greetings->id(), &english_greetings_servant);

    scs::core::ComponentContext portuguese_greetings_component(orb, componentId);
    GreetingsImpl portuguese_greetings_servant("Olá");
    portuguese_greetings_component.addFacet
      ("greetings", _tc_Greetings->id(), &portuguese_greetings_servant);

    scs::core::ComponentContext german_greetings_component(orb, componentId);
    GreetingsImpl german_greetings_servant("Guten Tag");
    german_greetings_component.addFacet
      ("greetings", _tc_Greetings->id(), &german_greetings_servant);
    
    openbusContext->setCurrentConnection(conn1.get());
    offer_registry::ServicePropertySeq properties;
    properties.length(2);
    properties[0].name = "offer.domain";
    properties[0].value = "Demos";
    properties[1].name = "language";
    properties[1].value = "english";
    openbusContext->getOfferRegistry()->registerService(english_greetings_component.getIComponent(), properties);

    openbusContext->setCurrentConnection(conn2.get());
    properties[1].name = "language";
    properties[1].value = "portuguese";
    openbusContext->getOfferRegistry()->registerService(portuguese_greetings_component.getIComponent(), properties);

    openbusContext->setCurrentConnection(conn3.get());
    properties[1].name = "language";
    properties[1].value = "german";
    openbusContext->getOfferRegistry()->registerService(german_greetings_component.getIComponent(), properties);

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

