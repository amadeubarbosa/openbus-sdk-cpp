// -*- coding: iso-8859-1-unix -*-
#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  std::auto_ptr <openbus::Connection>
    conn(bus_ctx->createConnection(cfg.host(), cfg.port()));
  try
  {
    conn->loginByPassword(cfg.user().c_str(),
			  (cfg.password() + "WRONG").c_str());
    std::abort();
  }
  catch(tecgraf::openbus::core::v2_0::services::access_control::AccessDenied const&)
  {
  }
  return 0; //MSVC
}
