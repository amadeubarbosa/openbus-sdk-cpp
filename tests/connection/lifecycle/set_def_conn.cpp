// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;

  boost::weak_ptr<openbus::Connection> weak_conn, weak_conn2, weak_conn3;
  
  {
    cfg::ctx_t ctx(cfg::init(argc, argv));
    openbus::OpenBusContext *bus_ctx(ctx.second);
    boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);

    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
    weak_conn = conn;

    bus_ctx->setDefaultConnection(conn);
    cfg::register_relogin_callback(conn);
    
    if (weak_conn.use_count() != 1)
    {
      std::cerr << "weak_conn.use_count() != 1" << std::endl;
      std::abort();
    }

    conn->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);
    openbus::idl::access::LoginRegistry_ptr login_registry
      (bus_ctx->getLoginRegistry());

    boost::shared_ptr<openbus::Connection> previous_conn(
      bus_ctx->setDefaultConnection(boost::shared_ptr<openbus::Connection>()));

    if (previous_conn != conn)
    {
      std::cerr << "previous_conn != conn" << std::endl;
      std::abort();
    }
    
    try
    {
      login_registry->getLoginValidity("invalid_id");
      std::abort();
    }
    catch (const CORBA::NO_PERMISSION &e)
    {
      if (openbus::idl::access::NoLoginCode != e.minor())
      {
        std::cerr << "openbus::idl::access::NoLoginCode != e.minor()"
                  << std::endl;
        std::abort();
      }
    }

    bus_ctx->setDefaultConnection(conn);
        
    boost::shared_ptr<openbus::Connection> conn2(
      bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
    conn2->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);
    weak_conn2 = conn2;
    bus_ctx->setDefaultConnection(conn2);
    cfg::register_relogin_callback(conn2);

    login_registry->getLoginValidity(conn2->login()->id);

    if (bus_ctx->getDefaultConnection() != conn2)
    {
      std::cerr << "bus_ctx->getDefaultConnection() != conn2" << std::endl;
      std::abort();
    }

    {
      boost::shared_ptr<openbus::Connection> conn3(
        bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
      conn3->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);
      weak_conn3 = conn3;
      bus_ctx->setDefaultConnection(conn3);
    }

    try
    {
      login_registry->getLoginValidity(conn->login()->id);
      std::abort();
    }
    catch (const CORBA::NO_PERMISSION &e)
    {
      if (openbus::idl::access::NoLoginCode != e.minor())
      {
        std::cerr << "openbus::idl::access::NoLoginCode != e.minor()"
                  << std::endl;
        std::abort();
      }
    }
  }

  if (!weak_conn.expired())
  {
    std::cerr << "weak_conn.expired() == false" << std::endl;
    std::abort();
  }

  if (!weak_conn2.expired())
  {
    std::cerr << "weak_conn2.expired() == false" << std::endl;
    std::abort();
  }

  return 0; //MSVC
}
