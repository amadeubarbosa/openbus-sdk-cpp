// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char* argv[])
{
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  if (cfg::openbus_test_verbose)
  {
    openbus::log()->set_level(openbus::debug_level);
  }

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
