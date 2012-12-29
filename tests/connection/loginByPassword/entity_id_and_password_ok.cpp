#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::auto_ptr <openbus::Connection> conn (openbusContext->createConnection(cfg.host(), cfg.port()));
  conn->loginByPassword(cfg.user().c_str(), cfg.password().c_str());
}
