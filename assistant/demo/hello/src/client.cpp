// -*- coding: iso-8859-1-unix -*-
#include <openbus/assistant.hpp>
#include <iostream>
#include <helloC.h>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace simple = tecgraf::openbus::interop::simple;

int main(int argc, char **argv)
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "demo", _password = "demo"
     , _argc = argc, _argv = argv
     , _log_level = logger::debug_level);

  bool helloSaid(false);
  do
  {
    offer_registry::ServiceOfferDescSeq offers
      = assistant.filterWorkingOffers(assistant.findServices
        (assistant.createFacetAndEntityProperty("hello", "demo"), -1));
    CORBA::ULong i(0);
    while(i != offers.length())
    {
      try
      {
        simple::Hello_var hello(
          simple::Hello::_narrow
          (offers[i].service_ref->getFacetByName("hello")));
        if(!CORBA::is_nil(hello))
        {
          hello->sayHello();
          std::cout << "sayHello was called" << std::endl;
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
