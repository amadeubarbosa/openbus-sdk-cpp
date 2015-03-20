// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_TESTS_BUILD_IOP_CODEC_HPP
#define TECGRAF_SDK_OPENBUS_TESTS_BUILD_IOP_CODEC_HPP

#include "iop_codec.hpp"

openbus::CallerChain build_fake_legacy_chain(
  const std::string &busid, 
  const std::string &target,
  const openbus::idl_ac::LoginInfoSeq &originators, 
  const openbus::idl_ac::LoginInfo &caller,
  openbus::OpenBusContext *bus_ctx) 
{
  openbus::idl_cr::SignedCallChain signed_chain;
  std::memset(signed_chain.signature, '\0', openbus::idl::EncryptedBlockSize);

  openbus::idl_ac::CallChain call_chain;
  call_chain.target = target.c_str();
  call_chain.originators = originators;
  call_chain.caller = caller;

  IOP::Codec_var codec(get_codec(bus_ctx));
  
  CORBA::Any any;
  any <<= call_chain;
  CORBA::OctetSeq_var seq(codec->encode_value(any));
  signed_chain.encoded = openbus::idl::OctetSeq(
    seq->maximum(),
    seq->length(),
    const_cast<unsigned char *>(seq->get_buffer()));
  
  return openbus::CallerChain(busid, target, originators, caller, signed_chain);
}

#endif