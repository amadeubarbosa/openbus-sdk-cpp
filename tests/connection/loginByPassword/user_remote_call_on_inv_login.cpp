// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

#include <boost/thread.hpp>
#include <iostream>

namespace cfg = openbus::tests::config;

boost::mutex mtx;
boost::condition_variable cond;
bool on_invalid_login_called(false);

struct relogin_callback
{
  void operator()(boost::shared_ptr<openbus::Connection> conn,
                  openbus::idl::access::LoginInfo info)
  {
    openbus::log_scope log(openbus::log()->general_logger(),
                           openbus::info_level, "relogin_callback");
    do
    {
      try
      {
        conn->loginByPassword(
          cfg::user_entity_name,
          cfg::user_password,
          cfg::user_password_domain);
        break;
      }
      catch (const CORBA::Exception &)
      {
      }
      boost::this_thread::sleep_for(boost::chrono::seconds(1));
    }
    while(true);
    boost::lock_guard<boost::mutex> lock(mtx);
    on_invalid_login_called = true;
    cond.notify_one();
  }
};

int main(int argc, char** argv)
{
  cfg::load_options(argc, argv);
  if (cfg::openbus_test_verbose)
  {
    openbus::log()->set_level(openbus::debug_level);
  }

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

  bus_ctx->getLoginRegistry()->getLoginValidity(conn->login()->id.in());  

  boost::unique_lock<boost::mutex> lock(mtx);
  while (!on_invalid_login_called)
  {
    if (boost::cv_status::timeout
        == cond.wait_for(lock, boost::chrono::seconds(validity*2)))
    {
      std::cerr << "on_invalid_login_called != true" << std::endl;
      std::abort();
    }
  }
  return 0; //MSVC
}
