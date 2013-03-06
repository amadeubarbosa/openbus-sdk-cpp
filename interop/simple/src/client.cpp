#include <openbus/ORBInitializer.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>
#include <openbus/log.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "stubs/hello.h"

const std::string entity("interop_hello_cpp_client");

template <typename T>
void read(std::ifstream &in, T &val, const std::string &param)
{
  std::string arg;
  while (in >> arg) 
  {
    if (arg == param) 
    {
      char c;
      in >> c;
      if (c == '=') 
      {
        if (!(in >> val)) 
        {
          assert(0);
        }
      } 
      else 
      {
        assert(0);
      }
    } 
  }
}

int main(int argc, char **argv) 
{
  std::string host("localhost");
  unsigned short port = 2089;
  std::ifstream config("test.properties", std::ifstream::in);
  assert(config);
  read(config, host, "bus.host.name");
  read(config, port, "bus.host.port");
  config.close();

  try
  {
    openbus::log().set_level(openbus::debug_level);
    CORBA::ORB_ptr orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::OpenBusContext *busCtx = dynamic_cast<openbus::OpenBusContext *>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr <openbus::Connection> conn (busCtx->createConnection(host, 
                                                                       port));
    busCtx->setDefaultConnection(conn.get());
    conn->loginByPassword(entity, entity);
    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    CORBA::ULong i = 0;
    props[i].name  = "offer.domain";
    props[i].value = "Interoperability Tests";
    props[i+1].name  = "openbus.component.interface";
    props[i+1].value = "IDL:tecgraf/openbus/interop/simple/Hello:1.0";
    openbus::idl_or::ServiceOfferDescSeq_var offers = 
      busCtx->getOfferRegistry()->findServices(props);
    for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
    {
      CORBA::Object_var o = offers[idx].service_ref->getFacetByName("Hello");
      tecgraf::openbus::interop::simple::Hello *hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      const char *msg = hello->sayHello();
      std::string s = "Hello " + entity + "!";
      assert(msg == s);
    }
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
}
