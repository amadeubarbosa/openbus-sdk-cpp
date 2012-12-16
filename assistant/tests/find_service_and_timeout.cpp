
#include <openbus/assistant.h>

#include <boost/chrono/include.hpp>

int main(int argc, char** argv)
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "demo", _password = "demo"
     , _argc = argc, _argv = argv, _log_level = logger::debug_level);

  namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;

  idl_or::ServiceOfferDescSeq offers
    = assistant.findServices(assistant.createFacetAndEntityProperty("hello", "notexistantentity")
                             , 1);
  assert(offers.length() == 0);
}
