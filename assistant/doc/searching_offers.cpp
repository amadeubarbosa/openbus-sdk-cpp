// -*- coding: iso-8859-1-unix -*-

//[searching_offers
//[searching_offers_include
#include <openbus/assistant.hpp>
//]

//[searching_offers_include_hello
#include <stubs/hello.h>
//]

#include <iostream>

//[searching_offers_auth
int main()
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "usuario"
     , _password = "senha");
  //]
  //[searching_offers_find_service
  using tecgraf::openbus::core::v2_0::services::offer_registry::ServiceOfferDescSeq;
  ServiceOfferDescSeq offers = assistant.findServices(assistant.createFacetAndEntityProperty("hello", "usuario"), -1);
  //]
  //[searching_offers_use_service
  for(CORBA::ULong i = 0; i != offers.length(); ++i)
    if( ::Hello_var p = ::Hello::_narrow(offers[i].service_ref))
      p->sayHello();
  //]
}
//]
