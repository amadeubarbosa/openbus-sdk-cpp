// -*- coding: iso-8859-1-unix -*-
#include "openbus/orb_initializer.hpp"
#include "openbus/log.hpp"
#include "openbus/openbus_context.hpp"
#include "openbus/detail/interceptors/orb_initializer.hpp"
#include "openbus/detail/interceptors/server.hpp"
#include "openbus/detail/interceptors/client.hpp"

#include <tao/ORBInitializer_Registry.h>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
  #include <boost/thread/once.hpp>
#endif
#include <boost/make_shared.hpp>

#include <memory>

namespace openbus 
{
// Para GCC compilando com C++11, nós fazemos uso de magic statics (thread-safe local statics)
// usando o mesmo código para singlethread. MSVC, mesmo na versão 11 ainda não suporta
// magic statics, então usamos call_once para initialização segura.
#if defined(OPENBUS_SDK_THREAD) && !(defined(__GNUC__) && __cplusplus == 201103L)
namespace {

log_type& get_log()
{
  log_type l;
  return l;
}

inline void init_log()
{
  static BOOST_ONCE_FLAG f;
  boost::call_once(&get_log, f);
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

#ifdef OPENBUS_SDK_MULTITHREAD
boost::mutex _mutex;
#endif

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

boost::shared_ptr<orb_ctx> ORBInitializer(int &argc, char **argv) 
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);
#endif
  log_scope l(log().general_logger(), info_level, "ORBInitializer");
  if (!orb_initializer.in())
  {
    orb_initializer = new interceptors::ORBInitializer;
  }
  PortableInterceptor::register_orb_initializer(orb_initializer.in());
  /* [obs] Mico 2.3.13 só permite a criação de apenas *um* ORB.
   * *CORBA garante que cada chamada a CORBA::ORB_init(argc, argv, "")
   * retorna o mesmo ORB.
  */
  CORBA::ORB_var orb(CORBA::ORB_init(argc, argv));
  try 
  {
    ACE_Time_Value t(0);
    orb->resolve_initial_references("OpenBusContext", &t);
    l.log("Este ORB ja foi criado.");
  } 
  catch (const CORBA::ORB::InvalidName &) 
  {
    interceptors::ORBInitializer *_orb_initializer
      (dynamic_cast<interceptors::ORBInitializer *>(orb_initializer.in()));
    assert(_orb_initializer != 0);
    
    CORBA::Object_var bus_ctx_obj(
      new OpenBusContext(orb, _orb_initializer));
    l.level_log(debug_level, "Registrando OpenBusContext");
    orb->register_initial_reference("OpenBusContext", bus_ctx_obj);

    interceptors::ClientInterceptor *cln_int(
      dynamic_cast<interceptors::ClientInterceptor *>(
        _orb_initializer->cln_interceptor.in()));
    cln_int->_bus_ctx = dynamic_cast<OpenBusContext *>(bus_ctx_obj.in());
    assert(cln_int->_bus_ctx != 0);
    
    interceptors::ServerInterceptor *srv_int(
      dynamic_cast<interceptors::ServerInterceptor *>(
        _orb_initializer->srv_interceptor.in()));
    srv_int->_bus_ctx = dynamic_cast<OpenBusContext *>(bus_ctx_obj.in());
    assert(srv_int->_bus_ctx != 0);
  }
  l.log("Retornando ORB");
  return boost::make_shared<orb_ctx>(orb);
}
}
