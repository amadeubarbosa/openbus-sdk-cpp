#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>
#include <iostream>
#include "stubs/hello.h"

#include "properties_reader.h"
#include <log/output/file_output.h>

const std::string entity("interop_multiplexing_cpp_client");

int main(int argc, char **argv) 
{
  try {
    openbus::log().set_level(openbus::debug_level);

    ::properties properties_file;
    if(!properties_file.openbus_log_file.empty())
    {
      std::auto_ptr<logger::output_base> output (
        new logger::output::file_output(properties_file.openbus_log_file.c_str()
                                        , std::ios::out));
      openbus::log().add_output(output);
    }
    
    if(properties_file.buses.size() < 2)
    {
      throw std::runtime_error(
        "There should be 2 buses configured in properties file");
    }

    CORBA::ORB_ptr orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::OpenBusContext *busCtx = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    
    for (std::size_t busIdx = 0; busIdx != 2; ++busIdx)
    {
      std::auto_ptr <openbus::Connection> 
        conn (busCtx->createConnection(properties_file.buses[busIdx].host
                                       , properties_file.buses[busIdx].port));
      busCtx->setDefaultConnection(conn.get());
      conn->loginByPassword(entity, entity);
      openbus::idl_or::ServicePropertySeq props;
      props.length(2);
      CORBA::ULong i = 0;
      props[i].name  = "openbus.component.interface";
      props[i].value = "IDL:tecgraf/openbus/interop/simple/Hello:1.0";
      props[i+1].name  = "offer.domain";
      props[i+1].value = "Interoperability Tests";
      openbus::idl_or::ServiceOfferDescSeq_var offers = 
        busCtx->getOfferRegistry()->findServices(props);
      for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
      {
        CORBA::Object_var o = offers[idx].service_ref->getFacetByName("Hello");
        tecgraf::openbus::interop::simple::Hello *hello = 
          tecgraf::openbus::interop::simple::Hello::_narrow(o);
        std::string msg(hello->sayHello());
        std::string s = "Hello " + entity + "@" + conn->busid() + "!";
        assert(s == msg);
      }
    }
  } 
  catch(const std::exception &e) 
  {
    std::cout << "[error (std::exception)] " << e.what() << std::endl;
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
}
