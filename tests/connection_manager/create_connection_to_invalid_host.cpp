// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <configuration.h>

int main(int argc, char* argv[])
{
  openbus::log().set_level(openbus::debug_level);
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  assert(openbusContext != 0);
  try
  {
    std::auto_ptr<openbus::Connection> conn(
      openbusContext->createConnection("invalid_host", cfg.port()));
  }
  catch(...)
  {
    return 0;
  }
  std::abort();
  return 0; //MSVC
}
