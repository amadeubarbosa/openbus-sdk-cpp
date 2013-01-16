
#include <openbus/assistant.hpp>

#include <fstream>

int main(int argc, char* argv[])
{
  //[auth_private_key
  CORBA::OctetSeq key/*= = GetPrivateKey()*/;
  /*<-*/
  {
    std::ifstream key_file(argv[argc-1]);
    key_file.seekg(0, std::ios::end);
    std::size_t size = key_file.tellg();
    key_file.seekg(0, std::ios::beg);
    key.length(size);
    key_file.rdbuf()->sgetn(static_cast<char*>(static_cast<void*>(key.get_buffer())), size);
  }
  /*->*/
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _entity = "tutorial"
     , _private_key = key);
  //]
}
