
#include <openbus.h>
#include <cstdlib>

int main(int argc, char* argv[])
{
  try
  {
    std::auto_ptr<openbus::Connection> conn(openbus::connect("localhost", 9999 /* Nenhum barramento nesta porta */
                                                             , openbus::createORB(argc, argv)));
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
