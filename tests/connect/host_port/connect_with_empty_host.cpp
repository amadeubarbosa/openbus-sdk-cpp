
#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
#include <cstdlib>

int main(int argc, char* argv[])
{
  try
  {
    CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenbusConnectionManager");
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>(obj_connection_manager);
    std::auto_ptr<openbus::Connection> conn(manager->createConnection("", 20989));
    std::cout << "No exception was thrown, exception COMM_FAILURE was expected" << std::endl;
    std::abort();
  }
  catch(openbus::InvalidBusAddress const&)
  {
  }
  catch(...)
  {
    std::cout << "Unknown exception was thrown" << std::endl;
    std::abort();
  }
}