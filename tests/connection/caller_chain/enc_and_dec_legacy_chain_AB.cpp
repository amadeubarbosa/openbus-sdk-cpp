// -*- coding: iso-8859-1-unix -*-

#include "build_fake_legacy_chain.hpp"
#include "iop_codec.hpp"

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));
  CORBA::Object_ptr obj(orb->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(
    dynamic_cast<openbus::OpenBusContext *>(obj));
  
  std::auto_ptr<openbus::Connection> conn_A(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn_A->loginByPassword("A", "A");
  
  bus_ctx->setDefaultConnection(conn_A.get());

  openbus::idl_ac::LoginInfoSeq originators;
  openbus::idl_ac::LoginInfo caller;
  caller.id = conn_A->login()->id;
  caller.entity = conn_A->login()->entity;
  
  openbus::CallerChain fake_legacy_chain(
    build_fake_legacy_chain(conn_A->busid(), "B", originators, caller,
                            bus_ctx));
  
  CORBA::OctetSeq encoded(bus_ctx->encodeChain(fake_legacy_chain));
  openbus::CallerChain decoded_chain(bus_ctx->decodeChain(encoded));

  if (fake_legacy_chain.busid() != decoded_chain.busid()) std::abort();
  if (fake_legacy_chain.target() != decoded_chain.target()) std::abort();
  if (fake_legacy_chain.target() != "B") std::abort();
  if (std::string(fake_legacy_chain.caller().id.in())
      != std::string(decoded_chain.caller().id.in())) std::abort();
  if (std::string(fake_legacy_chain.caller().entity.in())
      != std::string(decoded_chain.caller().entity.in())) std::abort();
  if (std::string(decoded_chain.caller().entity.in()) != "A") std::abort();
  if (fake_legacy_chain.originators().length()
      != decoded_chain.originators().length()) std::abort();

  const openbus::idl_ac::CallChain *decoded_signed_chain;
  IOP::Codec_var codec(get_codec(bus_ctx));
  CORBA::Any_var decoded_call_chain_any(
    codec->decode_value(
      CORBA::OctetSeq(decoded_chain._signedCallChain.encoded.maximum(),
                      decoded_chain._signedCallChain.encoded.length(),
                      const_cast<unsigned char *>
                      (decoded_chain._signedCallChain.encoded.get_buffer())),
      openbus::idl_ac::_tc_CallChain));
  *decoded_call_chain_any >>= decoded_signed_chain;

  if (std::string(decoded_signed_chain->target) != decoded_chain.target())
    std::abort();
  if (std::string(decoded_signed_chain->target) != "B") std::abort();
  if (std::string(decoded_signed_chain->caller.id.in())
      != std::string(decoded_chain.caller().id.in())) std::abort();
  if (std::string(decoded_signed_chain->caller.entity.in())
      != std::string(decoded_chain.caller().entity.in())) std::abort();
  if (std::string(decoded_signed_chain->caller.entity.in()) != "A") std::abort();
  if (decoded_signed_chain->originators.length()
      != decoded_chain.originators().length()) std::abort();
  return 0; //MSVC
}
