// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <configuration.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
  CORBA::Object_ptr o(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(o));
  std::auto_ptr<openbus::Connection> conn1(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn1->loginByPassword(cfg.user(), cfg.password());
  std::auto_ptr<openbus::Connection> conn2(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  std::string conn2_entity("encode_and_decode_chain_test");
  conn2->loginByPassword(conn2_entity, conn2_entity);
  bus_ctx->setDefaultConnection(conn1.get());
  openbus::CallerChain originalChain(bus_ctx->makeChainFor(conn2->login()->id.in()));
  if (originalChain == openbus::CallerChain())
  {
    std::cout << "WARNING: Unable to get caller chain." << std::endl;
  }
  CORBA::OctetSeq encodedChain(bus_ctx->encodeChain(originalChain));
  openbus::CallerChain decodedChain(bus_ctx->decodeChain(encodedChain));
  if (originalChain != decodedChain)
  {
    std::cout << "The original chain is different from the decoded chain." << std::endl;
    conn2->logout();
    std::abort();
  }
  conn2->logout();
}
