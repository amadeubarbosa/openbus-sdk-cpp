#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "stubs/messages.h"
#include <CORBA.h>

namespace delegation = tecgraf::openbus::interop::delegation;

#ifdef OPENBUS_SDK_MULTITHREAD
class RunThread : public MICOMT::Thread {
public:
  RunThread(openbus::ConnectionManager* m) : _manager(m) {}
  void _run(void*) { _manager->orb()->run(); }
private:
  openbus::ConnectionManager* _manager;
};
#endif

struct forwarding_thread
{
  forwarding_thread(const char* to, openbus::CallerChain const& caller_chain, delegation::Messenger_var messenger
                    , openbus::Connection& connection)
    : to(to), caller_chain(caller_chain), canceled(false), messenger(messenger)
    , connection(connection) {}
  std::string to;
  openbus::CallerChain caller_chain;
  boost::thread thread;
  boost::mutex mutex;
  boost::condition_variable c;
  bool canceled;
  delegation::Messenger_var messenger;
  openbus::Connection& connection;

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
          connection.joinChain(&caller_chain);
          delegation::PostDescSeq_var posts = messenger->receivePosts();
          std::cout << "Found " << posts->length() << " messages" << std::endl;
          connection.exitChain();
          for(std::size_t i = 0; i != posts->length(); ++i)
          {
            std::cout << "Has message" << std::endl;
            std::string msg("forwarded from ");
            msg += posts[i].from;
            msg += ':';
            msg += posts[i].message;
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
  ForwarderImpl(openbus::Connection& c, delegation::Messenger_var messenger)
    : connection(c), messenger(messenger) {}

  std::map<std::string, boost::shared_ptr<forwarding_thread> > threads;

  void setForward(const char* to)
  {
    boost::unique_lock<boost::mutex> lock(mutex);
    std::string from(connection.getCallerChain()->caller().entity);
    std::cout << "setup forward to " << to << " by " << from << std::endl;
    std::map<std::string, boost::shared_ptr<forwarding_thread> >::const_iterator
      iterator = threads.find(from);
    if(iterator == threads.end())
    {
      boost::shared_ptr<forwarding_thread> t
        (new forwarding_thread(to, *connection.getCallerChain(), messenger, connection));
      threads.insert(std::make_pair(from, t));
      t->thread = boost::thread(boost::bind(&forwarding_thread::run, t));
    }
    else
      std::cout << "Already setup to " << to << std::endl;
  }

  void cancelForward(const char* to)
  {
    boost::unique_lock<boost::mutex> lock(mutex);
    std::string from(connection.getCallerChain()->caller().entity);
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
  }

  openbus::Connection& connection;
  delegation::Messenger_var messenger;
  boost::mutex mutex;
};

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::info_level);
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    manager->setDefaultConnection(conn.get());
    
    #ifdef OPENBUS_SDK_MULTITHREAD
    RunThread* runThread = new RunThread(manager);
    runThread->start();
    #endif

    conn->loginByPassword("forwarder", "forwarder");

    openbus::idl_or::ServicePropertySeq properties;
    properties.length(2);
    properties[0].name  = "offer.domain";
    properties[0].value = "Interoperability Tests";
    properties[1].name  = "openbus.component.interface";
    properties[1].value = tecgraf::openbus::interop::delegation::_tc_Messenger->id();
    openbus::idl_or::ServiceOfferDescSeq_var offers = conn->offers()->findServices(properties);
    
    if (offers->length() > 0)
    {
      CORBA::Object_var o = offers[static_cast<CORBA::ULong> (0)].service_ref->getFacetByName("messenger");
      tecgraf::openbus::interop::delegation::Messenger_var
        m = tecgraf::openbus::interop::delegation::Messenger::_narrow(o);

      scs::core::ComponentId componentId;
      componentId.name = "Forwarder";
      componentId.major_version = '1';
      componentId.minor_version = '0';
      componentId.patch_version = '0';
      componentId.platform_spec = "C++";
      scs::core::ComponentContext forwarder_component(manager->orb(), componentId);
    
      ForwarderImpl forwarder_servant(*conn.get(), m);
      forwarder_component.addFacet("forwarder", tecgraf::openbus::interop::delegation::_tc_Forwarder->id(), &forwarder_servant);
    
      openbus::idl_or::ServicePropertySeq props;
      props.length(1);
      openbus::idl_or::ServiceProperty property;
      props[0].name = "offer.domain";
      props[0].value = "Interoperability Tests";

      conn->offers()->registerService(forwarder_component.getIComponent(), props);
      std::cout << "Forwarder no ar" << std::endl;
      runThread->wait();
    }
    else
    {
      std::cout << "Couldn't find messenger" << std::endl;
      return -1;
    }
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
