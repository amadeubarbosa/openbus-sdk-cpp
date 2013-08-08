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

const std::string entity("interop_sharedauth_cpp_client");
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

    ctx->setDefaultConnection(conn.get());
    conn->loginByPassword(entity, entity);

    {
      std::pair <openbus::idl_ac::LoginProcess_ptr, openbus::idl::OctetSeq> credential
        = conn->startSharedAuth();
      CORBA::Object_var object = orb->resolve_initial_references("CodecFactory");
      IOP::CodecFactory_var codec_factory
        = IOP::CodecFactory::_narrow(object);
      assert(!CORBA::is_nil(codec_factory));
      
      IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
      IOP::Codec_var codec = codec_factory->create_codec(cdr_encoding);

      sharedauth::EncodedSharedAuth sharedauth
        =
        {
          credential.first, credential.second
        };

      CORBA::Any any;
      any <<= sharedauth;
      CORBA::OctetSeq_var secret_seq = codec->encode_value(any);

      std::ofstream file(".secret");
      std::copy(secret_seq->get_buffer()
                , secret_seq->get_buffer() + secret_seq->length()
                , std::ostream_iterator<char>(file));
    }

    std::cout << "Chamando a faceta Hello por este cliente." << std::endl;

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
      tecgraf::openbus::interop::simple::Hello *hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      hello->sayHello();
    } 
    else 
    {
      std::cout << "nenhuma oferta encontrada." << std::endl;
    }
    std::cout << "orb.run()" << std::endl;
    ctx->orb()->run();
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
