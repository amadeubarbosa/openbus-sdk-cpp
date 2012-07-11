
#include <configuration.h>

#include <CORBA.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = CORBA::ORB_init(argc, argv, "");

  try
  {
    orb->resolve_initial_references("OpenbusConnectionManager");
    std::abort();
  }
  catch(CORBA::ORB::InvalidName const&)
  {
  }
}
