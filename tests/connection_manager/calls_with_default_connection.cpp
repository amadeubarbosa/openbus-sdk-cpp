// -*- coding: iso-8859-1-unix -*-

#include <openbus/OpenBusContext.hpp>
#include <openbus/ORBInitializer.hpp>
#include <configuration.h>
#include <stubs/hello.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  CORBA::ORB_var orb = openbus::ORBInitializer(argc, argv);
  CORBA::Object_ptr obj_connection_manager = orb->resolve_initial_references("OpenBusContext");
  openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>(obj_connection_manager);
  std::auto_ptr<openbus::Connection> conn(openbusContext->connectByAddress(cfg.host(), cfg.port()));
  conn->loginByPassword(cfg.user(), cfg.password());
  openbusContext->setDefaultConnection(conn.get());
  
  openbus::idl_or::ServicePropertySeq properties;
  openbus::idl_or::ServiceOfferDescSeq_var offers = openbusContext->getOfferRegistry()->findServices(properties);

  if(offers->length() == 1)
  {
    CORBA::Object_var hello_obj = (*offers)[0u].service_ref->getFacetByName("hello");
    ::Hello_var hello = ::Hello::_narrow(hello_obj);

    hello->sayHello();
  }
  else
    std::abort();
  return 0; //MSVC
}
