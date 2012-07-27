
#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <cstring>

int main(int argc, char* argv[])
{
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenbusConnectionManager");
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>(obj_connection_manager);
  std::auto_ptr<openbus::Connection> conn(manager->createConnection("localhost", 2089));
  conn->loginByPassword("demo", "demo");

  const tecgraf::openbus::core::v2_0::services::access_control::LoginInfo *login = conn->login();
  assert(login->id != 0 && std::strlen(login->id) != 0);
  assert(login->entity != 0 && std::strcmp(login->entity, "demo") == 0);
}
