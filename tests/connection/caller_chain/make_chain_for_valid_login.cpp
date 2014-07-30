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
  std::string conn2_entity = "make_chain_for_valid_login_and_use_test_1";
  conn2->loginByPassword(conn2_entity, conn2_entity);
  std::auto_ptr<openbus::Connection> conn3(openbusContext->createConnection(cfg.host(), cfg.port()));
  openbusContext->setDefaultConnection(conn1.get());
  openbus::CallerChain chain = openbusContext->makeChainFor(conn2->login()->id.in());
  assert(chain.target() == conn2_entity);
  assert(std::string(chain.caller().id.in()) == std::string(conn1->login()->id.in()));
}
