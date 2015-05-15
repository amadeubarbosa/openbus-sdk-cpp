// -*- coding: iso-8859-1-unix -*-

#include "helloC.h"
#include <util.hpp>
#include "encodingC.h"
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/interprocess/sync/file_lock.hpp>

namespace sharedauth = tecgraf::openbus::interop::sharedauth;

const std::string client_entity("interop_sharedauth_cpp_sharedauth");
std::string bus_host;
unsigned short bus_port;

using namespace boost::interprocess;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus");
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
      (bus_ctx->createConnection(bus_host, bus_port));

    struct secret_ctx
    {
      secret_ctx(const std::string &path)
        : path(path)
      {
        do
        {
          try
          {
            flock = file_lock(path.c_str());
            break;
          }
          catch (const interprocess_exception &)
          {
            sleep(1);
          }          
        } while(true);
        flock.lock();
        file.open(path);
        file.seekg(0, std::ios::end);
        secret_seq.length(static_cast<CORBA::ULong>(file.tellg()));
        file.seekg(0, std::ios::beg);
        file.rdbuf()->sgetn
          (static_cast<char*>(static_cast<void*>(secret_seq.get_buffer()))
           , secret_seq.length());
        flock.unlock();
      }
      ~secret_ctx()
      {
        try
        {
          flock.unlock();
        }
        catch (...)
        {
        }
      } 
      const std::string path;
      std::ifstream file;
      CORBA::OctetSeq secret_seq;
      file_lock flock;
    };

    secret_ctx secret_file(".secret");
    openbus::SharedAuthSecret
      secret(bus_ctx->decodeSharedAuthSecret(secret_file.secret_seq));
    conn->loginBySharedAuth(secret);

    bus_ctx->setDefaultConnection(conn.get());

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[0].name  = "openbus.component.facet";
    props[0].value = "Hello";
    props[1].name  = "offer.domain";
    props[1].value = "Interoperability Tests";
    openbus::idl_or::ServiceOfferDescSeq_var offers(
      find_offers(bus_ctx, props));
    
    if (offers->length())
    {
      CORBA::Object_var o = offers[static_cast<CORBA::ULong>(0)]
        .service_ref->getFacetByName("Hello");
      tecgraf::openbus::interop::simple::Hello_var hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      CORBA::String_var ret(hello->sayHello());
      std::string msg("Hello " + client_entity + "!");
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
