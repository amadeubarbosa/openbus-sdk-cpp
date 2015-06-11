// -*- coding: iso-8859-1-unix -*-

#include "build_fake_legacy_chain.hpp"

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
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  conn_A->loginByPassword("A", "A");

  std::auto_ptr<openbus::Connection> conn_B(
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  conn_B->loginByCertificate(cfg.certificate_user(),
                             openbus::PrivateKey(argv[argc-1]));

  std::auto_ptr<openbus::Connection> conn_C(
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  conn_C->loginByPassword("C", "C");

  openbus::idl_ac::LoginInfoSeq originators;

  openbus::CallerChain fake_legacy_chain(
    build_fake_legacy_chain(
      conn_A->busid(), conn_B->login()->entity.in(), originators,
      *(conn_A->login()), bus_ctx));
  bus_ctx->setCurrentConnection(conn_B.get());
  bus_ctx->joinChain(fake_legacy_chain);

  openbus::CallerChain chain(bus_ctx->makeChainFor(conn_C->login()->id.in()));
  if ("C" != chain.target())
  {
    std::cerr << "'C' != chain.target()" << std::endl;
    std::abort();
  }

  if (conn_B->login()->entity.in() != std::string(chain.caller().entity.in()))
  {
    std::cerr << "'B' != std::string(chain.caller().entity.in())" << std::endl;
    std::abort();
  }

  if (conn_B->login()->id.in() != std::string(chain.caller().id.in()))
  {
    std::cerr << "conn_B->login()->id.in() \
                  != std::string(chain.caller().id.in())"
              << std::endl;
    std::abort();
  }

  if (chain.originators().length() == 0)
  {
    std::cerr << "chain.originators().length() == 0" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
