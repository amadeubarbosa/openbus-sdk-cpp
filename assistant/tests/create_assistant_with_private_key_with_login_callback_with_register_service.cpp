
#include <openbus/assistant.hpp>
#include <openbus/assistant/waitlogin.hpp>

#include <stubs/hello.h>

#include <boost/bind.hpp>

#include <fstream>
#include <iostream>

void login_error()
{
  std::cout << "=== Error while logging in (callback) ===" << std::endl;
}

int main(int argc, char** argv)
{
  CORBA::OctetSeq key;
  if(argc < 2)
  {
    std::cout << "Test must be run with a command line" 
      " parameter with the name of the file that contains a binary key"
              << std::endl;
    return 1;
  }

  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _entity = "TestCppHello"
     , _private_key = openbus::PrivateKey(argv[argc-1])
     , _argc = argc, _argv = argv, _log_level = logger::debug_level
     , _on_login_error = boost::bind( &::login_error));

  namespace idl_or = tecgraf::openbus::core::v2_0::services::offer_registry;

  scs::core::ComponentId id = {"name", 1, 0, 0, "C++"};
  {
    scs::core::ComponentContext component(assistant.orb(), id);
    assistant.registerService(component.getIComponent());
    waitLogin(assistant);
  }
}
