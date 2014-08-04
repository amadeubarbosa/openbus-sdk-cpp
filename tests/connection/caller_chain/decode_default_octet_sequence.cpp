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
  openbus::CallerChain chain = openbusContext->decodeChain(CORBA::OctetSeq(0));
  if (chain != openbus::CallerChain())
  {
    std::abort();
  }
}