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
namespace access_control = tecgraf::openbus::core::v2_0::services::access_control;

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello
{
  HelloImpl(simple::Hello_var hello
            , openbus::OpenBusContext& bus_ctx)
    : hello(hello), bus_ctx(bus_ctx) {}

  void sayHello()
  {
    std::cout << "Hello called on proxy" << std::endl;
    openbus::CallerChain chain(bus_ctx.getCallerChain());
    bus_ctx.joinChain(chain);
    if(chain != openbus::CallerChain())
    {
      std::cout << "Caller: " << chain.caller().entity << std::endl;
      access_control::LoginInfoSeq originators = chain.originators();
      for(CORBA::ULong i(0); i != originators.length(); ++i)
      {
        std::cout << "Originator: " << originators[i].entity << std::endl;
      }
    }
    hello->sayHello();
  }

  simple::Hello_var hello;
  openbus::OpenBusContext& bus_ctx;
};

simple::Hello_ptr get_hello(offer_registry::ServiceOfferDescSeq_var offers)
{
  if (offers->length() == 0)
  {
    std::cout << "O servico Hello nao se encontra no barramento." << std::endl;
    return simple::Hello::_nil();
  }
  else if(offers->length() == 1)
  {
    CORBA::ULong i = 0;
    return simple::Hello::_narrow
      (offers[i].service_ref->getFacetByName("hello"));
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
        return simple::Hello::_narrow(o);
      }
      catch(CORBA::TRANSIENT const&) {}
      catch(CORBA::OBJECT_NOT_EXIST const&) {}
      catch(CORBA::COMM_FAILURE const&) {}
    }
    return simple::Hello::_nil();
  }
}

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
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));

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
  boost::thread orb_thread(boost::bind(&run_orb, orb_ctx->orb()));
#endif

  openbus::OpenBusContext *bus_ctx(
    dynamic_cast<openbus::OpenBusContext*>(
      orb_ctx->orb()->resolve_initial_references("OpenBusContext")));
    assert(bus_ctx != 0);
    std::auto_ptr <openbus::Connection> conn(
      bus_ctx->createConnection(bus_host, bus_port));
    try
    {
      conn->loginByCertificate("proxy", *private_key);
    }
    catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const&)
    {
      std::cout << "Falha ao tentar realizar o login por senha no barramento: "
        "a entidade já está com o login realizado. Esta falha será ignorada." << std::endl;
      return 1;
    }
    bus_ctx->setDefaultConnection(conn.get());

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[0].name  = "openbus.offer.entity";
    props[0].value = "server";
    props[1].name  = "openbus.component.facet";
    props[1].value = "hello";
    offer_registry::ServiceOfferDescSeq_var offers(
      bus_ctx->getOfferRegistry()->findServices(props));
    simple::Hello_ptr hello = ::get_hello(offers);
    if(!CORBA::is_nil(hello))
    {
      scs::core::ComponentId componentId;
      componentId.name = "Hello";
      componentId.major_version = '1';
      componentId.minor_version = '0';
      componentId.patch_version = '0';
      componentId.platform_spec = "";
      scs::core::ComponentContext hello_component
        (bus_ctx->orb(), componentId);
      HelloImpl hello_servant(hello, *bus_ctx);
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
      orb_ctx->orb()->run();
#endif
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
  return 1;
}
