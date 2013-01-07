#include <openbus/assistant.h>
#include <scs/ComponentContext.h>
#include <iostream>

#include <stubs/independent_clock.h>
#include <CORBA.h>
#include <time.h>

#ifndef OPENBUS_SDK_MULTITHREAD
#error This demo requires multithread
#endif

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <boost/program_options.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

void mysleep()
{
#ifndef _WIN32
  unsigned int t = 30u;
  do { t = sleep(t); } while(t);
#else
  Sleep(3000);
#endif
}

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

void run_orb(CORBA::ORB_var orb)
{
  orb->run();
}

void clock_loop()
{
  while(true)
  {
    std::cout << "Hora local atual em ticks: " << time(0) << std::endl;
    mysleep();
  }
}

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

  boost::thread orb_thread(boost::bind(&run_orb, assistant.orb()));

  scs::core::ComponentId componentId
    = { "IndependentClock", '1', '0', '0', ""};
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

  boost::thread local_clock_thread(& ::clock_loop);
  
  orb_thread.join();
  local_clock_thread.join();
}
