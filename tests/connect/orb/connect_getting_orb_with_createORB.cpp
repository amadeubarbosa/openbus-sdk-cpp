// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <configuration.h>

int main(int argc, char *argv[])
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
  CORBA::Object_ptr obj(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj));
  std::auto_ptr<openbus::Connection> conn(bus_ctx->createConnection(cfg.host(), cfg.port()));
  return 0; //MSVC
}
