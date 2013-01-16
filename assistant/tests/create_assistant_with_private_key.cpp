
#include <openbus/assistant.hpp>
#include <openbus/assistant/waitlogin.hpp>

#include <fstream>

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

  {
    std::ifstream key_file(argv[argc-1]);
    key_file.seekg(0, std::ios::end);
    std::size_t size = key_file.tellg();
    key_file.seekg(0, std::ios::beg);
    key.length(size);
    key_file.rdbuf()->sgetn(static_cast<char*>(static_cast<void*>(key.get_buffer())), size);
  }

  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _entity = "TestCppHello"
     , _private_key = key
     , _argc = argc, _argv = argv, _log_level = logger::debug_level);

  waitLogin(assistant);
  assistant.shutdown();
}
