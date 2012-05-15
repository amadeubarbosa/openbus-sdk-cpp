#include <openbus.h>

// Last argument is assumed to be the path for the private key file argv[argc-1]
int main(int argc, char** argv)
{
  assert(argc >= 2); // Check that there's at least one argument passed
  std::auto_ptr <openbus::Connection> conn (openbus::connect("localhost", 2089));
  conn->loginByCertificate("demo", argv[argc-1]);
}
