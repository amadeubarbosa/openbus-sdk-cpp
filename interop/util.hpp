// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_INTEROP_UTIL_HPP
#define TECGRAF_SDK_OPENBUS_INTEROP_UTIL_HPP

#include <openbus/OpenBusContext.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <vector>

struct handler
{
  handler(
    CORBA::ORB_var orb,
    const std::vector<openbus::Connection *> &connections,
    boost::asio::io_service *io_service)
    : orb(orb), connections(connections), io_service(io_service)
  {
  }

  handler(
    CORBA::ORB_var orb,
    openbus::Connection *conn,
    boost::asio::io_service *io_service)
    : orb(orb), connections(1, conn), io_service(io_service)
  {
  }

  handler(const handler& o)
  {
    orb = o.orb;
    connections = o.connections;
    io_service = o.io_service;
  }

  void operator()(
    const boost::system::error_code& error,
    int signal_number)
  {
    if (!error)
    {
      for(std::vector<openbus::Connection *>::iterator it(connections.begin());
          it != connections.end();
          ++it)
      {
        (*it)->logout();
      }
      orb->shutdown(true);        
      io_service->stop();
    }
  }

  CORBA::ORB_var orb;
  std::vector<openbus::Connection *> connections;
  boost::asio::io_service *io_service;
};

#ifdef OPENBUS_SDK_MULTITHREAD
void ORBRun(CORBA::ORB_ptr orb)
{
  orb->run();
}
#endif


openbus::OpenBusContext * get_bus_ctx(int argc, char **argv)
{
  CORBA::ORB_var orb(openbus::ORBInitializer(argc, argv));    
  CORBA::Object_var o(orb->resolve_initial_references("RootPOA"));
  PortableServer::POA_var poa(PortableServer::POA::_narrow(o));
  assert(!CORBA::is_nil(poa));
  PortableServer::POAManager_var poa_manager(poa->the_POAManager());
  poa_manager->activate();    
  openbus::OpenBusContext *const bus_ctx(
    dynamic_cast<openbus::OpenBusContext *>
    (orb->resolve_initial_references("OpenBusContext")));
  return bus_ctx;
}

openbus::idl_or::ServiceOfferDescSeq_var find_offers(
  const openbus::OpenBusContext *bus_ctx,
  const openbus::idl_or::ServicePropertySeq &props)
{
  openbus::idl_or::ServiceOfferDescSeq_var found_offers(
    new openbus::idl_or::ServiceOfferDescSeq);
  do
  {
    openbus::idl_or::ServiceOfferDescSeq_var offers(
      bus_ctx->getOfferRegistry()->findServices(props));
    for (CORBA::ULong idx(0); idx != offers->length(); ++idx)      
    {
      try
      {
        if (!offers[idx].service_ref->_non_existent())
        {
          found_offers->length(found_offers->length() + 1);
          found_offers[found_offers->length() - 1] = offers[idx];
        }
      }
      catch (const CORBA::SystemException &)
      {
      }
    }
    if (found_offers->length() > 0)
    {
      break;
    }
    boost::this_thread::sleep(boost::posix_time::seconds(1));
  } while (true);
  return found_offers._retn();
}

#endif
