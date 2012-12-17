#include <openbus/ORBInitializer.h>
#include <openbus/log.h>
#include <openbus/OpenBusContext.h>
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

    {
      CORBA::Object_var object = orb->resolve_initial_references("CodecFactory");
      IOP::CodecFactory_var codec_factory
        = IOP::CodecFactory::_narrow(object);
      assert(!CORBA::is_nil(codec_factory));
  
      IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
      IOP::Codec_var codec = codec_factory->create_codec(cdr_encoding);

      std::ifstream file(properties_file.openbus_log_file.c_str());
      CORBA::OctetSeq secret;
      file.seekg(0, std::ios::end);
      secret.length(file.tellg());
      file.seekg(0, std::ios::beg);
      file.rdbuf()->sgetn
        (static_cast<char*>(static_cast<void*>(secret.get_buffer()))
         , secret.length());

      CORBA::Any_var any = codec->decode_value(secret, sharedauth::_tc_EncodedSharedAuth);
      sharedauth::EncodedSharedAuth sharedauth;
      if((*any) >>= sharedauth)
      {
        access_control::LoginProcess_var login
          = access_control::LoginProcess::_narrow(sharedauth.attempt);
        conn->loginBySharedAuth(login, sharedauth.secret);
      }
      else
      {
        std::cout << "Falhou unmarshaling os dados no arquivo de log" << std::endl;
        return 1;
      }
      openbusContext->setDefaultConnection(conn.get());
    }

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[0].name  = "openbus.component.facet";
    props[0].value = "Hello";
    props[1].name  = "offer.domain";
    props[1].value = "Interoperability Tests";
    openbus::idl_or::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(props);
    if (offers->length())
    {
      CORBA::ULong zero = 0u;
      CORBA::Object_var o = offers[zero].service_ref->getFacetByName(
        "Hello");
      tecgraf::openbus::interop::simple::Hello *hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      hello->sayHello();
    } else std::cout << "nenhuma oferta encontrada." << std::endl;
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
