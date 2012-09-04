
#include <openbus/assistant.h>

int main(int argc, char** argv)
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "demo", _password = "demo"
     , _argc = argc, _argv = argv);

  namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;

  idl_or::ServiceOfferDescSeq offers
    = assistant.findOffers(assistant.createFacetAndEntityProperty("hello", "notexistantentity"), 0);
}
