// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);
  boost::weak_ptr<openbus::Connection> weak_conn;
  {
    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
    weak_conn = conn;
    bus_ctx->setDefaultConnection(conn);
  }
  if (!weak_conn.expired())
  {
    std::cerr << "weak_conn.expired() == false" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
