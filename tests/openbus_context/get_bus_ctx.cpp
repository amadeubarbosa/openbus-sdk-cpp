// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char* argv[])
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);

  if (bus_ctx == 0)
  {
    std::cerr << "bus_ctx == 0" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
