// -*- coding: iso-8859-1-unix -*-

#include <openbus.h>
#include <cstdlib>
#include <configuration.h>

int main(int argc, char **argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(openbus::initORB(argc, argv));
  CORBA::Object_ptr o(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(o));
  std::auto_ptr<openbus::Connection> conn(bus_ctx->createConnect(cfg.host(), 
                                                                 cfg.port()));
  conn->loginByPassword("demo", "demo");
  conn->close();
  if(conn->logout())
  {
    std::abort();
  }
}
