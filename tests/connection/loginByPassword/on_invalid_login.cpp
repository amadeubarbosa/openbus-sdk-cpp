// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

namespace cfg = openbus::tests::config;

bool on_invalid_login_called(false);

struct relogin_callback
{
  void operator()(openbus::Connection &conn, openbus::idl::access::LoginInfo info)
  {
    do
    {
      try
      {
        conn.loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);
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
};

int main(int argc, char** argv)
{
  cfg::load_options(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  
  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  bus_ctx->setDefaultConnection(conn);
  conn->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);
  relogin_callback callback;
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
