// -*- coding: iso-8859-1-unix -*-
#include "openbus/ORBInitializer.hpp"
#include "openbus/log.hpp"
#include "openbus/OpenBusContext.hpp"
#include "openbus/interceptors/ORBInitializer_impl.hpp"
#include "openbus/interceptors/ServerInterceptor_impl.hpp"
#include "openbus/interceptors/ClientInterceptor_impl.hpp"

#include <TAO/ORBInitializer_Registry.h>
#ifdef OPENBUS_SDK_MULTITHREAD
  #include <boost/thread.hpp>
  #include <boost/thread/once.hpp>
#endif
#include <memory>

namespace openbus 
{
// Para GCC compilando com C++11, nós fazemos uso de magic statics (thread-safe local statics)
// usando o mesmo código para singlethread. MSVC, mesmo na versão 11 ainda não suporta
// magic statics, então usamos call_once para initialização segura.
#if defined(OPENBUS_SDK_THREAD) && !(defined(__GNUC__) && __cplusplus == 201103L)
namespace {

log_type &get_log()
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

OPENBUS_SDK_DECL log_type &log()
{
  init_log();
  return get_log();
}
#else
OPENBUS_SDK_DECL log_type &log()
{
  static log_type l;
  return l;
}
#endif

interceptors::ORBInitializer orbInitializer;

#ifdef OPENBUS_SDK_MULTITHREAD
boost::mutex _mutex;
#endif

CORBA::ORB_ptr ORBInitializer(int &argc, char **argv) 
{
#ifdef OPENBUS_SDK_MULTITHREAD
  boost::lock_guard<boost::mutex> lock(_mutex);
#endif
  log_scope l(log().general_logger(), info_level, "ORBInitializer");
  PortableInterceptor::register_orb_initializer(&orbInitializer);
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
    boost::shared_ptr<OpenBusContext> openbusContext
      (new OpenBusContext(orb, orbInitializer._orb_info));
    l.level_log(debug_level, "Registrando OpenBusContext");
    orb->register_initial_reference("OpenBusContext", openbusContext.get());
    orbInitializer.clientInterceptor->_openbus_ctx = openbusContext;
    orbInitializer.serverInterceptor->_openbus_ctx = openbusContext;
  }
  l.log("Retornando ORB");
  return orb._retn();
}
}
