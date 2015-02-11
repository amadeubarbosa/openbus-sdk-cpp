// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
  CORBA::Object_ptr obj(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext* bus_ctx(
    dynamic_cast<openbus::OpenBusContext *>(obj));

  std::auto_ptr<openbus::Connection> conn_A(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn_A->loginByPassword("A", "A");

  std::auto_ptr<openbus::Connection> conn_B(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn_B->loginByPassword("B", "B");

  bus_ctx->setDefaultConnection(conn_A.get());

  openbus::CallerChain chain(bus_ctx->makeChainFor(conn_B->login()->id.in()));
  if ("B" != chain.target())
  {
    std::cerr << "'B' != chain.target()" << std::endl;
    std::abort();
  }

  if ("A" != std::string(chain.caller().entity.in()))
  {
    std::cerr << "'A' != std::string(chain.caller().entity.in())" << std::endl;
    std::abort();
  }

  if (conn_A->login()->id.in() != std::string(chain.caller().id.in()))
  {
    std::cerr << "conn_A->login()->id.in() \
                  != std::string(chain.caller().id.in())"
              << std::endl;
    std::abort();
  }

  if (chain.originators().length() > 0)
  {
    std::cerr << "chain.originators().length() > 0" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
