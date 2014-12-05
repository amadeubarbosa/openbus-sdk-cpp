// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
  openbus::configuration cfg(argc, argv);
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

  if (chain_A_B.busid() != decoded_chain_A_B.busid()) std::abort();
  if (chain_A_B.target() != decoded_chain_A_B.target()) std::abort();
  if (chain_A_B.target() != "B") std::abort();
  if (std::string(chain_A_B.caller().id.in())
      != std::string(decoded_chain_A_B.caller().id.in())) std::abort();
  if (std::string(chain_A_B.caller().entity.in())
      != std::string(decoded_chain_A_B.caller().entity.in())) std::abort();
  if (std::string(decoded_chain_A_B.caller().entity.in()) != "A")  std::abort();
  if (chain_A_B.originators().length()
      != decoded_chain_A_B.originators().length()) std::abort();
  return 0; //MSVC 9.0
}
