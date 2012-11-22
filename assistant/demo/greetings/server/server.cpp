#include <openbus/assistant.h>
#include <scs/ComponentContext.h>
#include <iostream>

#include <stubs/greetings.h>
#include <CORBA.h>

#ifdef OPENBUS_SDK_MULTITHREAD
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#endif

#include <boost/program_options.hpp>

namespace offer_registry
 = tecgraf::openbus::core::v2_0::services::offer_registry;
namespace services = tecgraf::openbus::core::v2_0::services;
namespace access_control
 = tecgraf::openbus::core::v2_0::services::access_control;

struct GreetingsImpl : virtual public ::POA_Greetings
{
  GreetingsImpl(std::string const& greeting)
    : greeting(greeting) {}
  char* sayGreetings()
  {
    return CORBA::string_dup(greeting.c_str());
  }
  std::string greeting;
};

#ifdef OPENBUS_SDK_MULTITHREAD
void run_orb(CORBA::ORB_var orb)
{
  orb->run();
}
#endif

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

  // Inicializando CORBA e ativando o RootPOA
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _entity = "demo"
     , _private_key_filename = private_key_filename
     , _argc = argc, _argv = argv);

#ifdef OPENBUS_SDK_MULTITHREAD
  boost::thread orb_thread(boost::bind(&run_orb, assistant.orb()));
#endif

  scs::core::ComponentId componentId = { "Greetings", '1', '0', '0', "" };
  scs::core::ComponentContext english_greetings_component(assistant.orb(), componentId);
  GreetingsImpl english_greetings_servant("Hello");
  english_greetings_component.addFacet
    ("greetings", _tc_Greetings->id(), &english_greetings_servant);

  scs::core::ComponentContext portuguese_greetings_component(assistant.orb(), componentId);
  GreetingsImpl portuguese_greetings_servant("Olá");
  portuguese_greetings_component.addFacet
    ("greetings", _tc_Greetings->id(), &portuguese_greetings_servant);

  scs::core::ComponentContext german_greetings_component(assistant.orb(), componentId);
  GreetingsImpl german_greetings_servant("Guten Tag");
  german_greetings_component.addFacet
    ("greetings", _tc_Greetings->id(), &german_greetings_servant);
    
  offer_registry::ServicePropertySeq properties;
  properties.length(2);
  properties[0].name = "offer.domain";
  properties[0].value = "Demos";
  properties[1].name = "language";
  properties[1].value = "english";
  assistant.registerService(english_greetings_component.getIComponent(), properties);

  properties[1].name = "language";
  properties[1].value = "portuguese";
  assistant.registerService(portuguese_greetings_component.getIComponent(), properties);

  properties[1].name = "language";
  properties[1].value = "german";
  assistant.registerService(german_greetings_component.getIComponent(), properties);

#ifdef OPENBUS_SDK_MULTITHREAD
  orb_thread.join();
#else
  assistant.orb()->run();
#endif
}

