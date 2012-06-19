#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include <boost/thread.hpp>

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

struct BroadcasterImpl : virtual public POA_tecgraf::openbus::interop::delegation::Broadcaster
{
  BroadcasterImpl(openbus::Connection& c, delegation::Messenger_var messenger)
    : connection(c), messenger(messenger) {}

  void post(const char* message)
  {
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
    boost::unique_lock<boost::mutex> lock(mutex);
    std::string from(connection.getCallerChain()->callers()[0].entity);
    std::vector<std::string>::iterator iterator
      = std::find(subscribers.begin(), subscribers.end(), from);
    if(iterator == subscribers.end())
      subscribers.push_back(from);
  }

  void unsubscribe()
  {
    boost::unique_lock<boost::mutex> lock(mutex);
    std::string from(connection.getCallerChain()->callers()[0].entity);
    std::vector<std::string>::iterator iterator
      = std::find(subscribers.begin(), subscribers.end(), from);
    if(iterator != subscribers.end())
      subscribers.erase(iterator);
  }

  openbus::Connection& connection;
  delegation::Messenger_var messenger;
  std::vector<std::string> subscribers;
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

    conn->loginByPassword("broadcaster", "broadcaster");

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
      componentId.name = "Broadcaster";
      componentId.major_version = '1';
      componentId.minor_version = '0';
      componentId.patch_version = '0';
      componentId.platform_spec = "C++";
      scs::core::ComponentContext broadcaster_component(manager->orb(), componentId);
    
      BroadcasterImpl broadcaster_servant(*conn.get(), m);
      broadcaster_component.addFacet("broadcaster", delegation::_tc_Broadcaster->id(), &broadcaster_servant);
    
      openbus::idl_or::ServicePropertySeq props;
      props.length(1);
      openbus::idl_or::ServiceProperty property;
      props[0].name = "offer.domain";
      props[0].value = "Interoperability Tests";

      conn->offers()->registerService(broadcaster_component.getIComponent(), props);
      std::cout << "Broadcaster no ar" << std::endl;
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
