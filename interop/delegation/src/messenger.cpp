#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>
#include <map>

#include "stubs/messages.h"
#include <CORBA.h>
#include "server_login.h"
#include "properties_reader.h"
#include <log/output/file_output.h>

namespace delegation = tecgraf::openbus::interop::delegation;

#ifdef OPENBUS_SDK_MULTITHREAD
void ORBRun(CORBA::ORB_ptr orb)
{
 orb->run();
}
#endif

struct MessengerImpl : virtual public POA_tecgraf::openbus::interop::delegation::Messenger
{
  MessengerImpl(openbus::OpenBusContext& c)
    : ctx(c) {}

  void post(const char* to, const char* message) 
  {
    std::cout << "post to " << to << " by " << ctx.getCallerChain().caller().entity
              << std::endl;
    std::cout << " Message content: " << message << std::endl;
    delegation::PostDesc desc = {ctx.getCallerChain().caller().entity, message};
    inbox.insert(std::make_pair(to, desc));
  }

  delegation::PostDescSeq* receivePosts()
  {
    std::string from (ctx.getCallerChain().caller().entity);
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

  openbus::OpenBusContext& ctx;
  std::multimap<std::string, delegation::PostDesc> inbox;
};

int main(int argc, char** argv) {
  try {
    openbus::log().set_level(openbus::info_level);

    ::properties properties_file;
    if(!properties_file.openbus_log_file.empty())
    {
      std::auto_ptr<logger::output_base> output
        (new logger::output::file_output(properties_file.openbus_log_file.c_str()
                                         , std::ios::out));
      openbus::log().add_output(output);
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
    openbusContext->setDefaultConnection(*conn);

    #ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread orbRun(ORBRun, openbusContext->orb());
    #endif

    scs::core::ComponentId componentId;
    componentId.name = "Messenger";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "C++";
    scs::core::ComponentContext messenger_component(openbusContext->orb(), componentId);
    MessengerImpl messenger_servant(*openbusContext);
    messenger_component.addFacet("messenger", tecgraf::openbus::interop::delegation::_tc_Messenger->id(), &messenger_servant);
    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "Interoperability Tests";
    props[0] = property;

    ::loginWithServerCredentials("interop_delegation_cpp_messenger", *conn);

    openbusContext->getOfferRegistry()->registerService(messenger_component.getIComponent(), props);
    std::cout << "Messenger no ar" << std::endl;
    #ifdef OPENBUS_SDK_MULTITHREAD
    orbRun.join();
    #endif
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
