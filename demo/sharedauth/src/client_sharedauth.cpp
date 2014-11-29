// -*- coding: iso-8859-1-unix -*-
#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <iostream>
#include <boost/program_options.hpp>

#include <stubs/hello.h>
#include <stubs/sharedauth.h>

#include <fstream>
#include <iterator>

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
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control
 = tecgraf::openbus::core::v2_0::services::access_control;
namespace simple
 = tecgraf::openbus::interop::simple;

template <typename F>
void try_call_with_found_reference(offer_registry::ServiceOfferDescSeq_var offers, F f)
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

struct sayHello
{
  sayHello(bool& try_again)
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
  onReloginCallback(CORBA::OctetSeq secret, access_control::LoginProcess_var attempt)
    : secret(secret), attempt(attempt) {}

  typedef void result_type;
  result_type operator()(openbus::Connection& c, access_control::LoginInfo info) const
  {
    do
    {
      try
      {
        c.loginBySharedAuth(attempt, secret);
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
      mysleep();
    }
    while(true);
  }

  CORBA::OctetSeq secret;
  access_control::LoginProcess_var attempt;
};

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    std::cout << "E necessario passar o nome do arquivo aonde esta gravado o segredo" << std::endl;
    return 1;
  }

  // Inicializando CORBA e ativando o RootPOA
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
  PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
  assert(!CORBA::is_nil(poa));
  PortableServer::POAManager_var poa_manager = poa->the_POAManager();
  poa_manager->activate();

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
    (orb->resolve_initial_references("OpenBusContext"));
  assert(openbusContext != 0);
  std::auto_ptr <openbus::Connection> conn;

  do
  {
    try
    {
      conn = openbusContext->createConnection(bus_host, bus_port);

      CORBA::Object_var object = orb->resolve_initial_references("CodecFactory");
      IOP::CodecFactory_var codec_factory
        = IOP::CodecFactory::_narrow(object);
      assert(!CORBA::is_nil(codec_factory));
  
      IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
      IOP::Codec_var codec = codec_factory->create_codec(cdr_encoding);

      std::ifstream file(argv[1]);
      CORBA::OctetSeq secret;
      file.seekg(0, std::ios::end);
      secret.length(file.tellg());
      file.seekg(0, std::ios::beg);
      file.rdbuf()->sgetn
        (static_cast<char*>(static_cast<void*>(secret.get_buffer()))
         , secret.length());

      CORBA::Any_var any = codec->decode_value(secret, _tc_EncodedSharedAuth);
      const EncodedSharedAuth *sharedauth;
      if(*any >>= sharedauth)
      {
        access_control::LoginProcess_var login
          = access_control::LoginProcess::_narrow(sharedauth->attempt);
        conn->onInvalidLogin( ::onReloginCallback(sharedauth->secret, login));
        conn->loginBySharedAuth(login, sharedauth->secret);
      }
      else
      {
        
      }

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
    mysleep();
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
      ::try_call_with_found_reference(offers, sayHello(try_again));
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
    mysleep();
  }
  while(try_again);

}
