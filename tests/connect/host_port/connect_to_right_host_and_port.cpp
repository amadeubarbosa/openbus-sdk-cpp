// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char* argv[])
{
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);  

  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByAddress(openbus::_host=cfg::bus_host_name,
                                   openbus::_port=cfg::bus_host_port));
  return 0; //MSVC
}
