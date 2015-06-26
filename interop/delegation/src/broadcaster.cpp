// -*- coding: iso-8859-1-unix -*-

#include "messagesS.h"
#include <util.hpp>
#include <openbus.hpp>
#include <scs/ComponentContext.h>
#include <log/output/file_output.h>

#include <iostream>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

namespace delegation = tecgraf::openbus::interop::delegation;

#ifdef OPENBUS_SDK_MULTITHREAD
void ORBRun(CORBA::ORB_var orb)
{
 orb->run();
}
#endif

struct BroadcasterImpl : 
  virtual public POA_tecgraf::openbus::interop::delegation::Broadcaster
{
  BroadcasterImpl(openbus::OpenBusContext& c,
		  delegation::Messenger_var messenger)
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
    openbus::log().set_level(openbus::debug_level);
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    std::auto_ptr <openbus::Connection>
      conn(bus_ctx->connectByAddress(bus_host, bus_port));
    bus_ctx->setDefaultConnection(conn.get());
    
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread orbRun(ORBRun, bus_ctx->orb());
#endif

    conn->loginByCertificate(entity, openbus::PrivateKey(private_key));    

    openbus::idl_or::ServicePropertySeq properties;
    properties.length(2);
    properties[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    properties[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    properties[static_cast<CORBA::ULong>(1)].name  =
      "openbus.component.interface";
    properties[static_cast<CORBA::ULong>(1)].value =
      delegation::_tc_Messenger->id();
    openbus::idl_or::ServiceOfferDescSeq_var offers = 
      find_offers(bus_ctx, properties);
    
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
      scs::core::ComponentContext
	broadcaster_component(bus_ctx->orb(), componentId);
    
      BroadcasterImpl broadcaster_servant(*bus_ctx, m);
      broadcaster_component.addFacet(
        "broadcaster", delegation::_tc_Broadcaster->id(), &broadcaster_servant);
    
      openbus::idl_or::ServicePropertySeq props;
      props.length(1);
      openbus::idl_or::ServiceProperty property;
      props[static_cast<CORBA::ULong>(0)].name = "offer.domain";
      props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";

      bus_ctx->getOfferRegistry()->registerService(
      broadcaster_component.getIComponent(), props);
      std::cout << "Broadcaster no ar" << std::endl;

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
  return 0; //MSVC
}
