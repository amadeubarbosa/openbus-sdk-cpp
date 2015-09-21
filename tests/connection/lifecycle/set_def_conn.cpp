// -*- coding: iso-8859-1-unix -*-

#include <configuration.h>
#include <openbus.hpp>

int main(int argc, char** argv)
{
  openbus::log().set_level(openbus::debug_level);
  openbus::configuration cfg(argc, argv);

  boost::weak_ptr<openbus::Connection> weak_conn, weak_conn2, weak_conn3;
  
  {
    boost::shared_ptr<openbus::orb_ctx>    
      orb_ctx(openbus::ORBInitializer(argc, argv));
    CORBA::Object_var
      obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
    openbus::OpenBusContext
      *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(cfg.host(), cfg.port()));
    weak_conn = conn;

    bus_ctx->setDefaultConnection(conn);
    
    if (weak_conn.use_count() != 1)
    {
      std::cerr << "weak_conn.use_count() != 1" << std::endl;
      std::abort();
    }

    conn->loginByPassword(cfg.user(), cfg.password(), cfg.domain());
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
      bus_ctx->connectByAddress(cfg.host(), cfg.port()));
    conn2->loginByPassword(cfg.user(), cfg.password(), cfg.domain());
    weak_conn2 = conn2;
    bus_ctx->setDefaultConnection(conn2);

    login_registry->getLoginValidity(conn2->login()->id);

    if (bus_ctx->getDefaultConnection() != conn2)
    {
      std::cerr << "bus_ctx->getDefaultConnection() != conn2" << std::endl;
      std::abort();
    }

    {
      boost::shared_ptr<openbus::Connection> conn3(
        bus_ctx->connectByAddress(cfg.host(), cfg.port()));
      conn3->loginByPassword(cfg.user(), cfg.password(), cfg.domain());
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
