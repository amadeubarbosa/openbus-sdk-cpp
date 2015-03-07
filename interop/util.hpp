// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_INTEROP_UTIL_HPP
#define TECGRAF_SDK_OPENBUS_INTEROP_UTIL_HPP

#include <openbus/OpenBusContext.hpp>
#include <tao/PortableServer/PortableServer.h>
#include <boost/thread.hpp>

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
