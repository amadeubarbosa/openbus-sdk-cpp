// -*- coding: iso-8859-1-unix -*-

#include <util.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>

#include <tao/PortableServer/PortableServer.h>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <iostream>
#include <fstream>

const std::string entity("interop_sharedauth_cpp_client");
std::string bus_host, tmp, domain;
unsigned short bus_port;
bool debug;

using namespace boost::interprocess;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("debug", po::value<bool>()->default_value(true) , "yes|no")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus")
    ("user.password.domain", po::value<std::string>()->default_value("testing"),
     "Password domain")
    ("tmp", po::value<std::string>()->default_value("/tmp"),
     "Temporary folder");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  if (vm.count("help")) 
  {
    std::cout << desc << std::endl;
    std::exit(1);
  }
  if (vm.count("debug"))
  {
    debug = vm["debug"].as<bool>();
  }
  if (vm.count("bus.host.name"))
  {
    bus_host = vm["bus.host.name"].as<std::string>();
  }
  if (vm.count("bus.host.port"))
  {
    bus_port = vm["bus.host.port"].as<unsigned short>();
  }
  if (vm.count("user.password.domain"))
  {
    domain = vm["user.password.domain"].as<std::string>();
  }
  if (vm.count("tmp"))
  {
    tmp = vm["tmp"].as<std::string>();
  }
}

int main(int argc, char** argv) {
  try 
  {
    load_options(argc, argv);
    if (debug)
    {
      openbus::log().set_level(openbus::debug_level);
    }
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    std::auto_ptr <openbus::Connection> conn
      (bus_ctx->connectByAddress(bus_host, bus_port));
    bus_ctx->setDefaultConnection(conn.get());
    
    conn->loginByPassword(entity, entity, domain);
    
    openbus::SharedAuthSecret secret(conn->startSharedAuth());
    CORBA::OctetSeq secret_seq(bus_ctx->encodeSharedAuthSecret(secret));

    const std::string secret_path(tmp + "/.secret");
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
