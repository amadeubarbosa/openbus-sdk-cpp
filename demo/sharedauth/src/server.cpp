#include <openbus/OpenBusContext.h>
#include <openbus/ORBInitializer.h>
#include <scs/ComponentContext.h>
#include <iostream>

#include <stubs/hello.h>
#include <CORBA.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

#include <boost/program_options.hpp>
#include <fstream>
#include <boost/bind.hpp>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace simple = tecgraf::openbus::interop::simple;
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control
 = tecgraf::openbus::core::v2_0::services::access_control;

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello
{
  void sayHello()
  {
    std::cout << "Hello" << std::endl;
  }
};

#ifdef OPENBUS_SDK_MULTITHREAD
void run_orb(CORBA::ORB_var orb)
{
  orb->run();
}
#endif

struct onReloginCallback
{
  typedef void result_type;
  result_type operator()(openbus::Connection& c, access_control::LoginInfo info
                         , CORBA::OctetSeq private_key) const
  {
    do
    {
      try
      {
        c.loginByCertificate("demo", private_key);
        break;
      }
      catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const& e)
      {
        std::cout << "Falha ao tentar realizar o login por senha no barramento: "
          "a entidade j� est� com o login realizado. Esta falha ser� ignorada." << std::endl;
      }
      catch (services::ServiceFailure e)
      {
        std::cout << "Falha no servi�o remoto. Causa: " << std::endl;
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
      unsigned int t = 30u;
      do { t = sleep(t); } while(t);
    }
    while(true);
  }
};

int main(int argc, char** argv)
{
  // Inicializando CORBA e ativando o RootPOA
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
  assert(!CORBA::is_nil(poa));
  PortableServer::POAManager_var poa_manager = poa->the_POAManager();
  poa_manager->activate();

  CORBA::OctetSeq private_key;
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
  std::auto_ptr <openbus::Connection> conn;
  do
  {
    try
    {
      conn = openbusContext->createConnection("localhost", 2089);
      conn->onInvalidLogin( boost::bind(::onReloginCallback(), _1, _2, private_key) );
      conn->loginByCertificate("demo", private_key);
      openbusContext->setDefaultConnection(conn.get());
      break;
    }
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const& e)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade j� est� com o login realizado. Esta falha ser� ignorada." << std::endl;
    }
    catch (services::ServiceFailure e)
    {
      std::cout << "Falha no servi�o remoto. Causa: " << std::endl;
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
    unsigned int t = 30u;
    do { t = sleep(t); } while(t);
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
      catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const& e)
      {
        std::cout << "Falha ao tentar realizar o login por senha no barramento: "
          "a entidade j� est� com o login realizado. Esta falha ser� ignorada." << std::endl;
      }
      catch (services::ServiceFailure const& e)
      {
        std::cout << "Falha no servico remoto. Causa: " << std::endl;
      }
      catch (offer_registry::UnauthorizedFacets const& e)
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
      unsigned int t = 30u;
      do { t = sleep(t); } while(t);
    }
    while(true);
  
#ifdef OPENBUS_SDK_MULTITHREAD
    orb_thread.join();
#else
    orb->run();
#endif
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
