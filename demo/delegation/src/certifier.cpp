#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/service.h"
#include <CORBA.h>

struct CertifierImpl : virtual public POA_Certifier {
  CertifierImpl(openbus::Connection* c) : _conn(c) { }
  CORBA::Boolean stamp(CORBA::Object* o, CORBA::Long passportNumber) 
    throw (CORBA::SystemException) 
  {
    if (passportNumber != 103045)
      return false;
    try {
      Client* c = Client::_narrow(o);
      openbus::CallerChain* chain = _conn->getCallerChain();
      // _conn->joinChain();
      c->sign(passportNumber);
    } catch(CORBA::Exception& e) {
      return false;
    }
    std::cout << "Passport #" << passportNumber << " certified." << std::endl;
    return true;
  }
  private:
    openbus::Connection* _conn;
};

int main(int argc, char** argv) {
  try {
    std::auto_ptr <openbus::Connection> conn (openbus::connect("localhost", 2089));
    
    scs::core::ComponentId componentId;
    componentId.name = "Certifier";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    scs::core::ComponentContext* ctx = new scs::core::ComponentContext(conn->orb(), componentId);
    
    std::auto_ptr<PortableServer::ServantBase> certifierServant(new CertifierImpl(conn.get()));
    ctx->addFacet("certifier", "IDL:Certifier:1.0", certifierServant);
    
    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    property.name = "certifier";
    property.value = "goGo";
    props[0] = property;

    conn->loginByPassword("goGo", "goGo");
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
