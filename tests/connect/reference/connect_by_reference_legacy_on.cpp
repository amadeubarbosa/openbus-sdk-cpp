// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char* argv[])
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);

  std::stringstream corbaloc;
  corbaloc << "corbaloc::" << cfg::bus_host_name << ":" << cfg::bus_host_port
           << "/" << tecgraf::openbus::core::v2_1::BusObjectKey;
  CORBA::Object_var
    ref(orb_ctx->orb()->string_to_object(corbaloc.str().c_str()));
  
  boost::shared_ptr<openbus::Connection>
    conn(bus_ctx->connectByReference(ref,
                                     openbus::_legacy_support=true));
  return 0; //MSVC
}
