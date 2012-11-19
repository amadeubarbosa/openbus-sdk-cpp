#include <openbus/OpenBusContext.h>
#include <openbus/ORBInitializer.h>
#include <configuration.h>

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::auto_ptr <openbus::Connection> conn (openbusContext->createConnection("localhost", 2089));
  try
  {
    conn->loginByPassword(cfg.user().c_str(), (cfg.password() + "WRONG").c_str());
    std::abort();
  }
  catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const&)
  {
  }
}
