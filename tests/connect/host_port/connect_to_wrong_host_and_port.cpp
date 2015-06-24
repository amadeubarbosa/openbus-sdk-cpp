// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/log.hpp>
#include <configuration.h>

#include <cstdlib>

int main(int argc, char* argv[])
{
  try
  {
    openbus::configuration cfg(argc, argv);
    openbus::log().set_level(openbus::debug_level);
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    CORBA::Object_var
      obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
    openbus::OpenBusContext
      *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
    std::auto_ptr<openbus::Connection>
      conn(bus_ctx->connectByAddress(cfg.host(), cfg.wrong_port()));
  }
  catch (const CORBA::SystemException &)
  {
    std::cout << "CORBA::SystemException was thrown." << std::endl;
    std::abort();
  }
  catch (...)
  {
    std::cout << "Unknown exception was thrown" << std::endl;
    std::abort();
  }
}
