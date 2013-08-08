// -*- coding: iso-8859-1-unix -*-

#include "stubs/messages.h"
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>
#include <scs/ComponentContext.h>
#include <log/output/file_output.h>

#include <CORBA.h>
#include <iostream>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

namespace delegation = tecgraf::openbus::interop::delegation;

#ifdef OPENBUS_SDK_MULTITHREAD
void ORBRun(CORBA::ORB_ptr orb)
{
 orb->run();
}
#endif

struct forwarding_thread
{
  forwarding_thread(const char* to, openbus::CallerChain const& caller_chain, delegation::Messenger_var messenger
                    , openbus::OpenBusContext& c)
    : to(to), caller_chain(caller_chain), canceled(false), messenger(messenger)
    , ctx(c) {}
  std::string to;
  openbus::CallerChain caller_chain;
  boost::thread thread;
  boost::mutex mutex;
  boost::condition_variable c;
  bool canceled;
  delegation::Messenger_var messenger;
  openbus::OpenBusContext& ctx;

  void run()
  {
    try
    {
      std::cout << "thread run" << std::endl;
      boost::unique_lock<boost::mutex> lock(mutex);
      while(!canceled)
      {
        c.timed_wait(lock, boost::posix_time::seconds(5));
        if(!canceled)
        {
          std::string from (caller_chain.caller().entity);
          std::cout << "Checking messages of " << from << std::endl;
          ctx.joinChain(caller_chain);
          delegation::PostDescSeq_var posts = messenger->receivePosts();
          std::cout << "Found " << posts->length() << " messages" << std::endl;
          ctx.exitChain();
          for(std::size_t i = 0; i != posts->length(); ++i)
          {
            std::cout << "Has message" << std::endl;
            std::string msg("forwarded from ");
            msg += (*posts)[i].from;
            msg += ':';
            msg += (*posts)[i].message;
            messenger->post(from.c_str(), msg.c_str());
          }
        }
      }
    }
    catch(CORBA::COMM_FAILURE const&)
    {
      std::cout << "Communication failure while contacting messenger (COMM_FAILURE)" << std::endl;
      std::abort();
    }
    catch(CORBA::OBJECT_NOT_EXIST const&)
    {
      std::cout << "Communication failure while contacting messenger (OBJECT_NOT_EXISTS)" << std::endl;
      std::abort();
    }
    catch(CORBA::TRANSIENT const&)
    {
      std::cout << "Communication failure while contacting messenger (TRANSIENT)" << std::endl;
      std::abort();
    }
    catch(std::exception const& e)
    {
      std::cout << "A C++ exception was thrown of type " << typeid(e).name()
                << " with what: " << e.what() << std::endl;
      std::abort();
    }
  }
};

struct ForwarderImpl : virtual public POA_tecgraf::openbus::interop::delegation::Forwarder {
  ForwarderImpl(openbus::OpenBusContext& c, delegation::Messenger_var messenger)
    : ctx(c), messenger(messenger) {}

  std::map<std::string, boost::shared_ptr<forwarding_thread> > threads;

  void setForward(const char* to)
  {
    boost::unique_lock<boost::mutex> lock(mutex);
    std::string from(ctx.getCallerChain().caller().entity);
    std::cout << "setup forward to " << to << " by " << from << std::endl;
    std::map<std::string, boost::shared_ptr<forwarding_thread> >::const_iterator
      iterator = threads.find(from);
    if(iterator == threads.end())
    {
      boost::shared_ptr<forwarding_thread> t
        (new forwarding_thread(to, ctx.getCallerChain(), messenger, ctx));
      threads.insert(std::make_pair(from, t));
      t->thread = boost::thread(boost::bind(&forwarding_thread::run, t));
    }
    else
      std::cout << "Already setup to " << to << std::endl;
  }

  void cancelForward(const char* to)
  {
    boost::unique_lock<boost::mutex> lock(mutex);
    std::string from(ctx.getCallerChain().caller().entity);
    std::cout << "cancel forward to " << to << " by " << from << std::endl;
    std::map<std::string, boost::shared_ptr<forwarding_thread> >::iterator
      iterator = threads.find(from);
    if(iterator != threads.end())
    {
      boost::shared_ptr<forwarding_thread> thread = iterator->second;
      lock.unlock();

      {
        boost::unique_lock<boost::mutex> thred_lock(thread->mutex);
        thread->canceled = true;
        thread->c.notify_one();
      }

      lock.lock();
      iterator = threads.find(from);
      if(iterator != threads.end())
        threads.erase(iterator);
    }
  }

  char* getForward()
  {
    boost::unique_lock<boost::mutex> lock(mutex);
    std::abort();
    return 0; // To shut up MSVC
  }

  openbus::OpenBusContext& ctx;
  delegation::Messenger_var messenger;
  boost::mutex mutex;
};

const std::string entity("interop_delegation_cpp_forwarder");
std::string private_key;
std::string bus_host;
unsigned short bus_port;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("private-key", po::value<std::string>()->default_value("admin/" + entity
                                                            + ".key"),
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
    boost::thread orb_run(ORBRun, ctx->orb());
#endif

    conn->loginByCertificate(entity, openbus::PrivateKey(private_key));

    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name  = "openbus.component.interface";
    props[static_cast<CORBA::ULong>(1)].value = delegation::_tc_Messenger->id();
    openbus::idl_or::ServiceOfferDescSeq_var offers = 
      ctx->getOfferRegistry()->findServices(props);
    
    if (offers->length() > 0)
    {
      CORBA::Object_var o = offers[static_cast<CORBA::ULong>(0)]
        .service_ref->getFacetByName("messenger");
      delegation::Messenger_var m = delegation::Messenger::_narrow(o);

      scs::core::ComponentId componentId;
      componentId.name = "Forwarder";
      componentId.major_version = '1';
      componentId.minor_version = '0';
      componentId.patch_version = '0';
      componentId.platform_spec = "C++";
      scs::core::ComponentContext forwarder_component(ctx->orb(), componentId);
    
      ForwarderImpl forwarder_servant(*ctx, m);
      forwarder_component.addFacet(
        "forwarder", delegation::_tc_Forwarder->id(), &forwarder_servant);
    
      openbus::idl_or::ServicePropertySeq props;
      props.length(1);
      props[static_cast<CORBA::ULong>(0)].name = "offer.domain";
      props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";

      ctx->getOfferRegistry()->registerService(
        forwarder_component.getIComponent(), props);
      std::cout << "Forwarder no ar" << std::endl;
#ifdef OPENBUS_SDK_MULTITHREAD
      orb_run.join();
#endif
    }
    else
    {
      std::cout << "Couldn't find messenger" << std::endl;
      return -1;
    }
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
