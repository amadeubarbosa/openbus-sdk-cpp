#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/service.h"
#include <CORBA.h>

struct AirportImpl : virtual public POA_Airport {
  AirportImpl(openbus::Connection* c) : _conn(c) { }
  CORBA::Boolean fly(CORBA::Long passportNumber, const char* airline) 
    throw (CORBA::SystemException) 
  {
    openbus::CallerChain* chain = _conn->getCallerChain();
    char* certifier = chain->callers[0].id;
    if (strcmp(certifier, "goGo"))
      return false;
    char* passenger = chain->callers[1].id;
    std::cout << "'" << passenger << "' flying with passport #" << passportNumber << std::endl;
    // std::cout << "Passport #" << passportNumber << " certified." << std::endl;
    return true;
  }
  private:
    openbus::Connection* _conn;
};

int main(int argc, char** argv) {
  try {
    std::auto_ptr <openbus::Connection> conn (openbus::connect("localhost", 2089));
    
    scs::core::ComponentId componentId;
    componentId.name = "Airport";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    scs::core::ComponentContext* ctx = new scs::core::ComponentContext(conn->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> airportServant(new AirportImpl(conn.get()));
    ctx->addFacet("airport", "IDL:Airport:1.0", airportServant);
    
    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "OpenBus Demos";
    props[0] = property;

    conn->loginByPassword("airport", "airport");
    conn->offers()->registerService(ctx->getIComponent(), props);
    conn->orb()->run();
  } catch (const CORBA::Exception& e) {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } catch (const openbus::Connection::Exception& e) {
    std::cout << "[error (Connection::Exception)] " << e.name() << std::endl;
    return -1;
  } catch (...) {
    std::cout << "[error *unknow exception*]" << std::endl;
    return -1;
  }
  return 0;
}
