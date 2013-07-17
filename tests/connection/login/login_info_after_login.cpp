// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <cstring>
#include <configuration.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = 
    dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::auto_ptr<openbus::Connection> conn(openbusContext->createConnection(cfg.host(), cfg.port()));
  conn->loginByPassword("demo", "demo");

  const tecgraf::openbus::core::v2_0::services::access_control::LoginInfo *login = conn->login();
  assert(login->id != 0 && std::strlen(login->id) != 0);
  assert(login->entity != 0 && std::strcmp(login->entity, "demo") == 0);
}
