// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/log.hpp>
#include <configuration.h>
#include <cstdlib>

int main(int argc, char** argv)
{
  openbus::log().set_level(openbus::debug_level);
  openbus::configuration cfg(argc, argv);
  openbus::orb_ctx orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var obj(orb_ctx.orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

  std::auto_ptr<openbus::Connection> conn(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  bus_ctx->setDefaultConnection(conn.get());
  conn->loginByPassword(cfg.user(), cfg.password());
  try
  {
    bus_ctx->makeChainFor("");
  }
  catch (const openbus::idl_ac::InvalidLogins &e)
  {
    if (std::string(e.loginIds[0]) == "")
    {
      return 0;
    }
  }
  std::abort();
  return 0; //MSVC
}
