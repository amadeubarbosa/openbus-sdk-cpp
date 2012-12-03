
//[create_offers
//[create_offers_component_created
//[create_offers_include
#include <openbus/assistant.h>
//]

//[create_offers_include_hello
#include <stubs/hello.h>
//]

#include <iostream>

//[create_offers_servant_hello
struct HelloImpl : ::POA_Hello
{
  void sayHello()
  {
    std::cout << "Hello World" << std::endl;
  }
};
//]

//[create_offers_auth
int main()
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "usuario"
     , _password = "senha");
  //]
  //[create_offers_instantiate_servant_hello
  HelloImpl helloImpl;
  //]
  //[create_offers_instantiate_component
  scs::core::ComponentId id = {"name", 1, 0, 0, "C++"};
  scs::core::ComponentContext component(assistant.orb(), id);
  //]
  //[create_offers_add_facet
  component.addFacet("hello", _tc_Hello->id(), &helloImpl);
  //]
  //]
  //[create_offers_register_service
  assistant.registerService(component.getIComponent());
  //]
  //[create_offers_run
  assistant.orb()->run();
  //]
}
//]
