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
  
  boost::shared_ptr<openbus::Connection> conn_C(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  conn_C->loginByPassword("C", "C", cfg::user_password_domain);

  bus_ctx->setDefaultConnection(conn_A);
  
  openbus::CallerChain chain_A_B(
    bus_ctx->makeChainFor(conn_B->login()->entity.in()));
  
  bus_ctx->setCurrentConnection(conn_B);
  bus_ctx->joinChain(chain_A_B);
  openbus::CallerChain chain_A_B_C(
    bus_ctx->makeChainFor(conn_C->login()->entity.in()));
    
  CORBA::OctetSeq encoded(bus_ctx->encodeChain(chain_A_B_C));
  openbus::CallerChain decoded_chain_A_B_C(bus_ctx->decodeChain(encoded));

  openbus::tests::is_equal<std::string>(
    chain_A_B_C.busid(), decoded_chain_A_B_C.busid(),
    "chain_A_B_C.busid()", "decoded_chain_A_B_C.busid()");
  
  openbus::tests::is_equal<std::string>(
    chain_A_B_C.target(), decoded_chain_A_B_C.target(),
    "chain_A_B_C.target()", "decoded_chain_A_B_C.target()");

  openbus::tests::is_equal<std::string>(
    chain_A_B_C.target(), "C", "chain_A_B_C.target()", "C");

  openbus::tests::is_equal<std::string>(
    std::string(chain_A_B_C.caller().id.in()),
    std::string(decoded_chain_A_B_C.caller().id.in()),
    "chain_A_B_C.caller().id", "decoded_chain_A_B_C.caller().id");

  openbus::tests::is_equal<std::string>(
    std::string(chain_A_B_C.caller().entity.in()),
    std::string(decoded_chain_A_B_C.caller().entity.in()),
    "chain_A_B_C.caller().entity", "decoded_chain_A_B_C.caller().entity");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_chain_A_B_C.caller().entity.in()), "B",
    "decoded_chain_A_B_C.caller().entity", "B");

  openbus::tests::is_equal<std::size_t>(
    chain_A_B_C.originators().length(),
    decoded_chain_A_B_C.originators().length(),
    "chain_A_B_C.originators().length()",
    "decoded_chain_A_B_C.originators().length()");

  openbus::tests::is_equal<std::size_t>(
    chain_A_B_C.originators().length(), 1,
    "chain_A_B_C.originators().length()", "1");

  openbus::tests::is_equal<std::string>(
    std::string(chain_A_B_C.originators()[0u].id.in()),
    std::string(decoded_chain_A_B_C.originators()[0u].id.in()),
    "chain_A_B_C.originators()[0u].id",
    "decoded_chain_A_B_C.originators()[0u].id");

  openbus::tests::is_equal<std::string>(
    std::string(chain_A_B_C.originators()[0u].entity.in()),
    std::string(decoded_chain_A_B_C.originators()[0u].entity.in()),
    "chain_A_B_C.originators()[0u].entity",
    "decoded_chain_A_B_C.originators()[0u].entity");;
  return 0; //MSVC
}
