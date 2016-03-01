// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char* argv[])
{
  CORBA::ORB_var orb(CORBA::ORB_init(argc, argv, ""));

  try
  {
    orb->resolve_initial_references("OpenBusContext");
    std::abort();
  }
  catch(const CORBA::ORB::InvalidName &)
  {
  }
  return 0; //MSVC
}
