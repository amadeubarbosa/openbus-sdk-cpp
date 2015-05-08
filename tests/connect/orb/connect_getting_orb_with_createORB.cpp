// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <configuration.h>

int main(int argc, char *argv[])
{
  openbus::configuration cfg(argc, argv);
  openbus::orb_ctx orb_ctx(openbus::ORBInitializer(argc, argv));
  if (CORBA::is_nil(orb_ctx.orb()))
  {
    std::cerr << "CORBA::is_nil(orb_ctx.orb())) == true" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
