// -*- coding: iso-8859-1-unix -*-
#include <openbus/OpenBusContext.hpp>
#include <openbus/log.hpp>
#include <configuration.h>

#include <fstream>
#include <iterator>
#include <algorithm>

// Last argument is assumed to be the path for the private key file argv[argc-1]
int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  assert(argc >= 2);
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

  std::auto_ptr <openbus::Connection> 
    conn (bus_ctx->connectByAddress(cfg.host(), cfg.wrong_port()));
  try
  {
    conn->loginByCertificate(cfg.certificate_user(), 
                             openbus::PrivateKey(argv[argc-1]));
    std::cout << "No exception was thrown, exception CORBA::SystemException was expected"
              << std::endl;
    std::abort();
   }
   catch (const CORBA::SystemException &)
   {
     std::cout << "CORBA::SystemException was thrown." << std::endl;
   }
  return 0; //MSVC
}
