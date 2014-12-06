// -*- coding: iso-8859-1-unix -*-

#include "stubs/hello.h"
#include "stubs/encoding.h"
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>

namespace sharedauth = tecgraf::openbus::interop::sharedauth;

const std::string client_entity("interop_sharedauth_cpp_client");
std::string bus_host;
unsigned short bus_port;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::store(po::parse_config_file<char>("test.properties", desc), vm);
  po::notify(vm);
  if (vm.count("help")) 
  {
    std::cout << desc << std::endl;
    std::exit(1);
  }
  if (vm.count("bus.host.name"))
  {
    bus_host = vm["bus.host.name"].as<std::string>();
  }
  if (vm.count("bus.host.port"))
  {
    bus_port = vm["bus.host.port"].as<unsigned short>();
  }
}

int main(int argc, char** argv) {
  try 
  {
    load_options(argc, argv);

    CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    openbus::OpenBusContext *const ctx = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr <openbus::Connection> conn
      (ctx->createConnection(bus_host, bus_port));

    {
      CORBA::Object_var object = orb->resolve_initial_references("CodecFactory");
      IOP::CodecFactory_var codec_factory
        = IOP::CodecFactory::_narrow(object);
      assert(!CORBA::is_nil(codec_factory));
  
      IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
      IOP::Codec_var codec = codec_factory->create_codec(cdr_encoding);

      CORBA::OctetSeq secret;
      std::ifstream file(".secret");
      file.seekg(0, std::ios::end);
      secret.length(file.tellg());
      file.seekg(0, std::ios::beg);
      file.rdbuf()->sgetn
        (static_cast<char*>(static_cast<void*>(secret.get_buffer()))
         , secret.length());

      CORBA::Any_var any = codec->decode_value(secret,
                                               sharedauth::_tc_EncodedSharedAuth);
      const sharedauth::EncodedSharedAuth *sharedauth;
      if(*any >>= sharedauth)
      {
        openbus::idl_ac::LoginProcess_var login
          = openbus::idl_ac::LoginProcess::_narrow(sharedauth->attempt);
        conn->loginBySharedAuth(login, sharedauth->secret);
      }
      else
      {
        std::cout << "Falhou unmarshaling os dados no arquivo de log" << std::endl;
        return 1;
      }
      ctx->setDefaultConnection(conn.get());
    }

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "openbus.component.facet";
    props[static_cast<CORBA::ULong>(0)].value = "Hello";
    props[static_cast<CORBA::ULong>(1)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(1)].value = "Interoperability Tests";
    openbus::idl_or::ServiceOfferDescSeq_var offers = 
      ctx->getOfferRegistry()->findServices(props);
    
    if (offers->length())
    {
      CORBA::Object_var o = offers[static_cast<CORBA::ULong>(0)]
        .service_ref->getFacetByName("Hello");
      tecgraf::openbus::interop::simple::Hello_var hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      CORBA::String_var ret(hello->sayHello());
      std::string msg("Hello " + client_entity + "!");
      if (!(msg == std::string(ret.in())))
      {
        std::cerr << "sayHello() não retornou a string '"
          + msg + "'." << std::endl;
        std::abort();
      }
    } 
    else 
    {
      std::cout << "nenhuma oferta encontrada." << std::endl;
    }
  } 
  catch(const std::exception &e) 
  {
    std::cout << "[error (std::exception)] " << e.what() << std::endl;
    return -1;
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } 
  catch (...) 
  {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;    
  }
}
