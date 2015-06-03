// -*- coding: iso-8859-1-unix -*-

#include "iop_codec.hpp"

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>
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
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn_A->loginByPassword("A", "A");
  
  std::auto_ptr<openbus::Connection> conn_B(
    bus_ctx->createConnection(cfg.host(), cfg.port()));
  conn_B->loginByPassword("B", "B");
  
  bus_ctx->setDefaultConnection(conn_A.get());
  
  openbus::CallerChain chain(
    bus_ctx->makeChainFor(conn_B->login()->id.in()));

  CORBA::OctetSeq encoded_chain(bus_ctx->encodeChain(chain));

  const std::size_t magic_tag_size(4);
  CORBA::OctetSeq seq;
  seq.length(encoded_chain.length() - magic_tag_size);
  std::memcpy(seq.get_buffer(), encoded_chain.get_buffer() + magic_tag_size,
              seq.length());
  IOP::Codec_var codec(get_codec(bus_ctx));
  CORBA::Any_var any(codec->decode_value(
                       seq, openbus::idl_data_export::_tc_ExportedVersionSeq));
  const openbus::idl_data_export::ExportedVersionSeq *tmp;
  *any >>= tmp;
  openbus::idl_data_export::ExportedVersionSeq
    exported_incompatible_version_seq(*tmp);
    
  CORBA::ULong incompatible_version(0);
  exported_incompatible_version_seq[static_cast<CORBA::ULong>(0)].version =
    incompatible_version;
  exported_incompatible_version_seq[static_cast<CORBA::ULong>(1)].version =
    incompatible_version;

  CORBA::Any exported_incompatible_version_seq_any;
  exported_incompatible_version_seq_any <<= exported_incompatible_version_seq;
  CORBA::OctetSeq_var exported_incompatible_version_seq_encoded(
    codec->encode_value(exported_incompatible_version_seq_any));  
  CORBA::OctetSeq incompatible_version_chain;
  incompatible_version_chain.length(
    exported_incompatible_version_seq_encoded->length() + magic_tag_size);
  std::memcpy(incompatible_version_chain.get_buffer(),
              openbus::idl_data_export::MagicTag_CallChain,
              magic_tag_size);
  std::memcpy(incompatible_version_chain.get_buffer() + magic_tag_size,
              exported_incompatible_version_seq_encoded->get_buffer(),
              exported_incompatible_version_seq_encoded->length());
  try
  {
    bus_ctx->decodeChain(incompatible_version_chain);
  }
  catch (const openbus::InvalidEncodedStream &e)
  {
    if (e.what() != std::string("Versão de cadeia incompatível.")) std::abort();
    return 0;
  }
  std::abort();  
  return 0; //MSVC
}
