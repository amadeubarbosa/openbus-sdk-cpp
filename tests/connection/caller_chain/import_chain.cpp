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

  boost::shared_ptr<openbus::Connection> conn(
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  conn->loginByPassword(cfg.user(), cfg.password(), cfg.domain());

  bus_ctx->setDefaultConnection(conn);

  std::string token(
    std::string(conn->login()->entity.in()) + "@" +
    conn->login()->id.in() + ":" + "ExternalOriginator0, ExternalCaller");
  CORBA::OctetSeq token_seq(
    token.size(), token.size(), (CORBA::Octet*)token.c_str());
    
  openbus::CallerChain imported(bus_ctx->importChain(token_seq, cfg.domain()));

  if (conn->busid() != imported.busid())
  {
    std::cerr << "conn->busid() != imported.busid()" << std::endl;
    std::abort();
  }

  if (std::string(conn->login()->entity.in()) != imported.target())
  {
    std::cerr << "std::string(conn->login()->entity.in()) != imported.target()"
              << std::endl;
    std::abort();
  }

  if (1 != imported.originators().length())
  {
    std::cerr << "1 != imported.originators().length()" << std::endl;
    std::abort();
  }

  if ("ExternalOriginator0" !=
      std::string(imported.originators()[0u].entity.in()))
  {
    std::cerr
      << "\"ExternalOriginator0\" != std::string(imported.originators()[0u])"
      << std::endl;
    std::abort();
  }

  if ("ExternalCaller" != std::string(imported.caller().entity.in()))
  {
    std::cerr << "\"ExternalCaller\" != imported.caller()" << std::endl;
    std::abort();
  }

  const std::string fake_entity("fake_entity");
  bus_ctx->joinChain(imported);
  openbus::CallerChain joined(bus_ctx->makeChainFor(fake_entity));
  bus_ctx->exitChain();
  
  if (conn->busid() != joined.busid())
  {
    std::cerr << "conn->busid() != joined.busid()" << std::endl;
    std::abort();
  }

  if (fake_entity != joined.target())
  {
    std::cerr << "fake_entity != joined.target()"
              << std::endl;
    std::abort();
  }

  if (2 != joined.originators().length())
  {
    std::cerr << "2 != joined.originators().length()" << std::endl;
    std::abort();
  }

  if ("ExternalOriginator0" !=
      std::string(joined.originators()[0u].entity.in()))
  {
    std::cerr
      << "\"ExternalOriginator0\" != std::string(joined.originators()[0u])"
      << std::endl;
    std::abort();
  }

  if ("ExternalCaller" !=
      std::string(joined.originators()[1u].entity.in()))
  {
    std::cerr
      << "\"ExternalCaller\" != std::string(joined.originators()[1u])"
      << std::endl;
    std::abort();
  }

  if (conn->login()->id.in() != std::string(joined.caller().id.in()))
  {
    std::cerr << "conn->login()->id.in() != joined.caller()" << std::endl;
    std::abort();
  }

  if (conn->login()->entity.in() != std::string(joined.caller().entity.in()))
  {
    std::cerr << "conn->login()->entity.in() != joined.caller()" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
