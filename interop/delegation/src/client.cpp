#include <openbus.h>
#include <openbus/log.h>
#include <openbus/manager.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include "stubs/messages.h"

openbus::CallerChain* certification;

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

int main(int argc, char** argv) {
  try {
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
    
    conn->loginByPassword("client", "client");

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
      
      properties[1].value = tecgraf::openbus::interop::delegation::_tc_Forwarder->id();
      offers = conn->offers()->findServices(properties);
      if(offers->length() > 0)
      {
        o = offers[static_cast<CORBA::ULong> (0)].service_ref->getFacetByName("forwarder");
        tecgraf::openbus::interop::delegation::Forwarder_var
          forwarder = tecgraf::openbus::interop::delegation::Forwarder::_narrow(o);

        properties[1].value = tecgraf::openbus::interop::delegation::_tc_Broadcaster->id();
        offers = conn->offers()->findServices(properties);
        if(offers->length() > 0)
        {
          o = offers[static_cast<CORBA::ULong> (0)].service_ref->getFacetByName("broadcaster");
          tecgraf::openbus::interop::delegation::Broadcaster_var
            broadcaster = tecgraf::openbus::interop::delegation::Broadcaster::_narrow(o);

          conn->logout();

          conn->loginByPassword("willian", "willian");
          forwarder->setForward("bill");
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("paul", "paul");
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("mary", "mary");
          broadcaster->subscribe();
          conn->logout();

          conn->loginByPassword("steve", "steve");
          broadcaster->subscribe();
          broadcaster->post("Testando a lista!");
          conn->logout();

          int i = 10;
          while(i = sleep(i));

          const char* names[] = {"willian", "bill", "paul", "mary", "steve"};
          for(const char** first = &names[0]; first != &names[sizeof(names)/sizeof(names[0])]
                ;++first)
          {
            conn->loginByPassword(*first, *first);
            delegation::PostDescSeq_var posts = m->receivePosts();
            for(std::size_t i = 0; i != posts->length(); ++i)
            {
              std::cout << i << ") " << posts[i].from << ": " << posts[i].message << std::endl;
            }
            broadcaster->unsubscribe();
            conn->logout();
          }

          conn->loginByPassword("willian", "willian");
          forwarder->cancelForward("bill");
          conn->logout();
        }
      }
    } else
      std::cout << "nenhuma oferta encontrada." << std::endl;
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
