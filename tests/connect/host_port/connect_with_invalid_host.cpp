// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

#include <cstdlib>

int main(int argc, char* argv[])
{
  try
  {
    namespace cfg = openbus::tests::config;
    cfg::ctx_t ctx(cfg::init(argc, argv));
    openbus::OpenBusContext *bus_ctx(ctx.second);  

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
