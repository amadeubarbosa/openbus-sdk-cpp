
#include <openbus.h>

int main(int argc, char* argv[])
{
  openbus::ConnectionManager* manager = openbus::getConnectionManager
    (openbus::initORB(argc, argv));
  assert(manager != 0);
  std::auto_ptr<openbus::Connection> conn(manager->createConnection("localhost", 2089));
}
