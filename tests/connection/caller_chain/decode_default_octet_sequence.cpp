// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <configuration.h>
#include <cstdlib>

int main(int argc, char **argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
  CORBA::Object_ptr o(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(dynamic_cast<openbus::OpenBusContext*>(o));
  openbus::CallerChain chain(bus_ctx->decodeChain(CORBA::OctetSeq(0)));
  if (chain != openbus::CallerChain())
  {
    std::abort();
  }
}
