#include <openbus/ORBInitializer.h>
#include <openbus/log.h>
#include <openbus/OpenBusContext.h>
#include <openbus/Connection.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include "stubs/messages.h"

#include "properties_reader.h"
#include <log/output/file_output.h>

openbus::CallerChain* certification;

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
      (openbusContext->createConnection(bus.host, bus.port));
    openbusContext->setDefaultConnection(conn.get());
    
    #ifdef OPENBUS_SDK_MULTITHREAD
    RunThread* runThread = new RunThread(openbusContext);
    runThread->start();
    #endif
    
    conn->loginByPassword("interop_delegation_cpp_broadcaster"
                          , "interop_delegation_cpp_broadcaster");

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
      
      properties[1].value = tecgraf::openbus::interop::delegation::_tc_Forwarder->id();
      offers = openbusContext->getOfferRegistry()->findServices(properties);
      if(offers->length() > 0)
      {
        o = offers[static_cast<CORBA::ULong> (0)].service_ref->getFacetByName("forwarder");
        tecgraf::openbus::interop::delegation::Forwarder_var
          forwarder = tecgraf::openbus::interop::delegation::Forwarder::_narrow(o);

        properties[1].value = tecgraf::openbus::interop::delegation::_tc_Broadcaster->id();
        offers = openbusContext->getOfferRegistry()->findServices(properties);
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
          while((i = sleep(i)));

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
