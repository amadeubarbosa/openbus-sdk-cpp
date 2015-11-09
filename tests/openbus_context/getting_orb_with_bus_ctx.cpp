// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char *argv[])
{
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  if (cfg::openbus_test_verbose)
  {
    openbus::log()->set_level(openbus::debug_level);
  }

  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

  if (CORBA::is_nil(bus_ctx->orb()))
  {
    std::cerr << "CORBA::is_nil(conn.orb())) == true" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
