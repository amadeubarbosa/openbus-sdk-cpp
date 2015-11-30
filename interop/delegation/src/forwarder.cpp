// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "messagesS.h"
#pragma clang diagnostic pop
#include <util.hpp>
#include <tests/config.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>
#include <scs/ComponentContext.h>
#include <log/output/file_output.h>

#include <iostream>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include <boost/asio.hpp>
#pragma clang diagnostic pop

namespace delegation = tecgraf::openbus::interop::delegation;
namespace cfg = openbus::tests::config;

void ORBRun(CORBA::ORB_ptr orb)
{
 orb->run();
}

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
      boost::unique_lock<boost::mutex> lock(mutex);
      while(!canceled)
      {
        c.timed_wait(lock, boost::posix_time::millisec(
                       (cfg::login_lease_time*1000)/2));
        if(!canceled)
        {
          std::string from (caller_chain.caller().entity);
          std::cout << "Checking messages of " << from << std::endl;
          ctx.joinChain(caller_chain);
          delegation::PostDescSeq_var posts = messenger->receivePosts();
          std::cout << "Found " << posts->length() << " messages" << std::endl;
          ctx.exitChain();
          for(CORBA::ULong i(0); i != posts->length(); ++i)
          {
            std::cout << "Has message" << std::endl;
            std::string msg("forwarded message by ");
            msg += (*posts)[i].from;
            msg += ": ";
            msg += (*posts)[i].message;
            messenger->post(to.c_str(), msg.c_str());
            std::cout << "message: " << msg << std::endl;
          }
        }
      }
    }
    catch(CORBA::COMM_FAILURE const&)
    {
      std::cerr << "Communication failure while contacting messenger (COMM_FAILURE)" << std::endl;
      std::exit(-1);
    }
    catch(CORBA::OBJECT_NOT_EXIST const&)
    {
      std::cerr << "Communication failure while contacting messenger (OBJECT_NOT_EXISTS)" << std::endl;
      std::exit(-1);
    }
    catch(CORBA::TRANSIENT const&)
    {
      std::cerr << "Communication failure while contacting messenger (TRANSIENT)" << std::endl;
      std::exit(-1);
    }
    catch(std::exception const& e)
    {
      std::cerr << "A C++ exception was thrown of type " << typeid(e).name()
                << " with what: " << e.what() << std::endl;
      std::exit(-1);
    }
  }
};

struct ForwarderImpl :
  virtual public POA_tecgraf::openbus::interop::delegation::Forwarder {
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
    return 0; // To shut up MSVC
  }

  openbus::OpenBusContext& ctx;
  delegation::Messenger_var messenger;
  boost::mutex mutex;
};

const std::string entity("interop_delegation_cpp_forwarder");

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
    boost::shared_ptr<openbus::Connection>
      conn(bus_ctx->connectByAddress(cfg::bus_host_name, cfg::bus_host_port));
    bus_ctx->setDefaultConnection(conn);
    
    boost::thread orb_run(ORBRun, bus_ctx->orb());

    EVP_PKEY *priv_key(
      openbus::demo::openssl::read_priv_key(cfg::system_private_key));
    if (!priv_key)
    {
      std::cerr << "Chave privada invalida." << std::endl;
      return -1;
    }
    conn->loginByCertificate(entity, priv_key);

    openbus::idl::offers::ServicePropertySeq props;
    props.length(2);
    props[static_cast<CORBA::ULong>(0)].name  = "offer.domain";
    props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";
    props[static_cast<CORBA::ULong>(1)].name  = "openbus.component.interface";
    props[static_cast<CORBA::ULong>(1)].value = delegation::_tc_Messenger->id();
    openbus::idl::offers::ServiceOfferDescSeq_var offers = 
      find_offers(bus_ctx, props);
    
    if (offers->length() > 0)
    {
      CORBA::Object_var o = offers[static_cast<CORBA::ULong>(0)]
        .service_ref->getFacet(delegation::_tc_Messenger->id());
      delegation::Messenger_var m = delegation::Messenger::_narrow(o);

      scs::core::ComponentId componentId;
      componentId.name = "Forwarder";
      componentId.major_version = '1';
      componentId.minor_version = '0';
      componentId.patch_version = '0';
      componentId.platform_spec = "C++";
      scs::core::ComponentContext
	forwarder_component(bus_ctx->orb(), componentId);
    
      ForwarderImpl forwarder_servant(*bus_ctx, m);
      forwarder_component.addFacet(
        "forwarder", delegation::_tc_Forwarder->id(), &forwarder_servant);
    
      openbus::idl::offers::ServicePropertySeq props;
      props.length(1);
      props[static_cast<CORBA::ULong>(0)].name = "offer.domain";
      props[static_cast<CORBA::ULong>(0)].value = "Interoperability Tests";

      bus_ctx->getOfferRegistry()->registerService(
        forwarder_component.getIComponent(), props);
      std::cout << "Forwarder no ar" << std::endl;

      orb_run.join();
    }
    else
    {
      std::cerr << "Couldn't find messenger" << std::endl;
      return -1;
    }
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
