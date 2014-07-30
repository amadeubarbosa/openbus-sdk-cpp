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
  CORBA::OctetSeq wrongCtxId;
  wrongCtxId.length(8); // Endianness (1 byte) + padding (3) + CORBA::ULong (4).
  for (CORBA::ULong i = 0; i < wrongCtxId.length(); i++)
  {
	wrongCtxId[i] = 0;
  }
  openbus::CallerChain chain = openbusContext->decodeChain(CORBA::OctetSeq(0));
  if (chain != openbus::CallerChain())
  {
    std::abort();
  }
}
