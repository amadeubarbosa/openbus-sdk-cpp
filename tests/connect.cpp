
#include <openbus.h>

int main(int argc, char* argv[])
{
  std::auto_ptr<openbus::Connection> conn(openbus::connect("localhost", 2089));
}
