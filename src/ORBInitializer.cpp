#include <memory>

#include "openbus/ORBInitializer.h"
#include "openbus/log.h"
#include "openbus/util/AutoLock_impl.h"
#include "openbus/OpenBusContext.h"
#include "openbus/interceptors/ORBInitializer_impl.h"
#include "openbus/interceptors/ClientInterceptor_impl.h"
#include "openbus/interceptors/ServerInterceptor_impl.h"


namespace openbus {
log_type log;

/* [obs] Eu não consegui usar um auto_ptr para segurar a referência ao orbInitializer porque tive
 * problemas no término do programa com relação a destruição do objeto. Parece que o Mico deve ficar
 * com o ownership por conta de uma vetor de ORBInitializer_var que ele possui em mico/pi_impl.h
 * VERIFICAR se o Mico está liberando o objeto.
*/
interceptors::ORBInitializer *orbInitializer;
Mutex _mutex;

CORBA::ORB *ORBInitializer(int argc, char **argv) {
  AutoLock m(&_mutex);
  log_scope l(log.general_logger(), info_level, "ORBInitializer");
  if (!orbInitializer) {
    orbInitializer = new interceptors::ORBInitializer();
    PortableInterceptor::register_orb_initializer(orbInitializer);
  }
  /* [obs] Mico 2.3.13 só permite a criação de apenas *um* ORB.  *CORBA garante que cada chamada a
   * CORBA::ORB_init(argc, argv, "") retorna o mesmo ORB.
  */
  CORBA::ORB *orb = CORBA::ORB_init(argc, argv);
  try {
    orb->resolve_initial_references("OpenBusContext");
    l.log("Este ORB ja foi criado.");
  } catch(CORBA::ORB_InvalidName &) {
    OpenBusContext *openbusContext = new OpenBusContext(
      orb, 
      orbInitializer->codec(), 
      orbInitializer->slotId_joinedCallChain(), 
      orbInitializer->slotId_signedCallChain(), 
      orbInitializer->slotId_legacyCallChain(), 
      orbInitializer->slotId_requesterConnection(),
      orbInitializer->slotId_receiveConnection());
    l.level_log(debug_level, "Registrando OpenBusContext");
    orb->register_initial_reference("OpenBusContext", openbusContext);
    orbInitializer->clientInterceptor()->openbusContext(openbusContext);
    orbInitializer->serverInterceptor()->openbusContext(openbusContext);
  }
  l.log("Retornando ORB");
  return orb;
}
}
