
#include <openbus.h>
#include <cstdlib>

int main(int argc, char* argv[])
{
  try
  {
    openbus::ConnectionManager* manager = openbus::getConnectionManager
      (openbus::initORB(argc, argv));
    std::auto_ptr<openbus::Connection> conn(manager->createConnection("localhost", 9999 /* Nenhum barramento nesta porta */));
    std::cout << "No exception was thrown, exception COMM_FAILURE was expected" << std::endl;
    std::abort();
  }
  catch(CORBA::COMM_FAILURE const&)
  {
  }
  catch(...)
  {
    std::cout << "Unknown exception was thrown" << std::endl;
    std::abort();
  }
}
