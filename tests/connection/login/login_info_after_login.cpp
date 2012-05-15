
#include <openbus.h>
#include <cstring>

int main(int argc, char* argv[])
{
  std::auto_ptr<openbus::Connection> conn(openbus::connect("localhost", 2089
                                                           , openbus::createORB(argc, argv)));
  conn->loginByPassword("demo", "demo");

  tecgraf::openbus::core::v2_00::services::access_control::LoginInfo const* login = conn->login();
  assert(login->id != 0 && std::strlen(login->id) != 0);
  assert(login->entity != 0 && std::strcmp(login->entity, "demo") == 0);
}
