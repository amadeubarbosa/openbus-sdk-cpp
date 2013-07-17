// -*- coding: iso-8859-1-unix -*-
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>
#include <iostream>
#include "stubs/hello.h"
#include "stubs/encoding.h"
#include "properties_reader.h"

namespace access_control
 = tecgraf::openbus::core::v2_0::services::access_control;
namespace sharedauth = tecgraf::openbus::interop::sharedauth;

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::debug_level);

    ::properties properties_file;
    if(properties_file.openbus_log_file.empty())
    {
      throw std::runtime_error("Nenhum arquivo de log foi especificado na configuracao");
    }

    if(properties_file.buses.size() < 1)
      throw std::runtime_error("No bus is configured");

    ::properties::bus bus = properties_file.buses[0];

    CORBA::ORB *orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    openbus::OpenBusContext *openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr <openbus::Connection> conn
      (openbusContext->createConnection(bus.host, bus.port));

    openbusContext->setDefaultConnection(conn.get());
    conn->loginByPassword("interop_sharedauth_cpp_client"
                          , "interop_sharedauth_cpp_client");

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

      std::ofstream file(properties_file.openbus_log_file.c_str());
      std::copy(secret_seq->get_buffer()
                , secret_seq->get_buffer() + secret_seq->length()
                , std::ostream_iterator<char>(file));
    }

    std::cout << "Chamando a faceta Hello por este cliente." << std::endl;

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[0].name  = "openbus.component.facet";
    props[0].value = "Hello";
    props[1].name  = "offer.domain";
    props[1].value = "Interoperability Tests";

    openbus::idl_or::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
    if (offers->length())
    {
      CORBA::Object_var o = offers[static_cast<CORBA::ULong> (0)].service_ref->getFacetByName(
        "Hello");
      tecgraf::openbus::interop::simple::Hello *hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      hello->sayHello();
    } else std::cout << "nenhuma oferta encontrada." << std::endl;
    std::cout << "orb.run()" << std::endl;
    openbusContext->orb()->run();
  } catch(std::exception const& e) {
    std::cout << "[error (std::exception)] " << e.what() << std::endl;
  } catch (const CORBA::Exception &e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;    
  }
  return 0;
}
