// -*- coding: iso-8859-1-unix -*-

#include <check.hpp>
#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char **argv)
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);
  
  boost::shared_ptr<openbus::Connection> conn_A(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  conn_A->loginByPassword("A", "A", cfg::user_password_domain);
  
  boost::shared_ptr<openbus::Connection> conn_B(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  conn_B->loginByPassword("B", "B", cfg::user_password_domain);
  
  bus_ctx->setDefaultConnection(conn_A);
  
  openbus::CallerChain chain_A_B(
    bus_ctx->makeChainFor(conn_B->login()->entity.in()));

  CORBA::OctetSeq encoded(bus_ctx->encodeChain(chain_A_B));

  openbus::CallerChain decoded_chain_A_B(bus_ctx->decodeChain(encoded));

  openbus::tests::is_equal<std::string>(
    chain_A_B.busid(), decoded_chain_A_B.busid(),
    "chain_A_B.busid()", "decoded_chain_A_B.busid()");

  openbus::tests::is_equal<std::string>(
    chain_A_B.target(), decoded_chain_A_B.target(),
    "chain_A_B.target()", "decoded_chain_A_B.target()");

  openbus::tests::is_equal<std::string>(
    chain_A_B.target(), "B",
    "chain_A_B.target()", "B");

  openbus::tests::is_equal<std::string>(
    std::string(chain_A_B.caller().id.in()),
    std::string(decoded_chain_A_B.caller().id.in()),
    "chain_A_B.caller().id", "decoded_chain_A_B.caller().id");

  openbus::tests::is_equal<std::string>(
    std::string(chain_A_B.caller().entity.in()),
    std::string(decoded_chain_A_B.caller().entity.in()),
    "chain_A_B.caller().entity", "decoded_chain_A_B.caller().entity()");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_chain_A_B.caller().entity.in()),
    "A",
    "chain_A_B.caller().entity", "A");

  openbus::tests::is_equal<std::size_t>(
    chain_A_B.originators().length(),
    decoded_chain_A_B.originators().length(),
    "chain_A_B.originators().length()",
    "decoded_chain_A_B.originators().length()");
  
  return 0; //MSVC
}
