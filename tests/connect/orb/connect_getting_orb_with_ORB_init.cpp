
#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <cstdlib>

#include "configuration.h"

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_ptr orb = CORBA::ORB_init(argc, argv, "");
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