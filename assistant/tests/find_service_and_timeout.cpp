
#include <openbus/assistant.h>

#include <boost/chrono/include.hpp>

int main(int argc, char** argv)
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "demo", _password = "demo"
     , _argc = argc, _argv = argv);

  namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;

  boost::chrono::steady_clock::time_point before
    = boost::chrono::steady_clock::now();

  try
  {
    idl_or::ServiceOfferDescSeq offers
      = assistant.findOffers(assistant.createFacetAndEntityProperty("hello", "notexistantentity")
                             , 120u);
    std::abort();
  }
  catch(openbus::assistant::timeout_error const&)
  {
    boost::chrono::steady_clock::time_point after = boost::chrono::steady_clock::now();
    assert(after >= before + boost::chrono::seconds(90)
           && after <= before + boost::chrono::seconds(150));
    assistant.shutdown();
  }
  catch(...)
  {
    std::abort();
  }
}
