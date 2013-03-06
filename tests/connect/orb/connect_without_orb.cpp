
#include <openbus.h>
#include <configuration.h>

int main(int argc, char* argv[])
{
  openbus::configuration cfg(argc, argv);
  std::auto_ptr<openbus::Connection> conn(openbus::connect(cfg.host().c_str(), cfg.port()));
}
