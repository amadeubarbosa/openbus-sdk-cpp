// -*- coding: iso-8859-1-unix -*-

#include "stubs/messages.h"
#include <util.hpp>
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>
#include <scs/ComponentContext.hpp>
#include <log/output/file_output.h>

#include <CORBA.h>
#include <iostream>
#include <map>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/program_options.hpp>
#include <boost/asio.hpp>

namespace delegation = tecgraf::openbus::interop::delegation;

boost::asio::io_service io_service;

struct MessengerImpl : 
  virtual public POA_tecgraf::openbus::interop::delegation::Messenger
{
  MessengerImpl(openbus::OpenBusContext& c)
    : ctx(c) {}

  void post(const char* to, const char* message) 
  {
    openbus::CallerChain chain(ctx.getCallerChain());
    std::string from;
    for(CORBA::ULong i(0); i < chain.originators().length(); ++i)
    {
      from = from + chain.originators()[i].entity.in() + "->";
    }
    from += chain.caller().entity.in();
    std::cout << "Post to " << to << " by " << from << std::endl;
    std::cout << " Message content: " << message << std::endl;
    delegation::PostDesc desc = {from.c_str(), message};
    inbox.insert(std::make_pair(to, desc));
  }

  delegation::PostDescSeq* receivePosts()
  {
    openbus::CallerChain chain(ctx.getCallerChain());
    std::string owner(chain.caller().entity);
    if (chain.originators().length() > 0)
    {
      owner = chain.originators()[0].entity;
    } 
    std::string by;
    for(CORBA::ULong i(0); i < chain.originators().length(); ++i)
    {
      by = by + chain.originators()[i].entity.in() + "->";
    }
    by += chain.caller().entity.in();
    std::cout << "Retrieving messages of " << owner
              << " by " << by
              << std::endl;
    typedef std::multimap<std::string, delegation::PostDesc>::iterator
      iterator;
    std::pair<iterator, iterator> range = inbox.equal_range(owner);
    delegation::PostDescSeq_var posts (new delegation::PostDescSeq);
    std::cout << "Retrieving " << std::distance(range.first, range.second) 
              << " messages" << std::endl;
    posts->length(std::distance(range.first, range.second));
    std::size_t index = 0;
    for(iterator first = range.first; first != range.second; ++first, ++index)
    {
      (*posts)[index] = first->second;
    }
    inbox.erase(range.first, range.second);
    return posts._retn();
  }

  openbus::OpenBusContext& ctx;
  std::multimap<std::string, delegation::PostDesc> inbox;
};

const std::string entity("interop_delegation_cpp_messenger");
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

#if 0
    openbus::log().set_level(openbus::debug_level);
#endif
    openbus::OpenBusContext *const ctx(get_bus_ctx(argc, argv));

    std::auto_ptr <openbus::Connection> conn(ctx->createConnection(bus_host,
                                                                   bus_port));
    ctx->setDefaultConnection(conn.get());

#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread orb_run(ORBRun, ctx->orb());
#endif

    scs::core::ComponentId componentId;
    componentId.name = "Messenger";
    componentId.major_version = 1;
    componentId.minor_version = 0;
    componentId.patch_version = 0;
    componentId.platform_spec = "C++";
    scs::core::ComponentContext messenger_component(ctx->orb(), componentId);
    MessengerImpl messenger_servant(*ctx);
    messenger_component.addFacet(
      "messenger", delegation::_tc_Messenger->id(), &messenger_servant);

    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    props[0u].name = "offer.domain";
    props[0u].value = "Interoperability Tests";

    conn->loginByCertificate(entity, openbus::PrivateKey(private_key));

    ctx->getOfferRegistry()->registerService(
      messenger_component.getIComponent(), props);
    std::cout << "Messenger no ar" << std::endl;
    boost::asio::signal_set signals(io_service, SIGINT, SIGTERM);

    handler io_handler(ctx->orb(), conn.get(), &io_service);
    signals.async_wait(io_handler);
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread io_service_run(
      boost::bind(&boost::asio::io_service::run, &io_service));
#endif

#ifdef OPENBUS_SDK_MULTITHREAD
    orb_run.join();
#endif
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
