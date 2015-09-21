// -*- coding: iso-8859-1-unix -*-

#include <configuration.h>
#include <openbus.hpp>

bool on_invalid_login_called(false);

struct relogin_callback
{
  relogin_callback(const openbus::configuration &cfg)
    : cfg(cfg)
  {
  }
  
  void operator()(openbus::Connection &conn, openbus::idl::access::LoginInfo info)
  {
    do
    {
      try
      {
        conn.loginByPassword(cfg.user(), cfg.password(), cfg.domain());
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
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  bus_ctx->setDefaultConnection(conn);
  conn->loginByPassword(cfg.user(), cfg.password(), cfg.domain());
  relogin_callback callback(cfg);
  conn->onInvalidLogin(callback);
  std::string invalid_login(conn->login()->id.in());

  std::size_t validity(
    bus_ctx->getLoginRegistry()->getLoginValidity(conn->login()->id.in()));
  
  bus_ctx->getLoginRegistry()->invalidateLogin(conn->login()->id.in());
  bool unlogged(conn->logout());
  if (!unlogged)
  {
    std::cerr << "unlogged == false: Logout nao foi realizado." << std::endl;
    std::abort();
  }

  std::cout << "sleep_for: " << ++validity << "s" << std::endl;
  boost::this_thread::sleep_for(boost::chrono::seconds(validity));

  if (on_invalid_login_called)
  {
    std::cerr << "on_invalid_login_called == true: Callback onInvalidLogin executada indevidamente."
              << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
