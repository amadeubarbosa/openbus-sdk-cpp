
//[searching_offers_errors_retry
//[searching_offers_errors_retry_include
#include <openbus/assistant.hpp>
//]

//[searching_offers_errors_retry_include_hello
#include <stubs/hello.h>
//]

#include <iostream>

//[searching_offers_errors_retry_auth
int main()
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "usuario"
     , _password = "senha");
  //]
  //[searching_offers_errors_retry_use_service
  using tecgraf::openbus::core::v2_0::services::offer_registry::ServiceOfferDescSeq;
  bool retry = true;
  do
  {
    ServiceOfferDescSeq offers = assistant.findServices(assistant.createFacetAndEntityProperty("hello", "usuario"), -1);
    for(CORBA::ULong i = 0; i != offers.length(); ++i)
      try
      {
        if( ::Hello_var p = ::Hello::_narrow(offers[i].service_ref))
        {
          p->sayHello();
          retry = false;
        }
      }
      catch(CORBA::SystemException const&) {}
  }
  while(retry);
  //]
}
//]
