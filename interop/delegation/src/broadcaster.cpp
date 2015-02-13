// -*- coding: iso-8859-1-unix -*-

#include "messagesS.h"
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>
#include <scs/ComponentContext.h>
#include <log/output/file_output.h>

#include <iostream>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

namespace delegation = tecgraf::openbus::interop::delegation;

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

#ifdef OPENBUS_SDK_MULTITHREAD
void ORBRun(CORBA::ORB_var orb)
{
 orb->run();
}
#endif

struct BroadcasterImpl : 
  virtual public POA_tecgraf::openbus::interop::delegation::Broadcaster
{
  BroadcasterImpl(openbus::OpenBusContext& c, delegation::Messenger_var messenger)
    : ctx(c), messenger(messenger) {}

  void post(const char* message)
  {
    ctx.joinChain();
    boost::unique_lock<boost::mutex> lock(mutex);
    for(std::vector<std::string>::const_iterator
          first = subscribers.begin(), last = subscribers.end()
          ;first != last; ++first)
    {
      messenger->post(first->c_str(), message);
    }
  }

  void subscribe()
  {
    std::string from(ctx.getCallerChain().caller().entity);
    boost::unique_lock<boost::mutex> lock(mutex);
    std::vector<std::string>::iterator iterator
      = std::find(subscribers.begin(), subscribers.end(), from);
    if(iterator == subscribers.end())
      subscribers.push_back(from);
  }

  void unsubscribe()
  {
    boost::unique_lock<boost::mutex> lock(mutex);
    std::string from(ctx.getCallerChain().caller().entity);
    std::vector<std::string>::iterator iterator
      = std::find(subscribers.begin(), subscribers.end(), from);
    if(iterator != subscribers.end())
      subscribers.erase(iterator);
  }

  openbus::OpenBusContext& ctx;
  delegation::Messenger_var messenger;
  std::vector<std::string> subscribers;
  boost::mutex mutex;
};

const std::string entity("interop_delegation_cpp_broadcaster");
std::string private_key;
std::string bus_host;
unsigned short bus_port;

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
  try {
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
    std::auto_ptr <openbus::Connection> conn(ctx->createConnection(bus_host, bus_port));
    ctx->setDefaultConnection(conn.get());
    
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread orbRun(ORBRun, ctx->orb());
#endif

    conn->loginByCertificate(entity, openbus::PrivateKey(private_key));    

    openbus::idl_or::ServicePropertySeq properties;
    properties.length(2);
    properties[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    properties[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    properties[static_cast<CORBA::ULong>(1)].name  = "openbus.component.interface";
    properties[static_cast<CORBA::ULong>(1)].value = delegation::_tc_Messenger->id();
    openbus::idl_or::ServiceOfferDescSeq_var offers = 
      ctx->getOfferRegistry()->findServices(properties);
    
    if (offers->length() > 0)
    {
      CORBA::Object_var o = offers[static_cast<CORBA::ULong> (0)]
        .service_ref->getFacetByName("messenger");
      delegation::Messenger_var m = delegation::Messenger::_narrow(o);

      scs::core::ComponentId componentId;
      componentId.name = "Broadcaster";
      componentId.major_version = '1';
      componentId.minor_version = '0';
      componentId.patch_version = '0';
      componentId.platform_spec = "C++";
      scs::core::ComponentContext broadcaster_component(ctx->orb(), componentId);
    
      BroadcasterImpl broadcaster_servant(*ctx, m);
      broadcaster_component.addFacet(
        "broadcaster", delegation::_tc_Broadcaster->id(), &broadcaster_servant);
    
      openbus::idl_or::ServicePropertySeq props;
      props.length(1);
      openbus::idl_or::ServiceProperty property;
      props[static_cast<CORBA::ULong>(0)].name = "offer.domain";
      props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";

      ctx->getOfferRegistry()->registerService(
      broadcaster_component.getIComponent(), props);
      std::cout << "Broadcaster no ar" << std::endl;

      boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);
      handler io_handler(orb, conn.get());
      signals.async_wait(io_handler);
#ifdef OPENBUS_SDK_MULTITHREAD
      boost::thread io_service_run(
        boost::bind(&boost::asio::io_service::run, &io_service));
#endif

#ifdef OPENBUS_SDK_MULTITHREAD
      orbRun.join();
#endif
    }
    else
    {
      std::cout << "Couldn't find messenger" << std::endl;
      return -1;
    }
  } 
  catch(std::exception const& e) 
  {
    std::cout << "[error (std::exception)] " << e.what() << std::endl;
    return -1;
  } 
  catch (const CORBA::Exception& e) 
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
