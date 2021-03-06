// -*- coding: iso-8859-1-unix -*-

#include <util.hpp>
#include <tests/config.hpp>
#include "encodingC.h"
#include <openbus.hpp>

#include <iostream>
#include <fstream>

const std::string client_entity("interop_sharedauth_cpp_client");

namespace cfg = openbus::tests::config;

int main(int argc, char** argv) {
  try 
  {
    cfg::load_options(argc, argv);
    if (cfg::openbus_test_verbose)
    {
      openbus::log()->set_level(openbus::debug_level);
    }
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    CORBA::Object_var bus_ref(cfg::get_bus_ref(orb_ctx->orb()));
    boost::shared_ptr<openbus::Connection> conn(
      bus_ctx->connectByReference(bus_ref));

    CORBA::OctetSeq secret_seq;  
    std::fstream file(std::string(cfg::system_sharedauth).c_str());
    file.seekg(0, std::ios::end);
    secret_seq.length(static_cast<CORBA::ULong>(file.tellg()));
    file.seekg(0, std::ios::beg);
    file.rdbuf()->sgetn
      (static_cast<char*>(static_cast<void*>(secret_seq.get_buffer()))
       , secret_seq.length());

    openbus::SharedAuthSecret
      secret(bus_ctx->decodeSharedAuthSecret(secret_seq));

    conn->loginBySharedAuth(secret);

    bus_ctx->setDefaultConnection(conn);
  } 
  catch(const std::exception &e) 
  {
    std::cerr << "[error (std::exception)] " << e.what() << std::endl;
    return -1;
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cerr << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } 
  catch (...) 
  {
    std::cerr << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0; //MSVC
}
