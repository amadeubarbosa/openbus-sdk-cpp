// -*- coding: iso-8859-1-unix -*-

#include <openbus/assistant.hpp>

int main()
{
  //[namedparameters_host_port
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "tutorial"
     , _password = "tutorial");
  //]
}
