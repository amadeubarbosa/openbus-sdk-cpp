#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include <typeinfo>

#include "stubs/hello.h"
#include "stubs/facetC.h"
#include "stubs/facetS.h"
#include <CORBA.h>

struct HelloImpl : virtual public POA_IHello {
  void sayHello() throw (CORBA::SystemException) {
    std::cout << "Hello" << std::endl;
  };
};

class facetC_impl : 
  virtual public POA_facetC,
  virtual public PortableServer::RefCountServantBase 
 {
  CORBA::ULong getId() {
    return 28;
  }
};

int main(int argc, char** argv) {
  try {
    // openbus::ORB* orb = openbus::createORB(argc, argv);
    // std::auto_ptr <openbus::Connection> conn = std::auto_ptr <openbus::Connection> 
    //   (openbus::connect("localhost", 2089, orb));
    // conn->loginByCertificate("DemoCppHello", "DemoCppHello.key");

    // CORBA::ORB* _orb = orb->orb();
    CORBA::ORB_var _orb = CORBA::ORB_init(argc, argv);
    
    scs::core::ComponentId componentId;
    componentId.name = "HelloComponent";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "nenhuma";
    
    scs::core::ComponentContext* ctx = new scs::core::ComponentContext(_orb, componentId);
    
    std::auto_ptr<PortableServer::ServantBase> facet(new HelloImpl);
    ctx->addFacet("IHello", "IDL:IHello:1.0", facet);
    assert(facet.get() == 0);
    
    // openbus::openbusidl_offer_registry::ServicePropertySeq props(0);
    // 
    // conn->offer_registry()->registerService(ctx->getIComponent(), props);
    // CORBA::ORB* _orb = orb->orb();
    // CORBA::Object_var o = _orb->resolve_initial_references("RootPOA");
    // PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    // PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    // poa_manager->activate();
    // facetC_impl s;
    // facetC_var facetC = s._this();
    // o = _orb->string_to_object(argv[1]);
    // facetS_var facetS = facetS::_narrow(o);
    // facetS->foo(facetC);
    
    
    CORBA::Object_var o = _orb->string_to_object(argv[1]);
    facetS_var facetS = facetS::_narrow(o);
    facetS->foo(ctx->getIComponent());
    
    // conn->close();
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
