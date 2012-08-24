#include <openbus/extension/openbus.h>
#include <iostream>
#include "hello.h"

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace simple = tecgraf::openbus::interop::simple;

int main(int argc, char** argv)
{
  openbus::extension::Openbus
    openbus = openbus::extension::Openbus::startByPassword
    ("demo", "demo", "localhost", 2089, argc, const_cast<const char**>(argv));

  bool helloSaid = false;
  do
  {
    offer_registry::ServiceOfferDescSeq offers
      = openbus.filterWorkingOffers(openbus.findOffers
        (openbus.createFacetAndEntityProperty("hello", "demo"), -1));
    CORBA::ULong i = 0;
    while(i != offers.length())
    {
      try
      {
        simple::Hello_var hello = simple::Hello::_narrow
          (offers[i].service_ref->getFacetByName("hello"));
        if(!CORBA::is_nil(hello))
        {
          hello->sayHello();
          helloSaid = true;
          break;
        }
      }
      catch(CORBA::TRANSIENT const&) {}
      catch(CORBA::COMM_FAILURE const&) {}
      catch(CORBA::OBJECT_NOT_EXIST const&) {}
      ++i;
    }
  }
  while(!helloSaid);
  return 0;
}
