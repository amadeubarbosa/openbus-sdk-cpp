// -*- coding: iso-8859-1-unix -*-

#include <openbus.h>
#include <cstdlib>
#include <configuration.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::initORB(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = 
    dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::auto_ptr<openbus::Connection> conn(openbusContext->createConnect(cfg.host().c_str(), 
                                                                        cfg.port()));
  conn->loginByPassword("demo", "demo");
  conn->close();
  if(conn->logout())
    std::abort();
  return 0; //MSVC
}
