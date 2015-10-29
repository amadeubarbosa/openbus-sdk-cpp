// -*- coding: iso-8859-1-unix -*-

#include <util.hpp>
#include <config.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>

#include <tao/PortableServer/PortableServer.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/asio.hpp>
#pragma clang diagnostic pop
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <iostream>
#include <fstream>

const std::string entity("interop_sharedauth_cpp_client");

using namespace boost::interprocess;
namespace cfg = openbus::test::config;

int main(int argc, char** argv) {
  try 
  {
    cfg::load_options(argc, argv);
    if (cfg::openbus_test_verbose)
    {
      openbus::log().set_level(openbus::debug_level);
    }
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    boost::shared_ptr<openbus::Connection> conn
      (bus_ctx->connectByAddress(cfg::bus_host_name,
                                 cfg::bus_host_port));
    bus_ctx->setDefaultConnection(conn);
    
    conn->loginByPassword(entity, entity, cfg::user_password_domain);
    
    openbus::SharedAuthSecret secret(conn->startSharedAuth());
    CORBA::OctetSeq secret_seq(bus_ctx->encodeSharedAuthSecret(secret));

    const std::string secret_path(cfg::system_sharedauth);
    boost::filesystem::remove(secret_path);
    std::ofstream file(secret_path.c_str());
    assert(file);
    std::copy(secret_seq.get_buffer()
              , secret_seq.get_buffer() + secret_seq.length()
              , std::ostream_iterator<char>(file));
    file.flush();
  } 
  catch(const std::exception &e) 
  {
    std::cout << "[error (std::exception)] " << e.what() << std::endl;
    throw;
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    throw;
  } 
  catch (...) 
  {
    std::cout << "[error *unknow exception*]" << std::endl;
    throw;
  }
  return 0; //MSVC
}
