// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>
#include <cstdlib>

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
  CORBA::Object_ptr obj(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext* bus_ctx(
    dynamic_cast<openbus::OpenBusContext *>(obj));
  std::auto_ptr<openbus::Connection> conn(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  bus_ctx->setDefaultConnection(conn.get());
  conn->loginByPassword(cfg.user(), cfg.password());
  try
  {
    bus_ctx->makeChainFor("");
  }
  catch (const CORBA::NO_PERMISSION &e)
  {
    if (openbus::idl_ac::InvalidTargetCode == e.minor()) return 0;
  }
  std::abort();
  return 0; //MSVC
}
