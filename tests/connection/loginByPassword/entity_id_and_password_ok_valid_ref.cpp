// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char** argv)
{
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  openbus::log().set_level(openbus::debug_level);
  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  std::stringstream corbaloc;
  corbaloc << "corbaloc::" << cfg::bus_host_name << ":" << cfg::bus_host_port
           << "/" << tecgraf::openbus::core::v2_1::BusObjectKey;
  CORBA::Object_var
    ref(orb_ctx->orb()->string_to_object(corbaloc.str().c_str()));
  
  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByReference(ref));
  conn->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);
  return 0; //MSVC
}
