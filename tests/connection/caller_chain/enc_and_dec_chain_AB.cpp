// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <configuration.h>
#include <check.hpp>
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
  openbus::configuration cfg(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
  CORBA::Object_ptr obj(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(
    dynamic_cast<openbus::OpenBusContext *>(obj));
  
  std::auto_ptr<openbus::Connection> conn_A(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn_A->loginByPassword("A", "A");
  
  std::auto_ptr<openbus::Connection> conn_B(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn_B->loginByPassword("B", "B");
  
  bus_ctx->setDefaultConnection(conn_A.get());
  
  openbus::CallerChain chain_A_B(
    bus_ctx->makeChainFor(conn_B->login()->id.in()));

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