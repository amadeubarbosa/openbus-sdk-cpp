// -*- coding: iso-8859-1-unix -*-

//[create_offers_chain
#include <openbus/assistant.hpp>

#include <stubs/hello.h>

#include <iostream>

//[create_offers_chain_servant_hello
//[create_offers_chain_servant_constructor
struct HelloImpl : ::POA_Hello
{
  HelloImpl(openbus::assistant::Assistant assistant)
    : assistant(assistant) {}
  openbus::assistant::Assistant assistant;
  //]

  //[create_offers_chain_say_hello
  void sayHello()
  {
    //[create_offers_get_openbus_context
    openbus::OpenBusContext* openbusContext = dynamic_cast<openbus::OpenBusContext*>
      (assistant.orb()->resolve_initial_references("OpenBusContext"));
    //]
    openbus::CallerChain chain = openbusContext->getCallerChain();
    std::cout << "Hello " << chain.caller().entity << std::endl;
  }
  //]
};
//]

//[create_offers_chain_auth
int main()
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "usuario"
     , _password = "senha");
  //]
  //[create_offers_chain_instantiate_servant_hello
  HelloImpl helloImpl(assistant);
  //]
  //[create_offers_chain_instantiate_component
  scs::core::ComponentId id = {"name", 1, 0, 0, "C++"};
  scs::core::ComponentContext component(assistant.orb(), id);
  //]
  //[create_offers_chain_add_facet
  component.addFacet("hello", _tc_Hello->id(), &helloImpl);
  //]
  //[create_offers_chain_register_service
  assistant.registerService(component.getIComponent());
  //]
  //[create_offers_chain_run
  assistant.orb()->run();
  //]
}
//]
