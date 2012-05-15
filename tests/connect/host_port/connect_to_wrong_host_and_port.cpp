
#include <openbus.h>
#include <cstdlib>

int main(int argc, char* argv[])
{
  try
  {
    openbus::ConnectionManager* manager = openbus::getConnectionManager
      (openbus::initORB(argc, argv));
    std::auto_ptr<openbus::Connection> conn(manager->createConnection("localhost", 9999 /* Nenhum barramento nesta porta */));
    std::abort();
  }
  catch(CORBA::COMM_FAILURE const&)
  {
  }
  catch(...)
  {
    std::abort();
  }
}
