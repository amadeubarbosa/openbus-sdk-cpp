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
      conn(bus_ctx->connectByAddress(
             cfg::bus_host_name, (cfg::bus_host_port + 10)));
  }
  catch (const openbus::InvalidBusAddress &)
  {
    std::cout << "CORBA::SystemException was thrown." << std::endl;
    std::abort();
  }
  catch (...)
  {
    std::cout << "Unknown exception was thrown" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
