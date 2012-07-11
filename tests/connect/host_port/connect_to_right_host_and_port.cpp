
#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>

int main(int argc, char* argv[])
{
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenbusConnectionManager");
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>(obj_connection_manager);
  std::auto_ptr<openbus::Connection> conn(manager->createConnection("localhost", 2089));
}
