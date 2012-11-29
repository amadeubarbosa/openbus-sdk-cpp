#include <openbus/ORBInitializer.h>
#include <openbus/log.h>
#include <openbus/OpenBusContext.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include <boost/thread.hpp>

#include "stubs/messages.h"
#include <CORBA.h>
#include "server_login.h"
#include "properties_reader.h"
#include <log/output/file_output.h>

namespace delegation = tecgraf::openbus::interop::delegation;

#ifdef OPENBUS_SDK_MULTITHREAD
class RunThread : public MICOMT::Thread {
public:
  RunThread(openbus::OpenBusContext* m) : _openbusContext(m) {}
  void _run(void*) { _openbusContext->orb()->run(); }
private:
  openbus::OpenBusContext* _openbusContext;
};
#endif

struct BroadcasterImpl : virtual public POA_tecgraf::openbus::interop::delegation::Broadcaster
{
  BroadcasterImpl(openbus::OpenBusContext& c, delegation::Messenger_var messenger)
    : ctx(c), messenger(messenger) {}

  void post(const char* message)
  {
    ctx.exitChain();
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

int main(int argc, char** argv) {
  try {
    openbus::log.set_level(openbus::info_level);

    ::properties properties_file;
    if(!properties_file.openbus_log_file.empty())
    {
      std::auto_ptr<logger::output_base> output
        (new logger::output::file_output(properties_file.openbus_log_file.c_str()
                                         , std::ios::out));
      openbus::log.add_output(output);
    }
    
    if(properties_file.buses.size() < 1)
      throw std::runtime_error("No bus is configured");

    ::properties::bus bus = properties_file.buses[0];

    CORBA::ORB* orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr <openbus::Connection> conn
      (openbusContext->createConnection(bus.host.c_str(), bus.port));
    openbusContext->setDefaultConnection(conn.get());
    
    #ifdef OPENBUS_SDK_MULTITHREAD
    RunThread* runThread = new RunThread(openbusContext);
    runThread->start();
    #endif

    ::loginWithServerCredentials("interop_delegation_cpp_broadcaster", *conn);

    openbus::idl_or::ServicePropertySeq properties;
    properties.length(2);
    properties[0].name  = "offer.domain";
    properties[0].value = "Interoperability Tests";
    properties[1].name  = "openbus.component.interface";
    properties[1].value = tecgraf::openbus::interop::delegation::_tc_Messenger->id();
    openbus::idl_or::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(properties);
    
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
      scs::core::ComponentContext broadcaster_component(openbusContext->orb(), componentId);
    
      BroadcasterImpl broadcaster_servant(*openbusContext, m);
      broadcaster_component.addFacet("broadcaster", delegation::_tc_Broadcaster->id(), &broadcaster_servant);
    
      openbus::idl_or::ServicePropertySeq props;
      props.length(1);
      openbus::idl_or::ServiceProperty property;
      props[0].name = "offer.domain";
      props[0].value = "Interoperability Tests";

      openbusContext->getOfferRegistry()->registerService(broadcaster_component.getIComponent(), props);
      std::cout << "Broadcaster no ar" << std::endl;
      #ifdef OPENBUS_SDK_MULTITHREAD
      runThread->wait();
      #endif
    }
    else
    {
      std::cout << "Couldn't find messenger" << std::endl;
      return -1;
    }
  } catch(std::exception const& e) {
    std::cout << "[error (std::exception)] " << e.what() << std::endl;
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
