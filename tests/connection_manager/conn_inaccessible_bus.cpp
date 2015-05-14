// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  assert(bus_ctx != 0);
  try
  {
    std::auto_ptr<openbus::Connection>
      conn(bus_ctx->createConnection(cfg.host(), 2090));
  }
  catch(...)
  {
    return 0;
  }
  std::abort();
  return 0; //MSVC
}
