
#include <openbus.h>

int main(int argc, char* argv[])
{
  CORBA::ORB_var orb (openbus::createORB(argc, argv));
  std::auto_ptr<openbus::Connection> conn(openbus::connect("localhost", 2089, orb.in()));
  assert(conn->orb() == orb.in());
}
