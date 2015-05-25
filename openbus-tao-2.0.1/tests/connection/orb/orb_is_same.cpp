// -*- coding: iso-8859-1-unix -*-

#include <openbus.h>
#include <configuration.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb (openbus::createORB(argc, argv));
  std::auto_ptr<openbus::Connection> conn(openbus::connect(cfg.host().c_str(), cfg.port(), 
                                                           orb.in()));
  assert(conn->orb() == orb.in());
  return 0; //MSVC
}
