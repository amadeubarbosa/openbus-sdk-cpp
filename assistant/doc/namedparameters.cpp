
#include <openbus/assistant.h>

int main()
{
  //[namedparameters
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    (_host = "localhost", _port = 2089, _username = "tutorial"
     , _password = "tutorial");
  //]
}
