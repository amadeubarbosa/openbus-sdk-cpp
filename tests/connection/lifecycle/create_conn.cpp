// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char** argv)
{
  openbus::log().set_level(openbus::debug_level);
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);

  boost::weak_ptr<openbus::Connection> weak_conn;
  
  {
    std::auto_ptr<openbus::orb_ctx>    
      orb_ctx(openbus::ORBInitializer(argc, argv));
    CORBA::Object_var
      obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
    openbus::OpenBusContext
      *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
    weak_conn = conn;
  }

  if (!weak_conn.expired())
  {
    std::cerr << "weak_conn.expired() == false" << std::endl;
    std::abort();
  }

  return 0; //MSVC
}
