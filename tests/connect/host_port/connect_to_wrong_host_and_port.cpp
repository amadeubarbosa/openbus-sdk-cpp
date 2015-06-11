// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/log.hpp>
#include <cstdlib>
#include <configuration.h>

int main(int argc, char* argv[])
{
  try
  {
    openbus::log().set_level(openbus::debug_level);
    openbus::configuration cfg(argc, argv);
    CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
    CORBA::Object_ptr o(orb->resolve_initial_references("OpenBusContext"));
    openbus::OpenBusContext *bus_ctx(
      dynamic_cast<openbus::OpenBusContext *>(o));
    std::auto_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(cfg.host(), cfg.wrong_port()));
    std::cout << "No exception was thrown, exception CORBA::SystemException was expected"
              << std::endl;
    std::abort();
  }
  catch (const CORBA::SystemException &)
  {
  }
  catch (...)
  {
    std::cout << "Unknown exception was thrown" << std::endl;
    std::abort();
  }
}
