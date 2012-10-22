
#include <openbus/assistant.h>
#include <openbus/assistant/waitlogin.h>

int main(int argc, char** argv)
{
  using namespace openbus::assistant::keywords;
  openbus::assistant::Assistant assistant
    ("localhost", 2089, _username = "demo", _password = "demo"
     , _argc = argc, _argv = argv, _log_level = logger::debug_level);

  waitLogin(assistant);
  assistant.shutdown();
}
