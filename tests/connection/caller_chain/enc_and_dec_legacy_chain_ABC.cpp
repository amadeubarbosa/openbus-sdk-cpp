// -*- coding: iso-8859-1-unix -*-

#include "build_fake_legacy_chain.hpp"
#include "iop_codec.hpp"

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>
#include <check.hpp>
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
  openbus::configuration cfg(argc, argv);
  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  
  boost::shared_ptr<openbus::Connection> conn_B(
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  conn_B->loginByPassword("B", "B", cfg.domain());
    
  bus_ctx->setDefaultConnection(conn_B.get());

  openbus::idl::access::LoginInfoSeq originators;
  originators.length(1);
  openbus::idl::access::LoginInfo delegate;
  delegate.id = "<unknown>";
  delegate.entity = "A";
  originators[static_cast<CORBA::ULong>(0)] = delegate;
  
  openbus::idl::access::LoginInfo caller;
  caller.id = conn_B->login()->id;
  caller.entity = conn_B->login()->entity;
  
  openbus::CallerChain fake_legacy_chain(
    build_fake_legacy_chain(conn_B->busid(), "C", originators, caller,
                            bus_ctx));
  
  CORBA::OctetSeq encoded(bus_ctx->encodeChain(fake_legacy_chain));
  openbus::CallerChain decoded_chain(bus_ctx->decodeChain(encoded));

  openbus::tests::is_equal<std::string>(
    fake_legacy_chain.busid(), decoded_chain.busid(),
    "fake_legacy_chain.busid()", "decoded_chain.busid()");
  
  openbus::tests::is_equal<std::string>(
    fake_legacy_chain.target(), decoded_chain.target(),
    "fake_legacy_chain.target()", "decoded_chain.target()");

  openbus::tests::is_equal<std::string>(
    fake_legacy_chain.target(), "C", "fake_legacy_chain.target()", "C");

  openbus::tests::is_equal<std::string>(
    std::string(fake_legacy_chain.caller().id.in()),
    std::string(decoded_chain.caller().id.in()),
    "fake_legacy_chain.caller().id", "decoded_chain.caller().id");

  openbus::tests::is_equal<std::string>(
    std::string(fake_legacy_chain.caller().entity.in()),
    std::string(decoded_chain.caller().entity.in()),
    "fake_legacy_chain.caller().entity", "decoded_chain.caller().entity");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_chain.caller().entity.in()), "B",
    "decoded_chain.caller().entity", "B");

  openbus::tests::is_equal<std::size_t>(
    fake_legacy_chain.originators().length(),
    decoded_chain.originators().length(),
    "fake_legacy_chain.originators().length()",
    "decoded_chain.originators().length()");

  openbus::tests::is_equal<std::size_t>(
    fake_legacy_chain.originators().length(), 1,
    "fake_legacy_chain.originators().length()", "1");

  openbus::tests::is_equal<std::string>(
    std::string(fake_legacy_chain.originators()[0u].id.in()),
    std::string(decoded_chain.originators()[0u].id.in()),
    "fake_legacy_chain.originators()[0u].id",
    "decoded_chain.originators()[0u].id");

  openbus::tests::is_equal<std::string>(
    std::string(fake_legacy_chain.originators()[0u].entity.in()),
    std::string(decoded_chain.originators()[0u].entity.in()),
    "fake_legacy_chain.originators()[0u].entity",
    "decoded_chain.originators()[0u].entity");;

  const openbus::idl::access::CallChain *decoded_signed_chain;
  IOP::Codec_var codec(get_codec(bus_ctx));
  CORBA::Any_var decoded_call_chain_any(
    codec->decode_value(
      CORBA::OctetSeq(decoded_chain._signedCallChain.encoded.maximum(),
                      decoded_chain._signedCallChain.encoded.length(),
                      const_cast<unsigned char *>
                      (decoded_chain._signedCallChain.encoded.get_buffer())),
      openbus::idl::access::_tc_CallChain));
  *decoded_call_chain_any >>= decoded_signed_chain;

  openbus::tests::is_equal<std::string>(
    decoded_signed_chain->target.in(), decoded_chain.target(),
    "decoded_signed_chain->target", "decoded_chain.target()");

  openbus::tests::is_equal<std::string>(
    decoded_signed_chain->target.in(), "C",
    "decoded_signed_chain->target()", "C");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_signed_chain->caller.id.in()),
    std::string(decoded_chain.caller().id.in()),
    "decoded_signed_chain->caller.id", "decoded_chain.caller().id");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_signed_chain->caller.entity.in()),
    std::string(decoded_chain.caller().entity.in()),
    "decoded_signed_chain->caller.entity", "decoded_chain.caller().entity");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_chain.caller().entity.in()), "B",
    "decoded_chain.caller().entity", "B");

  openbus::tests::is_equal<std::size_t>(
    decoded_signed_chain->originators.length(),
    decoded_chain.originators().length(),
    "decoded_signed_chain->originators.length()",
    "decoded_chain.originators().length()");

  openbus::tests::is_equal<std::size_t>(
    decoded_signed_chain->originators.length(), 1,
    "decoded_signed_chain->originators.length()", "1");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_signed_chain->originators[0u].id.in()),
    std::string(decoded_chain.originators()[0u].id.in()),
    "decoded_signed_chain->originators[0u].id",
    "decoded_chain.originators()[0u].id");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_signed_chain->originators[0u].entity.in()),
    std::string(decoded_chain.originators()[0u].entity.in()),
    "decoded_signed_chain->originators[0u].entity",
    "decoded_chain.originators()[0u].entity");;

  return 0; //MSVC
}
