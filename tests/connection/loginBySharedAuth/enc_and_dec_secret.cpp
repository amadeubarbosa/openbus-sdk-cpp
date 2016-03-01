// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>

// Last argument is assumed to be the path for the private key file argv[argc-1]
int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);
  
  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  EVP_PKEY *priv_key(openbus::demo::openssl::read_priv_key(cfg::system_private_key));
  if (!priv_key)
  {
    std::cerr << "Chave privada invalida." << std::endl;
    std::abort();
  }
  conn->loginByCertificate(cfg::system_entity_name, priv_key);

  CORBA::OctetSeq shared_auth_seq(
    bus_ctx->encodeSharedAuthSecret(conn->startSharedAuth()));

  openbus::SharedAuthSecret shared_auth(
    bus_ctx->decodeSharedAuthSecret(shared_auth_seq));
  
  {
    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
    conn->loginBySharedAuth(shared_auth);
    if (conn->login() == 0)
    {
      std::cerr << "conn->login() == 0" << std::endl;
      std::abort();
    }
  }
  return 0; //MSVC
}
