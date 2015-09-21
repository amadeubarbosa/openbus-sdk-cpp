// -*- coding: iso-8859-1-unix -*-

#include <configuration.h>
#include <demo/openssl.hpp>
#include <openbus.hpp>

// Last argument is assumed to be the path for the private key file argv[argc-1]
int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  assert(argc >= 2);
  boost::shared_ptr<openbus::orb_ctx>
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

  openbus::SharedAuthSecret shared_auth(conn->startSharedAuth());
  {
    boost::shared_ptr<openbus::Connection>
      conn (bus_ctx->connectByAddress(cfg.host(), cfg.wrong_port()));
    try
    {
      conn->loginBySharedAuth(shared_auth);
      std::cout << "No exception was thrown, exception CORBA::SystemException was expected"
                << std::endl;
      std::abort();
      if (conn->login() == 0)
      {
        std::cerr << "conn->login() == 0" << std::endl;
        std::abort();
      }
    }
    catch (const CORBA::SystemException &)
    {
      std::cout << "CORBA::SystemException was thrown." << std::endl;
    }      
  }
  return 0; //MSVC
}
