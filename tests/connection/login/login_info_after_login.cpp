// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <cstring>
#include <configuration.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  openbus::orb_ctx orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var obj(orb_ctx.orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  std::auto_ptr<openbus::Connection> conn(bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn->loginByPassword("demo", "demo");

  const tecgraf::openbus::core::v2_0::services::access_control::LoginInfo *login = conn->login();
  assert(login->id != 0 && std::strlen(login->id) != 0);
  assert(login->entity != 0 && std::strcmp(login->entity, "demo") == 0);
  return 0; //MSVC
}
