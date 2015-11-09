// -*- coding: iso-8859-1-unix -*-

#include <openbus.hpp>

int main(int argc, char* argv[])
{
  std::auto_ptr<openbus::orb_ctx>
    orb_ctx(openbus::ORBInitializer(argc, argv));
  CORBA::Object_var
    obj(orb_ctx->orb()->resolve_initial_references("OpenBusContext"));
  openbus::OpenBusContext
    *bus_ctx(dynamic_cast<openbus::OpenBusContext *>(obj.in()));
  if (bus_ctx->orb() != orb_ctx->orb().in())
  {
    std::cerr << "bus_ctx->orb() != orb.in()" << std::endl;
    std::abort();
  }
  return 0; //MSVC
}
