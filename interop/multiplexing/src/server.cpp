// -*- coding: iso-8859-1-unix -*-

#include "stubs/hello.h"
#include <openbus/ORBInitializer.hpp>
#include <openbus/log.hpp>
#include <openbus/OpenBusContext.hpp>
#include <openbus/Connection.hpp>
#include <scs/ComponentContext.h>
#include <log/output/file_output.h>

#include <iostream>
#include <map>
#include <vector>
#include <CORBA.h>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
#endif
#include <boost/bind.hpp>
#include <boost/program_options.hpp>

const std::string entity("interop_multiplexing_cpp_server");
std::string private_key;
struct bus
{
  std::string host;
  unsigned short port;
};
std::map<std::size_t, bus> buses;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("private-key", po::value<std::string>()->default_value("admin/" + entity
                                                            + ".key"),
     "Path to private key")
    ("bus.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to OpenBus")
    ("bus.host.port", po::value<unsigned short>()->default_value(2089), 
     "Port to OpenBus")
    ("bus2.host.name", po::value<std::string>()->default_value("localhost"),
     "Host to second OpenBus")
    ("bus2.host.port", po::value<unsigned short>()->default_value(3089), 
     "Port to second OpenBus");
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::store(po::parse_config_file<char>("test.properties", desc), vm);
  po::notify(vm);
  if (vm.count("private-key"))
  {
    private_key = vm["private-key"].as<std::string>();
  }
  if (vm.count("help")) 
  {
    std::cout << desc << std::endl;
    std::exit(1);
  }
  if (vm.count("bus.host.name"))
  {
    buses[0].host = vm["bus.host.name"].as<std::string>();
  }
  if (vm.count("bus.host.port"))
  {
    buses[0].port = vm["bus.host.port"].as<unsigned short>();
  }
  if (vm.count("bus2.host.name"))
  {
    buses[1].host = vm["bus2.host.name"].as<std::string>();
  }
  if (vm.count("bus2.host.port"))
  {
    buses[1].port = vm["bus2.host.port"].as<unsigned short>();
  }
}

class CallDispatchCallback 
{
public:
  typedef openbus::Connection *result_type;
  CallDispatchCallback(openbus::Connection &c1, openbus::Connection &c2) 
  {
    _vconn.push_back(&c1);
    _vconn.push_back(&c2);
  }

  result_type operator()(openbus::OpenBusContext &context, 
                         const std::string busId, const std::string loginId, 
                         const std::string operation)
  {
    for (std::vector<openbus::Connection *>::const_iterator it = 
           _vconn.begin(); it != _vconn.end(); ++it) 
    {
      if (busId == (*it)->busid()) 
      {
        return *it;
      }
    }
    std::cerr << "Conexao de despacho nao encontrada." << std::endl;
    return 0;
  }
private:
  std::vector<openbus::Connection *> _vconn;
};

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello 
{
  HelloImpl(openbus::OpenBusContext &c) : _ctx(c) 
  { 
  }

  char *sayHello() 
  {
    openbus::CallerChain chain = _ctx.getCallerChain();
    assert(chain != openbus::CallerChain());
    std::string msg = "Hello " + std::string(chain.caller().entity) + "@"
      + chain.busid() + "!";
    std::cout << msg << std::endl;
    CORBA::String_var r = CORBA::string_dup(msg.c_str());
    return r._retn();
  }
private:
  openbus::OpenBusContext &_ctx;
};

#ifdef OPENBUS_SDK_MULTITHREAD
void ORBRun(CORBA::ORB_ptr orb)
{
  orb->run();
}
#endif

void registerOffer(openbus::OpenBusContext &ctx, openbus::Connection &conn, 
                   scs::core::ComponentContext &componentCtx)
{
  try 
  {
    openbus::idl_or::ServicePropertySeq props;
    props.length(1);
    openbus::idl_or::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "Interoperability Tests";
    props[0] = property;
    ctx.setCurrentConnection(&conn);
    ctx.getOfferRegistry()->registerService(componentCtx.getIComponent(), 
                                            props);
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "error (CORBA::Exception): " << e << std::endl;
  }
}

int main(int argc, char **argv) {
  try 
  {
    load_options(argc, argv);
    openbus::log().set_level(openbus::debug_level);

    CORBA::ORB_ptr orb = openbus::ORBInitializer(argc, argv);
    CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    assert(!CORBA::is_nil(poa));
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    openbus::OpenBusContext *busCtx = dynamic_cast<openbus::OpenBusContext*>
      (orb->resolve_initial_references("OpenBusContext"));
    std::auto_ptr <openbus::Connection> connBusB
      (busCtx->createConnection(buses[0].host, buses[0].port));
    std::auto_ptr <openbus::Connection> conn1BusA
      (busCtx->createConnection(buses[1].host, buses[1].port));
    std::auto_ptr <openbus::Connection> conn2BusA
      (busCtx->createConnection(buses[1].host, buses[1].port));
    std::auto_ptr <openbus::Connection> conn3BusA
      (busCtx->createConnection(buses[1].host, buses[1].port));
    std::vector<openbus::Connection *> connVec;
    connVec.push_back(conn1BusA.get());
    connVec.push_back(conn2BusA.get());
    connVec.push_back(conn3BusA.get());
    connVec.push_back(connBusB.get());
    
#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread orbRun(ORBRun, busCtx->orb());
#endif
    
    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    scs::core::ComponentContext ctx(busCtx->orb(), componentId);
    
    HelloImpl srv(*busCtx);
    ctx.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0", &srv);
    
    try 
    {
      conn1BusA->loginByCertificate(entity, openbus::PrivateKey(private_key));
      conn2BusA->loginByCertificate(entity, openbus::PrivateKey(private_key));
      conn3BusA->loginByCertificate(entity, openbus::PrivateKey(private_key));
      connBusB->loginByCertificate(entity, openbus::PrivateKey(private_key));
    }
    catch(const openbus::InvalidPrivateKey &e)
    {
      std::cout << e.what() << std::endl;
    }
    
    busCtx->onCallDispatch(CallDispatchCallback(*conn1BusA, *connBusB));

#ifdef OPENBUS_SDK_MULTITHREAD
    boost::thread register1(boost::bind(registerOffer, boost::ref(*busCtx), 
                                        boost::ref(*conn1BusA), 
                                        boost::ref(ctx)));
    boost::thread register2(boost::bind(registerOffer, boost::ref(*busCtx), 
                                        boost::ref(*conn2BusA), 
                                        boost::ref(ctx)));
    boost::thread register3(boost::bind(registerOffer, boost::ref(*busCtx), 
                                        boost::ref(*conn3BusA), 
                                        boost::ref(ctx)));
    boost::thread register4(boost::bind(registerOffer, boost::ref(*busCtx), 
                                        boost::ref(*connBusB), 
                                        boost::ref(ctx)));
    orbRun.join();
#else
    registerOffer(*busCtx, *conn1BusA, ctx);
    registerOffer(*busCtx, *conn2BusA, ctx);
    registerOffer(*busCtx, *conn3BusA, ctx);
    registerOffer(*busCtx, *connBusB, ctx);
    busCtx->orb()->run();
#endif
  } 
  catch (const std::exception &e) 
  {
    std::cout << "[error (std::exception)] " << e.what() << std::endl;
    return -1;
  } 
  catch (const CORBA::Exception &e) 
  {
    std::cout << "[error (CORBA::Exception)] " << e << std::endl;
    return -1;
  }
}
