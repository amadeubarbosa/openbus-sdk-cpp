
#include <openbus.h>

int main(int argc, char* argv[])
{
  CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "");
  openbus::ConnectionManager* manager = openbus::getConnectionManager(orb);

  std::auto_ptr<openbus::Connection> conn(manager->createConnection("localhost", 2089));
}
