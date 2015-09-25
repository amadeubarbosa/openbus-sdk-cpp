// -*- coding: iso-8859-1-unix -*-

#include <configuration.h>
#include <openbus.hpp>

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  
  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  try
  {
    conn->loginByPassword(cfg.user(), cfg.password() + "WRONG", cfg.domain());
    std::abort();
  }
  catch(tecgraf::openbus::core::v2_1::services::access_control::AccessDenied const&)
  {
  }
  return 0; //MSVC
}
