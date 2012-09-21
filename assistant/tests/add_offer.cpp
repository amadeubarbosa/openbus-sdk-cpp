
#include <openbus/assistant.h>
#include <scs/ComponentContext.h>

#include <hello.h>

int main(int argc, char** argv)
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "demo", _password = "demo"
     , _argc = argc, _argv = argv);

  namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;

  scs::core::ComponentId id = {"name", 1, 0, 0, "C++"};
  {
    scs::core::ComponentContext component(assistant.orb(), id);
    assistant.addOffer(component.getIComponent());
    assistant.wait();
  }

  assistant.shutdown();
}
