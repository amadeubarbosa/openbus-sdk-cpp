// -*- coding: iso-8859-1-unix -*-

#include <config.hpp>
#include <openbus.hpp>

int main(int argc, char* argv[])
{
  namespace cfg = openbus::tests::config;
  cfg::ctx_t ctx(cfg::init(argc, argv));
  openbus::OpenBusContext *bus_ctx(ctx.second);
  boost::shared_ptr<openbus::orb_ctx> orb_ctx(ctx.first);

  if (bus_ctx->orb() != orb_ctx->orb().in())
  {
    std::cerr << "bus_ctx->orb() != orb.in()" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
