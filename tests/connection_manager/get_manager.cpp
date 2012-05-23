
#include <openbus.h>

int main(int argc, char* argv[])
{
  CORBA::ORB_var orb = openbus::initORB(argc, argv);
  openbus::ConnectionManager* manager = openbus::getConnectionManager(orb);
  assert(manager != 0);
}
