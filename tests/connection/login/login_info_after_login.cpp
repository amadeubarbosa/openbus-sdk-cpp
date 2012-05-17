
#include <openbus.h>
#include <cstring>

int main(int argc, char* argv[])
{
  openbus::ConnectionManager* manager = openbus::getConnectionManager
    (openbus::initORB(argc, argv));
  std::auto_ptr<openbus::Connection> conn(manager->createConnection("localhost", 2089));
  conn->loginByPassword("demo", "demo");

  tecgraf::openbus::core::v2_00::services::access_control::LoginInfo const* login = conn->login();
  assert(login->id != 0 && std::strlen(login->id) != 0);
  assert(login->entity != 0 && std::strcmp(login->entity, "demo") == 0);
}
