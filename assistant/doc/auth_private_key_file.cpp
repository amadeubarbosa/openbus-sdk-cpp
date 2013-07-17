// -*- coding: iso-8859-1-unix -*-

#include <openbus/assistant.hpp>

#include <fstream>

int main(int argc, char* argv[])
{
  //[auth_private_key_file
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _entity = "tutorial"
     , _private_key_filename = "filename.key");
  //]
}
