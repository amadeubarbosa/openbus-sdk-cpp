
#include <openbus.h>

int main(int argc, char* argv[])
{
  CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "");
  openbus::ConnectionManager* manager = openbus::getConnectionManager(orb);
  assert(manager == 0);
}
