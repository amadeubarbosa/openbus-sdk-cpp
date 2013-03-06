#include "stubs/hello.h"
#include <openbus/ORBInitializer.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>
#include <openbus/log.hpp>
#include <scs/ComponentContext.h>

#include <CORBA.h>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/bind.hpp>

#include <iostream>
#include <fstream>
#include <typeinfo>

const std::string entity("interop_hello_cpp_server");

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello 
{
  HelloImpl(openbus::OpenBusContext &c) : _busCtx(c) 
  { 
  }

  char *sayHello() 
  {
    openbus::CallerChain chain = _busCtx.getCallerChain();
    assert(chain != openbus::CallerChain());
    std::string msg = "Hello " + std::string(chain.caller().entity) + "!";
    std::cout << msg << std::endl;
    CORBA::String_var r = CORBA::string_dup(msg.c_str());
    return r._retn();
  }
private:
  openbus::OpenBusContext &_busCtx;
};

void loginAndRegister(const openbus::OpenBusContext &busCtx, 
                      scs::core::ComponentContext &compCtx,
                      const openbus::idl_or::ServicePropertySeq &props,
                      openbus::Connection &conn)
{
  const openbus::PrivateKey pKey(entity + ".key");
  conn.loginByCertificate(entity, pKey);
  busCtx.getOfferRegistry()->registerService(compCtx.getIComponent(), 
                                                      props);
}

struct onInvalidLogin
{
  typedef void result_type;
  onInvalidLogin(const openbus::OpenBusContext &busCtx, 
                 scs::core::ComponentContext &ctx, 
                 const openbus::idl_or::ServicePropertySeq &props,
                 openbus::Connection  &conn) 
    : _busCtx(busCtx), _compCtx(ctx), _props(props), _conn(conn)
  {
  }

  result_type operator()(openbus::Connection &c, openbus::idl_ac::LoginInfo l) 
  {
    try 
    {
      std::cout << "invalid login: " << l.id.in() << std::endl; 
      loginAndRegister(_busCtx, _compCtx, _props, _conn);
    } 
    catch (const CORBA::Exception &e) 
    {
      std::cout << "[error (CORBA::Exception)] " << e << std::endl;    
    }
  }
private:
  const openbus::OpenBusContext &_busCtx;
  scs::core::ComponentContext &_compCtx;
  const openbus::idl_or::ServicePropertySeq &_props;
  openbus::Connection &_conn;
};

#ifdef OPENBUS_SDK_MULTITHREAD
void ORBRun(CORBA::ORB_ptr orb)
{
  orb->run();
}
#endif

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
  try 
  {
    std::string host("localhost");
    unsigned short port = 2089;
    std::ifstream config("test.properties", std::ifstream::in);
    assert(config);
    read(config, host, "bus.host.name");
    read(config, port, "bus.host.port");
    config.close();
    
    openbus::log().set_level(openbus::debug_level);
    CORBA::ORB_ptr orb = openbus::ORBInitializer(argc, argv);
    
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    
    openbus::OpenBusContext *busCtx = dynamic_cast<openbus::OpenBusContext *>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr<openbus::Connection> conn = busCtx->createConnection(host, 
                                                                       port);
    busCtx->setDefaultConnection(conn.get());
    
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread orbRun(boost::bind(ORBRun, busCtx->orb()));
#endif
    
    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "c++";
    scs::core::ComponentContext compCtx(busCtx->orb(), componentId);

    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    CORBA::ULong i = 0;
    property.name = "offer.domain";
    property.value = "Interoperability Tests";
    props[i] = property;

    conn->onInvalidLogin(onInvalidLogin(*busCtx, compCtx, props, *conn));

    HelloImpl srv(*busCtx);
    compCtx.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0", 
                     &srv);

    loginAndRegister(*busCtx, compCtx, props, *conn);

#ifdef OPENBUS_SDK_MULTITHREAD
    orbRun.join();
#else
    busCtx->orb()->run();
#endif
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  } 
}
