
#include <openbus.h>

int main(int argc, char* argv[])
{
  openbus::ConnectionManager* manager = openbus::getConnectionManager
    (openbus::initORB(argc, argv));
  assert(manager != 0);
  try
  {
    std::auto_ptr<openbus::Connection> conn(manager->createConnection("0.0.0.0", 2089));
  }
  catch(...)
  {
    return 0;
  }
  std::abort();
}
