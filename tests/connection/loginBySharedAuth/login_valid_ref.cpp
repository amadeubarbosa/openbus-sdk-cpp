// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>

// Last argument is assumed to be the path for the private key file argv[argc-1]
int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;
  assert(argc >= 2);
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  if (cfg::openbus_test_verbose)
  {
    openbus::log()->set_level(openbus::debug_level);
  }
  
  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  cfg::register_relogin_callback(conn);
  EVP_PKEY *priv_key(openbus::demo::openssl::read_priv_key(cfg::system_private_key));
  if (!priv_key)
  {
    std::cerr << "Chave privada invalida." << std::endl;
    std::abort();
  }
  conn->loginByCertificate(cfg::system_entity_name, priv_key);

  openbus::SharedAuthSecret shared_auth(conn->startSharedAuth());
  {
    std::stringstream corbaloc;
    corbaloc << "corbaloc::" << cfg::bus_host_name << ":" << cfg::bus_host_port
             << "/" << tecgraf::openbus::core::v2_1::BusObjectKey;
    CORBA::Object_var
      ref(orb_ctx->orb()->string_to_object(corbaloc.str().c_str()));
    boost::shared_ptr<openbus::Connection>
      conn(bus_ctx->connectByReference(ref));
    conn->loginBySharedAuth(shared_auth);
    if (conn->login() == 0)
    {
      std::cerr << "conn->login() == 0" << std::endl;
      std::abort();
    }
  }
  return 0; //MSVC
}
