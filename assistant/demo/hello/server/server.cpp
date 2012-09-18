#include <openbus/assistant.h>
#include "hello.h"

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace simple = tecgraf::openbus::interop::simple;

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello
{
  void sayHello()
  {
    std::cout << "Hello" << std::endl;
  }
};

int main(int argc, char** argv)
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "demo", _password = "demo"
     , _argc = argc, _argv = argv);
  
  scs::core::ComponentId componentId = { "Hello",  '1', '0', '0', "" };
  scs::core::ComponentContext hello_component (assistant.orb(), componentId);
  HelloImpl hello_servant;
  hello_component.addFacet("hello", simple::_tc_Hello->id(), &hello_servant);
    
  offer_registry::ServicePropertySeq properties;
  properties.length(1);
  properties[0].name = "offer.domain";
  properties[0].value = "Demos";
  assistant.addOffer(hello_component.getIComponent(), properties);

  assistant.wait();
}

