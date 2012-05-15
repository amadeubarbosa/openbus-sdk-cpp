
#include <openbus.h>
#include <cstdlib>

int main(int argc, char* argv[])
{
  std::auto_ptr<openbus::Connection> conn(openbus::connect("localhost", 2089
                                                           , openbus::createORB(argc, argv)));
  conn->loginByPassword("demo", "demo");
  conn->close();
  if(conn->logout())
    std::abort();
}
