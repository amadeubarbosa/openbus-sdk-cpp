// -*- coding: iso-8859-1-unix -*-
#include "openbus/ORBInitializer.hpp"
#include "openbus/log.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/interceptors/ORBInitializer_impl.hpp"
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/interceptors/ClientInterceptor_impl.hpp"

#include <tao/ORBInitializer_Registry.h>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
  #include <boost/thread/once.hpp>
#endif

#include <memory>

namespace openbus 
{
// Para GCC compilando com C++11, n�s fazemos uso de magic statics (thread-safe local statics)
// usando o mesmo c�digo para singlethread. MSVC, mesmo na vers�o 11 ainda n�o suporta
// magic statics, ent�o usamos call_once para initializa��o segura.
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

CORBA::ORB_ptr ORBInitializer(int &argc, char **argv) 
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
  /* [obs] Mico 2.3.13 s� permite a cria��o de apenas *um* ORB.
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
      new OpenBusContext(orb, _orb_initializer->_orb_info));
    l.level_log(debug_level, "Registrando OpenBusContext");
    orb->register_initial_reference("OpenBusContext", bus_ctx_obj);
    _orb_initializer->clientInterceptor->_bus_ctx_obj = bus_ctx_obj;
    _orb_initializer->clientInterceptor->_bus_ctx =
      dynamic_cast<OpenBusContext *>(bus_ctx_obj.in());
    assert(_orb_initializer->clientInterceptor->_bus_ctx != 0);
    _orb_initializer->serverInterceptor->_bus_ctx_obj = bus_ctx_obj;
    _orb_initializer->serverInterceptor->_bus_ctx =
      dynamic_cast<OpenBusContext *>(bus_ctx_obj.in());
    assert(_orb_initializer->serverInterceptor->_bus_ctx != 0);
  }
  l.log("Retornando ORB");
  return orb._retn();
}
}
