// -*- coding: iso-8859-1-unix -*-

#include <configuration.h>
#include <openbus.hpp>
#include <boost/thread.hpp>

int main(int argc, char** argv)
{
  openbus::configuration cfg(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  boost::this_thread::sleep_for(boost::chrono::seconds(5));
  for (std::size_t i(0); i < 3; ++i)
  {
    try
    {
      conn->loginByPassword(cfg.user(), "invalid_password", cfg.domain());
      std::abort();
    }
    catch (const openbus::idl::access::AccessDenied &)
    {
    }
  }
  try
  {
    conn->loginByPassword(cfg.user(), cfg.password(), cfg.domain());
    std::abort();
  }
  catch (const openbus::idl::access::TooManyAttempts &)
  {
  }
  boost::this_thread::sleep_for(boost::chrono::seconds(5));
  return 0; //MSVC
}
