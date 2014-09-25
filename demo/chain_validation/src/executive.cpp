// -*- coding: iso-8859-1-unix -*-
#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <scs/ComponentContext.h>
#include <iostream>

#include <chain_validationS.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

#include <boost/program_options.hpp>
#include <fstream>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_0::services;

struct MessageImpl : public POA_Message
{
  MessageImpl(openbus::OpenBusContext& openbusContext)
    : openbusContext(openbusContext) {}

  void sendMessage(const char* message)
  {
    openbus::CallerChain chain = openbusContext.getCallerChain();
    if(chain != openbus::CallerChain()
       && !std::strcmp(chain.caller().entity, "secretary"))
    {
      std::cout << "Hello" << std::endl;
    }
    else
    {
      const char* name = "[unknown]";
      if(chain != openbus::CallerChain())
        name = chain.caller().entity;
      std::cout << "I'm unavailable to " << name << std::endl;
      throw ::Unavailable();
    }
  }

  openbus::OpenBusContext& openbusContext;
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
    // Inicializando CORBA e ativando o RootPOA
    CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    openbus::idl::OctetSeq private_key;
    {
      namespace po = boost::program_options;
      po::options_description desc("Allowed options");
      desc.add_options()
        ("help", "This help message")
        ("private-key", po::value<std::string>(), "Path to private key")
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
      std::ifstream f(private_key_filename.c_str());
      f.seekg(0, std::ios::end);
      std::size_t size = f.tellg();
      f.seekg(0, std::ios::beg);
      private_key.length(size);
      f.rdbuf()->sgetn(static_cast<char*>(static_cast<void*>(private_key.get_buffer())), size);
    }

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&run_orb, orb));
#endif

    // Construindo e logando conexao
    openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    assert(openbusContext != 0);
    std::auto_ptr <openbus::Connection> conn (openbusContext->createConnection("localhost", 2089));
    try
    {
      conn->loginByCertificate("executive", private_key);
    }
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const& e)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    openbusContext->setDefaultConnection(conn.get());

    scs::core::ComponentId componentId = { "Message", '1', '0', '0', "" };
    scs::core::ComponentContext message_component
      (openbusContext->orb(), componentId);
    MessageImpl message_servant(*openbusContext);
    message_component.addFacet
      ("message", ::_tc_Message->id(), &message_servant);
    
    offer_registry::ServicePropertySeq properties;
    properties.length(2);
    properties[0].name = "offer.domain";
    properties[0].value = "Demos";
    openbusContext->getOfferRegistry()->registerService(message_component.getIComponent(), properties);

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

