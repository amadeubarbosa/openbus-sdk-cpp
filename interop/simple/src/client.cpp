#include <openbus/ORBInitializer.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>
#include <openbus/log.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include "stubs/hello.h"

const std::string entity("interop_hello_cpp_client");

std::string host("localhost");
unsigned short port = 2089;
bool regex_cb(const boost::match_results<std::string::const_iterator> &what)
{
  if ("bust.host.name" == what[1])
  {
    host = what[2];
  }
  else if ("bus.host.port" == what[1])
  {
    port = boost::lexical_cast<unsigned short>(what[2]);
  }
  return true;
}

int main(int argc, char **argv) 
{
  std::string prop;
  {
    std::ifstream in("test.properties");
    assert(in);
    in.seekg(0, std::ios::end);
    prop.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&prop[0], prop.size());
  }
  boost::regex exp("([\\w\\.]+)\\s*=\\s*(\\w+)");
  boost::sregex_iterator m1(prop.begin(), prop.end(), exp), m2;
  std::for_each(m1, m2, &regex_cb);
  try
  {
    openbus::log().set_level(openbus::debug_level);
    CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::OpenBusContext *ctx = dynamic_cast<openbus::OpenBusContext *>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr<openbus::Connection> conn(ctx->createConnection(host, port));
    ctx->setDefaultConnection(conn.get());
    conn->loginByPassword(entity, entity);
    openbus::idl_or::ServicePropertySeq props;
    props.length(2);
    CORBA::ULong i = 0;
    props[i].name  = "offer.domain";
    props[i].value = "Interoperability Tests";
    props[i+1].name  = "openbus.component.interface";
    props[i+1].value = "IDL:tecgraf/openbus/interop/simple/Hello:1.0";
    openbus::idl_or::ServiceOfferDescSeq_var offers = 
      ctx->getOfferRegistry()->findServices(props);
    for (CORBA::ULong idx = 0; idx != offers->length(); ++idx) 
    {
      CORBA::Object_var o = offers[idx].service_ref->getFacetByName("Hello");
      tecgraf::openbus::interop::simple::Hello *hello = 
        tecgraf::openbus::interop::simple::Hello::_narrow(o);
      const char *msg = hello->sayHello();
      std::string s = "Hello " + entity + "!";
      if (!(msg == s))
      {
        throw std::runtime_error("msg != s");
      }
    }
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
}
