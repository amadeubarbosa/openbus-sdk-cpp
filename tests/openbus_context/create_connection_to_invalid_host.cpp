// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <configuration.h>

int main(int argc, char **argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
  CORBA::Object_ptr o(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *ctx(dynamic_cast<openbus::OpenBusContext *>(o));
  assert(ctx != 0);
  try
  {
    std::auto_ptr<openbus::Connection> conn(
      ctx->createConnection("invalid_host", cfg.port()));
  }
  catch (const CORBA::Exception &)
  {
    return 0;
  }
  std::abort();
}
