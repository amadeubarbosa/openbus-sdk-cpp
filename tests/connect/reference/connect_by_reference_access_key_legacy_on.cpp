// -*- coding: iso-8859-1-unix -*-

#include <configuration.h>
#include <demo/openssl.hpp>
#include <openbus_core-2.1C.h>
#include <openbus.hpp>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

  std::stringstream corbaloc;
  corbaloc << "corbaloc::" << cfg.host() << ":" << cfg.port()
           << "/" << tecgraf::openbus::core::v2_1::BusObjectKey;
  CORBA::Object_var
    ref(orb_ctx->orb()->string_to_object(corbaloc.str().c_str()));
  
  EVP_PKEY *priv_key(openbus::demo::openssl::read_priv_key(argv[argc-1]));
  if (!priv_key)
  {
    std::cerr << "Chave privada inv�lida." << std::endl;
    std::abort();
  }

  std::auto_ptr<openbus::Connection>
    conn(bus_ctx->connectByReference(ref,
                                     openbus::_access_key=priv_key,
                                     openbus::_legacy_support=true));
  return 0; //MSVC
}