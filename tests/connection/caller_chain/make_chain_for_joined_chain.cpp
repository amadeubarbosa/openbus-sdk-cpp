// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv)
{
  openbus::log().set_level(openbus::debug_level);
  openbus::configuration cfg(argc, argv);
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

  std::auto_ptr<openbus::Connection> conn_A(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn_A->loginByPassword("A", "A");

  std::auto_ptr<openbus::Connection> conn_B(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn_B->loginByPassword("B", "B");

  std::auto_ptr<openbus::Connection> conn_C(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn_C->loginByPassword("C", "C");

  bus_ctx->setDefaultConnection(conn_A.get());

  openbus::CallerChain chain_A_B(
    bus_ctx->makeChainFor(conn_B->login()->id.in()));
  bus_ctx->setCurrentConnection(conn_B.get());
  bus_ctx->joinChain(chain_A_B);

  openbus::CallerChain chain_A_B_C(
    bus_ctx->makeChainFor(conn_C->login()->id.in()));

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
