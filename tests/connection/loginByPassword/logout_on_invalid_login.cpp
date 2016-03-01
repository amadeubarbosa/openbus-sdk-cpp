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
    boost::lock_guard<boost::mutex> lock(mtx);
    on_invalid_login_called = true;
    cond.notify_one();
  }
};

int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);

  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  bus_ctx->setDefaultConnection(conn);
  conn->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);
  relogin_callback callback;
  conn->onInvalidLogin(callback);

  std::size_t validity(
    bus_ctx->getLoginRegistry()->getLoginValidity(conn->login()->id.in()));
  
  bus_ctx->getLoginRegistry()->invalidateLogin(conn->login()->id.in());
  bool unlogged(conn->logout());
  if (!unlogged)
  {
    std::cerr << "unlogged == false: Logout nao foi realizado." << std::endl;
    std::abort();
  }

  boost::unique_lock<boost::mutex> lock(mtx);
  while (!on_invalid_login_called)
  {
    if (boost::cv_status::timeout
        == cond.wait_for(lock, boost::chrono::seconds(validity)))
    {
      return 0;
    }
  }
  std::cerr << "on_invalid_login_called == true: Callback onInvalidLogin " \
    "executada indevidamente."
            << std::endl;
  std::abort();
  return 0; //MSVC
}
