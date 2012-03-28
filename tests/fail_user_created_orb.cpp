
#include <openbus.h>

int main(int argc, char* argv[])
{
  CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "");

  std::auto_ptr<openbus::Connection> conn(openbus::connect("localhost", 2089, orb.in()));
}
