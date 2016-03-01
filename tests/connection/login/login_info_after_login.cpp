// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char* argv[])
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);

  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  conn->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);

  const tecgraf::openbus::core::v2_1::services::access_control::LoginInfo
    *login(conn->login());
  if (!(login->id != 0 && std::strlen(login->id) != 0))
  {
    std::cerr << "!(login->id != 0 && std::strlen(login->id) != 0)"
              << std::endl;
    std::abort();
  }
  if (!(login->entity != 0
        && std::strcmp(login->entity, cfg::user_entity_name.c_str()) == 0))
  {
    std::cerr << "!(login->entity != 0 \
        && std::strcmp(login->entity, cfg::user_entity_name.c_str()) == 0))"
              << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
