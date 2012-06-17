#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>
#include <map>

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

struct MessengerImpl : virtual public POA_tecgraf::openbus::interop::delegation::Messenger
{
  MessengerImpl(openbus::Connection& connection)
    : connection(connection) {}

  void post(const char* to, const char* message) 
  {
    std::cout << "post to " << to << " by " << connection.getCallerChain()->callers()[0].entity
              << std::endl;
    std::cout << " Message content: " << message << std::endl;
    delegation::PostDesc desc = {connection.getCallerChain()->callers()[0].entity, message};
    inbox.insert(std::make_pair(to, desc));
  }

  delegation::PostDescSeq* receivePosts()
  {
    std::string from (connection.getCallerChain()->callers()[0].entity);
    std::cout << "Retrieving messages for " << from << std::endl;
    typedef std::multimap<std::string, delegation::PostDesc>::const_iterator iterator;
    std::pair<iterator, iterator> range = inbox.equal_range(from);
    delegation::PostDescSeq_var posts (new delegation::PostDescSeq);
    std::cout << "Retrieving " << std::distance(range.first, range.second) << " messages" << std::endl;
    posts->length(std::distance(range.first, range.second));
    std::size_t index = 0;
    for(iterator first = range.first; first != range.second; ++first, ++index)
    {
      posts[index] = first->second;
    }
    return posts._retn();
  }

  openbus::Connection& connection;
  std::multimap<std::string, delegation::PostDesc> inbox;
};

int main(int argc, char** argv) {
  try {
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    openbus::ConnectionManager* manager = dynamic_cast<openbus::ConnectionManager*>
      (orb->resolve_initial_references(CONNECTION_MANAGER_ID));
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    manager->setDefaultConnection(conn.get());

    #ifdef OPENBUS_SDK_MULTITHREAD
    RunThread* runThread = new RunThread(manager);
    runThread->start();
    #endif

    scs::core::ComponentId componentId;
    componentId.name = "Messenger";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "C++";
    scs::core::ComponentContext messenger_component(manager->orb(), componentId);
    MessengerImpl messenger_servant(*conn.get());
    messenger_component.addFacet("messenger", tecgraf::openbus::interop::delegation::_tc_Messenger->id(), &messenger_servant);
    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "Interoperability Tests";
    props[0] = property;
    conn->loginByPassword("messenger", "messenger");
    conn->offers()->registerService(messenger_component.getIComponent(), props);
    std::cout << "Messenger no ar" << std::endl;
    runThread->wait();
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
