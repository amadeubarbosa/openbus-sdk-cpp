// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>

int main(int argc, char* argv[])
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);  

  EVP_PKEY *priv_key(
    openbus::demo::openssl::read_priv_key(cfg::system_private_key));
  if (!priv_key)
  {
    std::cerr << "Chave privada invalida." << std::endl;
    std::abort();
  }

  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port,
                                   openbus::_access_key=priv_key,
                                   openbus::_legacy_support=true));
  return 0; //MSVC
}
