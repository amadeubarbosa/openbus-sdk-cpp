// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext =
    dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::auto_ptr<openbus::Connection> conn1(openbusContext->createConnection(cfg.host(), cfg.port()));
  conn1->loginByPassword(cfg.user().c_str(), cfg.password().c_str());
  std::auto_ptr<openbus::Connection> conn2(openbusContext->createConnection(cfg.host(), cfg.port()));
  std::string conn2_entity = "encode_and_decode_chain_test";
  conn2->loginByPassword(conn2_entity, conn2_entity);
  openbusContext->setDefaultConnection(conn1.get());
  openbus::CallerChain originalChain = openbusContext->makeChainFor(conn2->login()->id.in());
  if (originalChain == openbus::CallerChain())
  {
    std::cout << "WARNING: Unable to get caller chain." << std::endl;
  }
  CORBA::OctetSeq encodedChain = openbusContext->encodeChain(originalChain);
  openbus::CallerChain decodedChain = openbusContext->decodeChain(encodedChain);
  if (originalChain != decodedChain)
  {
    std::cout << "The original chain is different from the decoded chain." << std::endl;
    conn2->logout();
    std::abort();
  }
  conn2->logout();
}
