#include <openbus.h>
#include <log.h>
#ifdef OPENBUS_SDK_MULTITHREAD
#include <util/autolock_impl.h>
#endif
#include <manager.h>
#include <interceptors/orbInitializer_impl.h>
#include <interceptors/clientInterceptor_impl.h>
#include <interceptors/serverInterceptor_impl.h>

#include <memory>

namespace openbus {
log_type log;

/* [obs]
** Eu não consegui usar um auto_ptr para segurar a referência ao orbInitializer porque 
** tive problemas no término do programa com relação a destruição do objeto. Parece que 
** o Mico deve ficar com o ownership por conta de uma vetor de ORBInitializer_var que 
** ele possui em mico/pi_impl.h
** VERIFICAR se o Mico está liberando o objeto.
*/
interceptors::ORBInitializer* orbInitializer;
MICOMT::Mutex _mutex;

CORBA::ORB* initORB(int argc, char** argv) throw(CORBA::Exception) {
  AutoLock m(&_mutex);
  log_scope l(log.general_logger(), info_level, "initORB");
  if (!orbInitializer) {
    orbInitializer = new interceptors::ORBInitializer();
    PortableInterceptor::register_orb_initializer(orbInitializer);
  }
  /* [obs] Mico 2.3.13 só permite a criação de apenas *um* ORB.
  ** CORBA garante que cada chamada a CORBA::ORB_init(argc, argv, "") retorna o mesmo ORB.
  */
  CORBA::ORB* orb = CORBA::ORB_init(argc, argv);
  try {
    orb->resolve_initial_references(CONNECTION_MANAGER_ID);
    l.log("Este ORB ja foi criado.");
  } catch(CORBA::ORB_InvalidName&) {
    ConnectionManager* manager = new ConnectionManager(orb, orbInitializer);
    l.level_log(debug_level, "Registrando ConnectionManager");
    orb->register_initial_reference(CONNECTION_MANAGER_ID, manager);
    orbInitializer->clientInterceptor()->connectionManager(manager);
    orbInitializer->serverInterceptor()->connectionManager(manager);
  }
  l.log("Retornando ORB");
  return orb;
}
}
