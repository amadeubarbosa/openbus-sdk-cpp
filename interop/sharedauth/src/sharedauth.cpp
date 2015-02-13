// -*- coding: iso-8859-1-unix -*-

#include "helloC.h"
#include "encodingC.h"
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>

#include <tao/PortableServer/PortableServer.h>
#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

namespace sharedauth = tecgraf::openbus::interop::sharedauth;

const std::string entity("interop_sharedauth_cpp_sharedauth");
std::string private_key;
std::string bus_host;
unsigned short bus_port;
boost::asio::io_service io_service;

struct handler
{
  handler(
    CORBA::ORB_var orb,
    openbus::Connection *conn)
    : orb(orb), conn(conn)
  {
  }

  handler(const handler& o)
  {
    orb = o.orb;
    conn = o.conn;
  }

  void operator()(
    const boost::system::error_code& error,
    int signal_number)
  {
    if (!error)
    {
      conn->logout();
      orb->shutdown(true);        
      io_service.stop();
    }
  }

  CORBA::ORB_var orb;
  openbus::Connection *conn;
};

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
  if (vm.count("private-key"))
  {
    private_key = vm["private-key"].as<std::string>();
  }
}

int main(int argc, char** argv) {
  try 
  {
    load_options(argc, argv);
    // openbus::log().set_level(openbus::debug_level);

    CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();

    openbus::OpenBusContext *const ctx = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr <openbus::Connection> conn
      (ctx->createConnection(bus_host, bus_port));

    ctx->setDefaultConnection(conn.get());
    try 
    {
      conn->loginByCertificate(entity, openbus::PrivateKey(private_key));
    }
    catch(const openbus::InvalidPrivateKey &e)
    {
      std::cout << e.what() << std::endl;
    }

    {
      openbus::SharedAuthSecret secret(conn->startSharedAuth());
      CORBA::OctetSeq secret_seq(ctx->encodeSharedAuthSecret(secret));

      std::ofstream file(".secret");
      std::copy(secret_seq.get_buffer()
                , secret_seq.get_buffer() + secret_seq.length()
                , std::ostream_iterator<char>(file));
    }

    std::cout << "Chamando a faceta Hello por este cliente." << std::endl;

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "openbus.component.facet";
    props[static_cast<CORBA::ULong>(0)].value = "Hello";
    props[static_cast<CORBA::ULong>(1)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(1)].value = "Interoperability Tests";

    openbus::idl_or::ServiceOfferDescSeq_var offers = 
      ctx->getOfferRegistry()->findServices(props);
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

    boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
    handler io_handler(orb, conn.get());
    signals.async_wait(io_handler);
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread io_service_run(
      boost::bind(&boost::asio::io_service::run, &io_service));
#endif

    ctx->orb()->run();
  } 
  catch(const std::exception &e) 
  {
    std::cout << "[error (std::exception)] " << e.what() << std::endl;
    return -1;
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } 
  catch (...) 
  {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;    
  }
}
