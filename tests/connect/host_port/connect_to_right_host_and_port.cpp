// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/log.hpp>
#include <configuration.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = 
    dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::cout << cfg.host() << std::endl;
  std::auto_ptr<openbus::Connection> conn(openbusContext->createConnection(cfg.host(), cfg.port()));
  return 0; //MSVC
}
