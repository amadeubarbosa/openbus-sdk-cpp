// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

#include <cstdlib>

int main(int argc, char* argv[])
{
  try
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
      conn(bus_ctx->connectByAddress("$invalid_host$", cfg::bus_host_port));
  }
  catch (const openbus::InvalidBusAddress &)
  {
    std::cout << "CORBA::TRANSIENT was thrown." << std::endl;
    std::abort();
  }
  catch (...)
  {
    std::cout << "Unknown exception was thrown" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
