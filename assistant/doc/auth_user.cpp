// -*- coding: iso-8859-1-unix -*-

#include <openbus/assistant.hpp>

int main()
{
  //[auth_user
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "usuario"
     , _password = "senha");
  //]
}
