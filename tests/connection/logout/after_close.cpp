
#include <openbus.h>
#include <cstdlib>

int main(int argc, char* argv[])
{
  CORBA::ORB_var orb = openbus::initORB(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::auto_ptr<openbus::Connection> conn(openbusContext->createConnect("localhost", 2089));
  conn->loginByPassword("demo", "demo");
  conn->close();
  if(conn->logout())
    std::abort();
}
