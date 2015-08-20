// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_TESTS_BUILD_FAKE_LEGACY_CHAIN_HPP
#define TECGRAF_SDK_OPENBUS_TESTS_BUILD_FAKE_LEGACY_CHAIN_HPP

#include <openbus/openbus_context.hpp>

IOP::Codec_var get_codec(openbus::OpenBusContext *bus_ctx)
{
  CORBA::Object_var codec_factory_obj(
    bus_ctx->orb()->resolve_initial_references("CodecFactory"));
  ::IOP::CodecFactory_var codec_factory(::IOP::CodecFactory::_narrow(
                                          codec_factory_obj));
  IOP::Encoding cdr_encoding = {IOP::ENCODING_CDR_ENCAPS, 1, 2};
  IOP::Codec_var codec(codec_factory->create_codec(cdr_encoding));
  return codec._retn();
}

#endif
