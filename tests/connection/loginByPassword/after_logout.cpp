// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);

  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  conn->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);
  conn->logout();
  conn->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);
  return 0; //MSVC
}
