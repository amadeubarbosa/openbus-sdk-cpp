// -*- coding: iso-8859-1-unix -*-
#include "openbus/orb_initializer.hpp"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-local-typedef"
#include "openbus/log.hpp"
#pragma clang diagnostic pop
#include "openbus/openbus_context.hpp"
#include "openbus/detail/interceptors/orb_initializer.hpp"

#include <tao/ORBInitializer_Registry.h>
#include <boost/thread.hpp>
#include <boost/thread/once.hpp>
#include <boost/make_shared.hpp>

#include <memory>

namespace openbus 
{
// Para GCC compilando com C++11, nós fazemos uso de magic statics (thread-safe local statics)
// usando o mesmo código para singlethread. MSVC, mesmo na versão 11 ainda não suporta
// magic statics, então usamos call_once para initialização segura.
#if !(defined(__GNUC__) && __cplusplus == 201103L)
namespace {

log_type& get_log()
{
  static log_type l;
  return l;
}

inline void init_log()
{
  static boost::once_flag once = BOOST_ONCE_INIT;
  boost::call_once<log_type &(*)()>(once, &get_log);
}

}

OPENBUS_SDK_DECL log_type& log()
{
  init_log();
  return get_log();
}
#else
OPENBUS_SDK_DECL log_type& log()
{
  static log_type l;
  return l;
}
#endif

PortableInterceptor::ORBInitializer_var orb_initializer;

boost::mutex _mutex;

orb_ctx::orb_ctx(CORBA::ORB_var orb)
  : orb_(orb)
{
}

orb_ctx::~orb_ctx()
{
  try
  {
    orb_->destroy();
  }
  catch (...)
  {
  }  
}

std::auto_ptr<orb_ctx>
ORBInitializer(int &argc, char **argv) 
{
  boost::lock_guard<boost::mutex> lock(_mutex);
  log_scope l(log().general_logger(), info_level, "ORBInitializer");
  if (CORBA::is_nil(orb_initializer))
  {
    orb_initializer = new interceptors::ORBInitializer;
  }
  PortableInterceptor::register_orb_initializer(orb_initializer.in());
  CORBA::ORB_var orb(CORBA::ORB_init(argc, argv));
  try 
  {
    ACE_Time_Value t(0);
    orb->resolve_initial_references("OpenBusContext", &t);
    l.log("ORB OpenBus ja foi criado.");
  } 
  catch (const CORBA::ORB::InvalidName &) 
  {
    interceptors::ORBInitializer *_orb_initializer(
      dynamic_cast<interceptors::ORBInitializer *>(orb_initializer.in()));
    assert(_orb_initializer != 0);
    
    CORBA::Object_var bus_ctx_obj(
      new OpenBusContext(orb, _orb_initializer));
    orb->register_initial_reference("OpenBusContext", bus_ctx_obj);
  }
  return std::auto_ptr<orb_ctx>(new orb_ctx(orb));
}
}
