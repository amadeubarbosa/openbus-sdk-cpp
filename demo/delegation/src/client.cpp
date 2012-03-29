#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include "stubs/service.h"

openbus::CallerChain* certification;

struct ClientImpl : virtual public POA_Client {
  ClientImpl(openbus::Connection* c) : _conn(c) { }
  void sign(CORBA::Long passportNumber) 
    throw (CORBA::SystemException) 
  {
    std::cout << "Signing #" << passportNumber << "." << std::endl;
    certification = _conn->getCallerChain();
  }
  private:
    openbus::Connection* _conn;
};

int main(int argc, char** argv) {
  try {
    std::auto_ptr <openbus::Connection> conn (openbus::connect("localhost", 2089));
    conn->loginByPassword("client", "client");
    scs::core::ComponentId componentId;
    componentId.name = "Client";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    scs::core::ComponentContext* ctx = new scs::core::ComponentContext(conn->orb(), componentId);
    PortableServer::ServantBase* clientServant(new ClientImpl(conn.get()));
    ctx->addFacet("client", "IDL:Client:1.0", clientServant);    
    openbus::idl_or::ServicePropertySeq props;
    props.length(0);
    conn->offers()->registerService(ctx->getIComponent(), props);

    openbus::idl_or::ServicePropertySeq _props;
    _props.length(1);
    _props[0].name  = "certifier";
    _props[0].value = "goGo";
    openbus::idl_or::ServiceOfferDescSeq_var offers = conn->offers()->findServices(_props);
    if (offers->length()) {
      CORBA::Object_var o = offers[0].service_ref->getFacetByName("certifier");
      Certifier* c = Certifier::_narrow(o);
      CORBA::Long passportNumber = 103045;
      if (c->stamp(clientServant->_this(), passportNumber)) {
        conn->joinChain(certification);
        props.length(1);
        props[0].name  = "openbus.component.facet";
        props[0].value = "airport";
        openbus::idl_or::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
        CORBA::Object_var o = offers[0].service_ref->getFacetByName("airport");
        Airport* a = Airport::_narrow(o);
        a->fly(passportNumber, "iberia");
      }
    } else
      std::cout << "nenhuma oferta encontrada." << std::endl;
    conn->close();
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
