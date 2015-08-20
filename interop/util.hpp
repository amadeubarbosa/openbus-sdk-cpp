// -*- coding: iso-8859-1-unix -*-

#ifndef TECGRAF_SDK_OPENBUS_INTEROP_UTIL_HPP
#define TECGRAF_SDK_OPENBUS_INTEROP_UTIL_HPP

#include <openbus/OpenBusContext.hpp>
#include <tao/PortableServer/PortableServer.h>
#include <boost/thread.hpp>

openbus::OpenBusContext * get_bus_ctx(boost::shared_ptr<openbus::orb_ctx> orb_ctx)
{
  CORBA::Object_var o(orb_ctx->orb()->resolve_initial_references("RootPOA"));
  PortableServer::POA_var poa(PortableServer::POA::_narrow(o));
  assert(!CORBA::is_nil(poa));
  PortableServer::POAManager_var poa_manager(poa->the_POAManager());
  poa_manager->activate();    
  CORBA::Object_var obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  return bus_ctx;
}

openbus::idl::offers::ServiceOfferDescSeq_var find_offers(
  const openbus::OpenBusContext *bus_ctx,
  const openbus::idl::offers::ServicePropertySeq &props)
{
  openbus::idl::offers::ServiceOfferDescSeq_var found_offers(
    new openbus::idl::offers::ServiceOfferDescSeq);
  do
  {
    openbus::idl::offers::ServiceOfferDescSeq_var offers(
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
