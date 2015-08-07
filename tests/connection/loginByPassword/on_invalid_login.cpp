// -*- coding: iso-8859-1-unix -*-
#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <configuration.h>

bool on_invalid_login_called(false); 

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
        on_invalid_login_called = true;
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
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
  CORBA::Object_ptr obj(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(
    dynamic_cast<openbus::OpenBusContext *>(obj));
  std::auto_ptr <openbus::Connection> conn(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  bus_ctx->setDefaultConnection(conn.get());
  conn->loginByPassword(cfg.user(), cfg.password());
  relogin_callback callback(cfg);
  conn->onInvalidLogin(callback);

  std::size_t validity(
    bus_ctx->getLoginRegistry()->getLoginValidity(conn->login()->id.in()));

  std::string invalid_login(conn->login()->id.in());
  bus_ctx->getLoginRegistry()->invalidateLogin(conn->login()->id.in());

  std::cout << "sleep_for: " << ++validity << "s" << std::endl;
  boost::this_thread::sleep_for(boost::chrono::seconds(validity));

  if (!on_invalid_login_called)
  {
    std::cerr << "on_invalid_login_called != true" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
