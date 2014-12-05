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
    
  CORBA::OctetSeq encoded(bus_ctx->encodeChain(chain_A_B_C));
  openbus::CallerChain decoded_chain_A_B_C(bus_ctx->decodeChain(encoded));

  if (chain_A_B_C.busid() != decoded_chain_A_B_C.busid()) std::abort();
  if (chain_A_B_C.target() != decoded_chain_A_B_C.target()) std::abort();
  if (chain_A_B_C.target() != "C") std::abort();
  if (std::string(chain_A_B_C.caller().id.in())
      != std::string(decoded_chain_A_B_C.caller().id.in())) std::abort();
  if (std::string(chain_A_B_C.caller().entity.in())
      != std::string(decoded_chain_A_B_C.caller().entity.in())) std::abort();
  if (std::string(decoded_chain_A_B_C.caller().entity.in()) != "B") std::abort();
  if (chain_A_B_C.originators().length()
      != decoded_chain_A_B_C.originators().length()) std::abort();
  if (decoded_chain_A_B_C.originators().length() != 1) std::abort();
  if (std::string(
        chain_A_B_C.originators()
        [static_cast<CORBA::ULong>(0)].id.in())
      !=
      std::string(
        decoded_chain_A_B_C.originators()
        [static_cast<CORBA::ULong>(0)].id.in())) std::abort();
  if (std::string(
        chain_A_B_C.originators()
        [static_cast<CORBA::ULong>(0)].entity.in())
      !=
      std::string(
        decoded_chain_A_B_C.originators()
        [static_cast<CORBA::ULong>(0)].entity.in())) std::abort();
  return 0; //MSVC 9.0
}
