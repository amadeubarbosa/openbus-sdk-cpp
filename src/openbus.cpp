#include <openbus.h>
#include <log.h>
#ifdef OPENBUS_SDK_MULTITHREAD
#include <util/mutex.h>
#endif
#include <interceptors/orbInitializer_impl.h>

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
  CORBA::ORB* singleORB;
  #ifdef OPENBUS_SDK_MULTITHREAD
  MICOMT::Mutex _mutex;
  #endif
  
  CORBA::ORB* initORB(int argc, char** argv) throw(CORBA::Exception) {
    #ifdef OPENBUS_SDK_MULTITHREAD
    Mutex m(&_mutex);
    #endif
    log_scope l(log.general_logger(), info_level, "openbus::initORB");
    if (!singleORB) {
      /* [doubt] se eu receber uma exceção após a construção do orbInitializer, quem 
      ** vai liberar a memória do orbInitializer ? O destrutor não pode fazer 
      ** isso... P.S.: Acho que não posso liberar o orbInitializer, pelo menos 
      ** no Mico.
      */
      if (!orbInitializer) {
        orbInitializer = new interceptors::ORBInitializer();
        PortableInterceptor::register_orb_initializer(orbInitializer);
      }
      /* [obs] Mico 2.3.13 só permite a criação de apenas *um* ORB.
      ** CORBA garante que cada chamada a CORBA::ORB_init(argc, argv, "") retorna o mesmo ORB.
      */
      CORBA::ORB* orb = CORBA::ORB_init(argc, argv);
      ConnectionManager* manager = new ConnectionManager(orb, orbInitializer);
      l.level_log(debug_level, "Registrando ConnectionManager");
      orb->register_initial_reference(CONNECTION_MANAGER_ID, manager);
      /* [obs]
      ** É necessário ativar o POA para evitar uma deadlock distribuído, que pode por exemplo ser 
      ** exercitado com uma chamada offer_registry::registerService(), em que  o  servidor  chama 
      ** um getComponentId() ao servant que é responsável pelo serviço que está sendo registrado.
      ** [doubt] 
      ** A responsabilidade desta ativação deve ser do SDK ou do SCS?
      */
      CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
      assert(!CORBA::is_nil(o));
      PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
      PortableServer::POAManager_var poa_manager = poa->the_POAManager();
      l.level_log(debug_level, "Ativando RootPOA");
      poa_manager->activate();
      orbInitializer->getClientInterceptor()->setConnectionManager(manager);
      orbInitializer->getServerInterceptor()->setConnectionManager(manager);
      singleORB = orb;
    }
    l.log("Retornando ORB");
    return singleORB;
  }
}
