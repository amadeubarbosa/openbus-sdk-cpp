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
  boost::shared_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  
  std::auto_ptr<openbus::Connection> conn_A(
    bus_ctx->connectByAddress(cfg.host(), cfg.port()));
  conn_A->loginByPassword("A", "A");
  
  bus_ctx->setDefaultConnection(conn_A.get());

  openbus::idl::access::LoginInfoSeq originators;
  openbus::idl::access::LoginInfo caller;
  caller.id = conn_A->login()->id;
  caller.entity = conn_A->login()->entity;
  
  openbus::CallerChain fake_legacy_chain(
    build_fake_legacy_chain(conn_A->busid(), "B", originators, caller,
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
    fake_legacy_chain.target(), decoded_chain.target(),
    "B", "B");

  openbus::tests::is_equal<std::string>(
    std::string(fake_legacy_chain.caller().id.in()),
    std::string(decoded_chain.caller().id.in()),
    "fake_legacy_chain.caller().id",
    "decoded_chain.caller().id");

  openbus::tests::is_equal<std::string>(
    std::string(fake_legacy_chain.caller().entity.in()),
    std::string(decoded_chain.caller().entity.in()),
    "fake_legacy_chain.caller().entity",
    "decoded_chain.caller().entity");

  openbus::tests::is_equal<std::string>(
    std::string(fake_legacy_chain.caller().entity.in()), "A",
    "fake_legacy_chain.caller().entity", "A");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_chain.caller().entity.in()), "A",
    "decoded_chain.caller().entity", "A");

  openbus::tests::is_equal<std::size_t>(
    fake_legacy_chain.originators().length(),
    decoded_chain.originators().length(),
    "fake_legacy_chain.originators().length()",
    "decoded_chain.originators().length()");

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
    std::string(decoded_signed_chain->target), decoded_chain.target(),
    "std::string(decoded_signed_chain->target)", "decoded_chain.target()");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_signed_chain->target), "B",
    "std::string(decoded_signed_chain->target)", "B");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_signed_chain->caller.id.in()),
    std::string(decoded_chain.caller().id.in()),
    "decoded_signed_chain->caller.id",
    "decoded_chain.caller().id");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_signed_chain->caller.entity.in()),
    std::string(decoded_chain.caller().entity.in()),
    "decoded_signed_chain->caller.entity",
    "decoded_chain.caller().entity");

  openbus::tests::is_equal<std::string>(
    std::string(decoded_signed_chain->caller.entity.in()), "A",
    "decoded_signed_chain->caller.entity", "A");

  openbus::tests::is_equal<std::size_t>(
    decoded_signed_chain->originators.length(),
    decoded_chain.originators().length(),
    "decoded_signed_chain->originators.length()",
    "decoded_chain.originators().length()");
  
  return 0; //MSVC
}
