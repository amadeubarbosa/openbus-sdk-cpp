// -*- coding: iso-8859-1-unix -*-

#include <configuration.h>
#include <openbus.hpp>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  conn->loginByPassword(cfg.user(), cfg.password(), cfg.domain());

  const tecgraf::openbus::core::v2_1::services::access_control::LoginInfo
    *login(conn->login());
  if (!(login->id != 0 && std::strlen(login->id) != 0))
  {
    std::cerr << "!(login->id != 0 && std::strlen(login->id) != 0)"
              << std::endl;
    std::abort();
  }
  if (!(login->entity != 0
        && std::strcmp(login->entity, cfg.user().c_str()) == 0))
  {
    std::cerr << "!(login->entity != 0 \
        && std::strcmp(login->entity, cfg.user().c_str()) == 0))"
              << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
