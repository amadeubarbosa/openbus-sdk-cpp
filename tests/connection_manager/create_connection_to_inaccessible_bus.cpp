
#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <configuration.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenbusConnectionManager");
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>(obj_connection_manager);
  assert(manager != 0);
  try
  {
    std::auto_ptr<openbus::Connection> conn(manager->createConnection(cfg.host().c_str(), 2090));
  }
  catch(...)
  {
    return 0;
  }
  std::abort();
}
