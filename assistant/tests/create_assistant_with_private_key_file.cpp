
#include <openbus/assistant.h>
#include <openbus/assistant/waitlogin.h>

#include <fstream>

int main(int argc, char** argv)
{
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
     , _private_key_filename = argv[argc-1]
     , _argc = argc, _argv = argv, _log_level = logger::debug_level);

  waitLogin(assistant);
  assistant.shutdown();
}
