#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>

#include <fstream>
#include <iterator>
#include <algorithm>

// Last argument is assumed to be the path for the private key file argv[argc-1]
int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  assert(argc >= 2); // Check that there's at least one argument passed
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = 
    orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = 
    dynamic_cast<openbus::OpenBusContext *> (obj_connection_manager);
  std::auto_ptr <openbus::Connection> 
    conn (openbusContext->createConnection(cfg.host(), cfg.port()));
  conn->loginByCertificate(cfg.certificate_user(), 
                           openbus::PrivateKey(argv[argc-1]));
}
