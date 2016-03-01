// -*- coding: iso-8859-1-unix -*-

#include <check.hpp>
#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);

  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  conn->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);

  bus_ctx->setDefaultConnection(conn);

  std::string token("invalid_token");
  CORBA::OctetSeq token_seq(
    token.size(), token.size(), (CORBA::Octet*)token.c_str());

  try
  {
    openbus::CallerChain imported(bus_ctx->importChain(token_seq, cfg::user_password_domain));
    std::abort();
  }
  catch (const openbus::idl::access::InvalidToken&)
  {
  }
  return 0; //MSVC
}
