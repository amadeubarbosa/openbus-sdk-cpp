#include <openbus/ConnectionManager.h>
#include <openbus/ORBInitializer.h>
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
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenbusConnectionManager");
  openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>(obj_connection_manager);
  std::auto_ptr <openbus::Connection> conn (manager->createConnection(cfg.host().c_str(), cfg.port()));
  CORBA::OctetSeq key;
  {
    std::ifstream key_file(argv[argc-1]);
    key_file.seekg(0, std::ios::end);
    std::size_t size = key_file.tellg();
    key_file.seekg(0, std::ios::beg);
    key.length(size);
    key_file.rdbuf()->sgetn(static_cast<char*>(static_cast<void*>(key.get_buffer())), size);
  }
  conn->loginByCertificate(cfg.certificate_user().c_str(), key);
}
