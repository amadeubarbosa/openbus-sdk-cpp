// -*- coding: iso-8859-1-unix -*-

#include <openbus/assistant.hpp>

int main()
{
  //[namedparameters
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    (_host = "localhost", _port = 2089, _username = "tutorial"
     , _password = "tutorial");
  //]
}
