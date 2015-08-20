// -*- coding: iso-8859-1-unix -*-

#include "helloC.h"
#include <util.hpp>
#include <demo/openssl.hpp>
#include "encodingC.h"
#include <openbus.hpp>

#include <tao/PortableServer/PortableServer.h>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <iostream>
#include <fstream>

namespace sharedauth = tecgraf::openbus::interop::sharedauth;

const std::string entity("interop_sharedauth_cpp_sharedauth");
std::string priv_key_filename, bus_host, tmp;
unsigned short bus_port;

using namespace boost::interprocess;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("private-key", po::value<std::string>()->default_value(entity + ".key"),
     "Path to private key")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus")
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
  if (vm.count("bus.host.name"))
  {
    bus_host = vm["bus.host.name"].as<std::string>();
  }
  if (vm.count("bus.host.port"))
  {
    bus_port = vm["bus.host.port"].as<unsigned short>();
  }
  if (vm.count("private-key"))
  {
    priv_key_filename = vm["private-key"].as<std::string>();
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
    openbus::log().set_level(openbus::debug_level);
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    std::auto_ptr <openbus::Connection> conn
      (bus_ctx->connectByAddress(bus_host, bus_port));

    bus_ctx->setDefaultConnection(conn.get());
    EVP_PKEY *priv_key(
      openbus::demo::openssl::read_priv_key(priv_key_filename));
    if (!priv_key)
    {
      std::cerr << "Chave privada inválida." << std::endl;
      std::abort();
    }
    conn->loginByCertificate(entity, priv_key);
    
    openbus::SharedAuthSecret secret(conn->startSharedAuth());
    CORBA::OctetSeq secret_seq(bus_ctx->encodeSharedAuthSecret(secret));

    {
      const std::string secret_path(tmp + "/.secret");
      const std::string lock_path(tmp + "/.secret.lock");
      boost::filesystem::remove(secret_path);
      std::ofstream(lock_path.c_str());
      file_lock flock(lock_path.c_str());
      flock.lock();
      std::ofstream file(secret_path.c_str());
      assert(file);
      std::copy(secret_seq.get_buffer()
		, secret_seq.get_buffer() + secret_seq.length()
		, std::ostream_iterator<char>(file));
      file.flush();
      flock.unlock();
    }
    
    std::cout << "Chamando a faceta Hello por este cliente." << std::endl;

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "openbus.component.facet";
    props[static_cast<CORBA::ULong>(0)].value = "Hello";
    props[static_cast<CORBA::ULong>(1)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(1)].value = "Interoperability Tests";

    openbus::idl_or::ServiceOfferDescSeq_var offers(
      find_offers(bus_ctx, props));
    if (offers->length())
    {
      CORBA::Object_var o = offers[static_cast<CORBA::ULong>(0)]
        .service_ref->getFacetByName("Hello");
      tecgraf::openbus::interop::simple::Hello *hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      CORBA::String_var ret(hello->sayHello());
      std::string msg("Hello " + entity + "!");
      if (!(msg == std::string(ret.in())))
      {
        std::cerr << "sayHello() não retornou a string '"
          + msg + "'." << std::endl;
        std::abort();
      }
    } 
    else 
    {
      std::cout << "nenhuma oferta encontrada." << std::endl;
    }

    bus_ctx->orb()->run();
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
