// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <cstdlib>

#include "configuration.h"

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(CORBA::ORB_init(argc, argv, ""));
  try
  {
    std::auto_ptr<openbus::Connection> conn(openbus::connect(cfg.host(), cfg.port(), orb));
    std::abort();
  }
  catch(openbus::InvalidORB const&)
  {
  }
  catch(...)
  {
    std::abort();
  }
}
