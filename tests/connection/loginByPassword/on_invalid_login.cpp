// -*- coding: iso-8859-1-unix -*-
#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <configuration.h>

bool ok = false;

struct relogin_callback
{
  relogin_callback(const openbus::configuration &cfg)
    : cfg(cfg)
  {
  }
  
  void operator()(openbus::Connection &conn, openbus::idl_ac::LoginInfo info)
  {
    do
    {
      try
      {
        conn.loginByPassword(cfg.user(), cfg.password());
        ok = true;
        break;
      }
      catch (const CORBA::Exception &)
      {
      }
#ifndef _WIN32
      unsigned int t = 30u;
      do { t = sleep(t); } while(t);
#else
      Sleep(3000);
#endif
    }
    while(true);
  }
  openbus::configuration cfg;
};

int main(int argc, char** argv)
{
  openbus::log().set_level(openbus::debug_level);
  openbus::configuration cfg(argc, argv);
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  std::auto_ptr <openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  bus_ctx->setDefaultConnection(conn.get());
  conn->loginByPassword(cfg.user(), cfg.password());
  relogin_callback callback(cfg);
  conn->onInvalidLogin(callback);
  std::string invalid_login(conn->login()->id.in());
  bus_ctx->getLoginRegistry()->invalidateLogin(conn->login()->id.in());
  std::size_t validity(
    bus_ctx->getLoginRegistry()->getLoginValidity(invalid_login.c_str()));
  if (!ok)
  {
    std::cerr << "ok != true" << std::endl;
    std::abort();
  }
  if (validity > 0)
  {
    std::cerr << "getLoginValidity() > 0" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
