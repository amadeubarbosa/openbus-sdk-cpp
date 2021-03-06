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

  if ("C" != chain_A_B_C.target())
  {
    std::cerr << "'C' != chain_A_B_C.target()" << std::endl;
    std::abort();
  }

  if ("B" != std::string(chain_A_B_C.caller().entity.in()))
  {
    std::cerr << "'B' != std::string(chain_A_B_C.caller().entity.in())"
              << std::endl;
    std::abort();
  }

  if (conn_B->login()->id.in() != std::string(chain_A_B_C.caller().id.in()))
  {
    std::cerr << "conn_B->login()->id.in() \
                  != std::string(chain_A_B_C.caller().id.in())"
              << std::endl;
    std::abort();
  }

  if (chain_A_B_C.originators().length() != 1)
  {
    std::cerr << "chain_A_B_C.originators().length() != 1" << std::endl;
    std::abort();
  }

  if ("A" != std::string(chain_A_B_C.originators()[0u].entity.in()))
  {
    std::cerr << "'A' != std::string( \
                  chain_A_B_C.originators()[0u].entity.in())"
              << std::endl;
    std::abort();
  }

  if (conn_A->login()->id.in()
      != std::string(chain_A_B_C.originators()[0u].id.in()))
  {
    std::cerr << "conn_B->login()->id.in() \
                  != std::string(chain_A_B_C.originators()[0u].id.in())"
              << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
