// -*- coding: iso-8859-1-unix -*-

#include "iop_codec.hpp"
#include <check.hpp>
#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char **argv)
{
  namespace cfg = openbus::tests::config;
  cfg::load_options(argc, argv);
  if (cfg::openbus_test_verbose)
  {
    openbus::log()->set_level(openbus::debug_level);
  }

  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  
  boost::shared_ptr<openbus::Connection> conn_A(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  conn_A->loginByPassword("A", "A", cfg::user_password_domain);
  
  boost::shared_ptr<openbus::Connection> conn_B(
    bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
  conn_B->loginByPassword("B", "B", cfg::user_password_domain);
  
  bus_ctx->setDefaultConnection(conn_A);
  
  openbus::CallerChain chain(
    bus_ctx->makeChainFor(conn_B->login()->entity.in()));

  CORBA::OctetSeq encoded_chain(bus_ctx->encodeChain(chain));

  const std::size_t magic_tag_size(4);
  CORBA::OctetSeq seq;
  seq.length(encoded_chain.length() - magic_tag_size);
  std::memcpy(seq.get_buffer(), encoded_chain.get_buffer() + magic_tag_size,
              seq.length());
  IOP::Codec_var codec(get_codec(bus_ctx));
  CORBA::Any_var any(codec->decode_value(
                       seq, openbus::idl::data_export::_tc_VersionedDataSeq));
  const openbus::idl::data_export::VersionedDataSeq *tmp;
  *any >>= tmp;
  openbus::idl::data_export::VersionedDataSeq
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
              openbus::idl::data_export::MagicTag_CallChain,
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
    if (e.what() != std::string("Versao de cadeia incompativel.")) std::abort();
    return 0;
  }
  std::abort();  
  return 0; //MSVC
}
