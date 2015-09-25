// -*- coding: iso-8859-1-unix -*-

#include <configuration.h>
#include <demo/openssl.hpp>
#include <openbus.hpp>

// Last argument is assumed to be the path for the private key file argv[argc-1]
int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  assert(argc >= 2); // Check that there's at least one argument passed
  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  
  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  EVP_PKEY *priv_key(openbus::demo::openssl::read_priv_key(argv[argc-1]));
  if (!priv_key)
  {
    std::cerr << "Chave privada invalida." << std::endl;
    std::abort();
  }
  conn->loginByCertificate(cfg.certificate_user(), priv_key);

  CORBA::OctetSeq shared_auth_seq(
    bus_ctx->encodeSharedAuthSecret(conn->startSharedAuth()));

  openbus::SharedAuthSecret shared_auth(
    bus_ctx->decodeSharedAuthSecret(shared_auth_seq));
  
  {
    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(cfg.host(), cfg.port()));
    conn->loginBySharedAuth(shared_auth);
    if (conn->login() == 0)
    {
      std::cerr << "conn->login() == 0" << std::endl;
      std::abort();
    }
  }
  return 0; //MSVC
}
