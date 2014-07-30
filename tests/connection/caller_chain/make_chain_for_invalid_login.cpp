// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>
#include <cstdlib>

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
	  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext =
    dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::auto_ptr<openbus::Connection> conn(openbusContext->createConnection(cfg.host(), cfg.port()));
  openbusContext->setDefaultConnection(conn.get());
  conn->loginByPassword(cfg.user().c_str(), cfg.password().c_str());
  try
  {
    openbusContext->makeChainFor("");
  }
  catch (const CORBA::NO_PERMISSION &e)
  {
    if (e.minor() == openbus::idl_ac::InvalidTargetCode)
    {
	std::exit(EXIT_SUCCESS);
    }
  }
  std::abort();
}
