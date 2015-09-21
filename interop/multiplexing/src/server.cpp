// -*- coding: iso-8859-1-unix -*-

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include "helloS.h"
#pragma clang diagnostic pop
#include <util.hpp>
#include <demo/openssl.hpp>
#include <openbus.hpp>
#include <scs/ComponentContext.h>
#include <log/output/file_output.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <map>
#include <vector>

const std::string entity("interop_multiplexing_cpp_server");
std::string priv_key_filename;
struct bus
{
  std::string host;
  unsigned short port;
};
std::map<std::size_t, bus> buses;
bool debug;

void load_options(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("help", "Help")
    ("debug", po::value<bool>()->default_value(true) , "yes|no")
    ("private-key", po::value<std::string>()->default_value(entity + ".key"),
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
  po::notify(vm);
  if (vm.count("private-key"))
  {
    priv_key_filename = vm["private-key"].as<std::string>();
  }
  if (vm.count("help")) 
  {
    std::cout << desc << std::endl;
    std::exit(1);
  }
  if (vm.count("debug"))
  {
    debug = vm["debug"].as<bool>();
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
  typedef boost::shared_ptr<openbus::Connection>result_type;
  CallDispatchCallback(
    boost::shared_ptr<openbus::Connection> c1,
    boost::shared_ptr<openbus::Connection> c2) 
  {
    _vconn.push_back(c1);
    _vconn.push_back(c2);
  }

  result_type operator()(openbus::OpenBusContext &context, 
                         const std::string busId,
                         const std::string loginId,
                         const std::string operation)
  {
    for (std::vector<boost::shared_ptr<openbus::Connection> >::const_iterator it = 
           _vconn.begin(); it != _vconn.end(); ++it) 
    {
      if (busId == (*it)->busid()) 
      {
        return *it;
      }
    }
    std::cerr << "Conexao de despacho nao encontrada." << std::endl;
    return boost::shared_ptr<openbus::Connection>();
  }
private:
  std::vector<boost::shared_ptr<openbus::Connection> > _vconn;
};

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello 
{
  HelloImpl(openbus::OpenBusContext &c) : _ctx(c) 
  { 
  }

  char *sayHello() 
  {
    openbus::CallerChain chain(_ctx.getCallerChain());
    if (chain == openbus::CallerChain())
    {
      std::cerr << "chain == openbus::CallerChain()"
                << std::endl;
      std::abort();
    }
    std::string msg("Hello " + std::string(chain.caller().entity) + "@"
                    + chain.busid() + "!");
    std::cout << msg << std::endl;
    CORBA::String_var ret(msg.c_str());
    return ret._retn();
  }
private:
  openbus::OpenBusContext &_ctx;
};

void ORBRun(CORBA::ORB_ptr orb)
{
  orb->run();
}

void registerOffer(openbus::OpenBusContext &ctx, boost::shared_ptr<openbus::Connection> conn, 
                   scs::core::ComponentContext &componentCtx)
{
  try 
  {
    openbus::idl::offers::ServicePropertySeq props;
    props.length(1);
    openbus::idl::offers::ServiceProperty property;
    property.name = "offer.domain";
    property.value = "Interoperability Tests";
    props[0] = property;
    ctx.setCurrentConnection(conn);
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
    if (debug)
    {
      openbus::log().set_level(openbus::debug_level);
    }
    boost::shared_ptr<openbus::orb_ctx>
      orb_ctx(openbus::ORBInitializer(argc, argv));
    openbus::OpenBusContext *const bus_ctx(get_bus_ctx(orb_ctx));
    boost::shared_ptr<openbus::Connection> connBusB
      (bus_ctx->connectByAddress(buses[1].host, buses[1].port));
    boost::shared_ptr<openbus::Connection> conn1BusA
      (bus_ctx->connectByAddress(buses[0].host, buses[0].port));
    boost::shared_ptr<openbus::Connection> conn2BusA
      (bus_ctx->connectByAddress(buses[0].host, buses[0].port));
    boost::shared_ptr<openbus::Connection> conn3BusA
      (bus_ctx->connectByAddress(buses[0].host, buses[0].port));
    std::vector<openbus::Connection *> connVec;
    connVec.push_back(conn1BusA.get());
    connVec.push_back(conn2BusA.get());
    connVec.push_back(conn3BusA.get());
    connVec.push_back(connBusB.get());
    
    boost::thread orbRun(ORBRun, bus_ctx->orb());
    
    scs::core::ComponentId componentId;
    componentId.name = "Hello";
    componentId.major_version = '1';
    componentId.minor_version = '0';
    componentId.patch_version = '0';
    componentId.platform_spec = "";
    scs::core::ComponentContext ctx(bus_ctx->orb(), componentId);
    
    HelloImpl srv(*bus_ctx);
    ctx.addFacet("Hello", "IDL:tecgraf/openbus/interop/simple/Hello:1.0", &srv);
    
    EVP_PKEY *priv_key(
      openbus::demo::openssl::read_priv_key(priv_key_filename));
    if (!priv_key)
    {
      std::cout << "Chave privada invalida." << std::endl;
      return -1;
    }
    conn1BusA->loginByCertificate(entity, priv_key);
    conn2BusA->loginByCertificate(entity, priv_key);
    conn3BusA->loginByCertificate(entity, priv_key);
    connBusB->loginByCertificate(entity, priv_key);
    
    bus_ctx->onCallDispatch(CallDispatchCallback(conn1BusA, connBusB));

    std::vector<boost::shared_ptr<openbus::Connection> > connections;
    connections.push_back(conn1BusA);
    connections.push_back(conn2BusA);
    connections.push_back(conn3BusA);
    connections.push_back(connBusB);

    boost::thread register1(boost::bind(registerOffer, boost::ref(*bus_ctx), 
                                        conn1BusA, 
                                        boost::ref(ctx)));
    boost::thread register2(boost::bind(registerOffer, boost::ref(*bus_ctx), 
                                        conn2BusA, 
                                        boost::ref(ctx)));
    boost::thread register3(boost::bind(registerOffer, boost::ref(*bus_ctx), 
                                        conn3BusA, 
                                        boost::ref(ctx)));
    boost::thread register4(boost::bind(registerOffer, boost::ref(*bus_ctx), 
                                        connBusB, 
                                        boost::ref(ctx)));
    orbRun.join();
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
  return 0; //MSVC
}
