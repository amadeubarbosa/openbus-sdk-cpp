// -*- coding: iso-8859-1-unix -*-

#include <check.hpp>
#include <configuration.h>
#include <openbus.hpp>

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

  std::auto_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  conn->loginByPassword(cfg.user(), cfg.password());

  bus_ctx->setDefaultConnection(conn.get());

  std::string token("invalid_token");
  CORBA::OctetSeq token_seq(
    token.size(), token.size(), (CORBA::Octet*)token.c_str());

  try
  {
    openbus::CallerChain imported(bus_ctx->importChain(token_seq));
    std::abort();
  }
  catch (const openbus::idl_ac::InvalidToken&)
  {
  }
  return 0; //MSVC
}
