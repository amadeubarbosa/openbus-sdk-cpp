
#include <openbus/assistant.h>

int main(int argc, char** argv)
{
  openbus::assistant::Assistant assistant
    = openbus::assistant::Assistant::createWithPassword
    ("demo", "demo", "localhost", 2089, argc, argv);

  assistant.waitLogin();
  assistant.shutdown();
}
