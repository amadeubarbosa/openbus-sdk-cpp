// -*- coding: iso-8859-1-unix -*-

#include <check.hpp>
#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char** argv)
{
  openbus::log().set_level(openbus::debug_level);
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));

  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  conn->loginByPassword(cfg::user_entity_name, cfg::user_password, cfg::user_password_domain);

  bus_ctx->setDefaultConnection(conn);

  std::string token(
    std::string(conn->login()->entity.in()) + "@" +
    conn->login()->id.in() + ":" + "ExternalOriginator0, ExternalCaller");
  CORBA::OctetSeq token_seq(
    token.size(), token.size(), (CORBA::Octet*)token.c_str());

  try
  {
    openbus::CallerChain imported(bus_ctx->importChain(token_seq, "unknown_domain"));
    std::abort();
  }
  catch (const openbus::idl::access::UnknownDomain&)
  {
  }
  return 0; //MSVC
}
