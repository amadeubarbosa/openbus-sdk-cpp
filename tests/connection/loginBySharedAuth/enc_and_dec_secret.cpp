// -*- coding: iso-8859-1-unix -*-
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
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
  CORBA::Object_ptr obj(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(
    dynamic_cast<openbus::OpenBusContext *>(obj));
  
  std::auto_ptr<openbus::Connection> conn(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn->loginByCertificate(cfg.certificate_user(), 
                           openbus::PrivateKey(argv[argc-1]));

  CORBA::OctetSeq shared_auth_seq(
    bus_ctx->encodeSharedAuthSecret(conn->startSharedAuth()));

  openbus::SharedAuthSecret shared_auth(
    bus_ctx->decodeSharedAuthSecret(shared_auth_seq));
  
  {
    std::auto_ptr<openbus::Connection> conn(
      bus_ctx->createConnection(cfg.host(), cfg.port()));
    conn->loginBySharedAuth(shared_auth);
    if (conn->login() == 0)
    {
      std::cerr << "conn->login() == 0" << std::endl;
      std::abort();
    }
  }
  return 0; //MSVC
}
