// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/log.hpp>
#include <configuration.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  openbus::orb_ctx orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var obj(orb_ctx.orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  std::auto_ptr<openbus::Connection> conn(bus_ctx->createConnection(cfg.host(), cfg.port()));
  return 0; //MSVC
}
