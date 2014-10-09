// -*- coding: iso-8859-1-unix -*-
#include <openbus/assistant.hpp>
#include <helloS.h>

#include <boost/program_options.hpp>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace simple = tecgraf::openbus::interop::simple;

struct HelloImpl : virtual public POA_tecgraf::openbus::interop::simple::Hello
{
  void sayHello()
  {
    std::cout << "Hello!" << std::endl;
  }
};

int main(int argc, char **argv)
{
  std::string private_key_filename;
  {
    namespace po = boost::program_options;
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "This help message")
      ("private-key", po::value<std::string>(), "Path to private key")
      ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(vm.count("help") || !vm.count("private-key"))
    {
      std::cout << desc << std::endl;
      return 0;
    }
    private_key_filename = vm["private-key"].as<std::string>();
  }

  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _entity = "demo"
     , _private_key_filename = private_key_filename
     , _argc = argc, _argv = argv
     , _log_level = logger::debug_level);
  
  scs::core::ComponentId componentId = { "Hello",  '1', '0', '0', "" };
  scs::core::ComponentContext hello_component (assistant.orb(), componentId);
  HelloImpl hello_servant;
  hello_component.addFacet("hello", simple::_tc_Hello->id(), &hello_servant);
    
  offer_registry::ServicePropertySeq properties;
  properties.length(1);
  properties[0].name = "offer.domain";
  properties[0].value = "Demos";
  assistant.registerService(hello_component.getIComponent(), properties);

  assistant.orb()->run();
}
