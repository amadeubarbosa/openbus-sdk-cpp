// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>

// Last argument is assumed to be the path for the private key file argv[argc-1]
int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  assert(argc >= 2);
  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  
  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  EVP_PKEY *priv_key(openbus::demo::openssl::read_priv_key(cfg::system_private_key));
  if (!priv_key)
  {
    std::cerr << "Chave privada invalida." << std::endl;
    std::abort();
  }
  conn->loginByCertificate(cfg::system_entity_name, priv_key);

  openbus::SharedAuthSecret shared_auth(conn->startSharedAuth());
  {
    boost::shared_ptr<openbus::Connection>
      conn(bus_ctx->connectByAddress("$invalid_host$", cfg::bus_host_port));
    try
    {
      conn->loginBySharedAuth(shared_auth);
      std::cout << "No exception was thrown, exception CORBA::TRANSIENT was expected"
                << std::endl;
      std::abort();
      if (conn->login() == 0)
      {
        std::cerr << "conn->login() == 0" << std::endl;
        std::abort();
      }
    }
    catch (const CORBA::TRANSIENT &)
    {
      std::cout << "CORBA::TRANSIENT was thrown." << std::endl;
    }      
  }
  return 0; //MSVC
}
