// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>
#include <boost/thread.hpp>

int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  boost::this_thread::sleep_for(boost::chrono::seconds(5));
  for (std::size_t i(0); i < 3; ++i)
  {
    try
    {
      conn->loginByPassword(cfg::user_entity_name, "invalid_password", cfg::user_password_domain);
      std::abort();
    }
    catch (const openbus::idl::access::AccessDenied &)
    {
    }
  }
  try
  {
    conn->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);
    std::abort();
  }
  catch (const openbus::idl::access::TooManyAttempts &)
  {
  }
  boost::this_thread::sleep_for(boost::chrono::seconds(5));
  return 0; //MSVC
}
