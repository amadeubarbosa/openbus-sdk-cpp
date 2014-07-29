// -*- coding: iso-8859-1-unix -*-
#include <openbus/assistant.hpp>
#include <scs/ComponentContext.h>
#include <iostream>

#include <stubs/dedicated_clock.h>
#include <CORBA.h>
#include <time.h>

#include <boost/program_options.hpp>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace demo = tecgraf::openbus::demo;
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control = tecgraf::openbus::core::v2_0::services::access_control;

struct ClockImpl : public POA_tecgraf::openbus::demo::Clock
{
  CORBA::Long getTimeInTicks()
  {
    return time(0);
  }
};

int main(int argc, char** argv)
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
     , _argc = argc, _argv = argv);

  scs::core::ComponentId componentId
    = { "DedicatedClock", '1', '0', '0', ""};
  scs::core::ComponentContext clock_component
    (assistant.orb(), componentId);
  ClockImpl clock_servant;
  clock_component.addFacet
    ("clock", demo::_tc_Clock->id(), &clock_servant);
  offer_registry::ServicePropertySeq properties;
  properties.length(1);
  properties[0].name = "offer.domain";
  properties[0].value = "Demos";
  assistant.registerService(clock_component.getIComponent(), properties);

  assistant.orb()->run();

  return 0;
}
