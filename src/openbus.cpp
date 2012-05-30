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
  ** Eu n�o consegui usar um auto_ptr para segurar a refer�ncia ao orbInitializer porque 
  ** tive problemas no t�rmino do programa com rela��o a destrui��o do objeto. Parece que 
  ** o Mico deve ficar com o ownership por conta de uma vetor de ORBInitializer_var que 
  ** ele possui em mico/pi_impl.h
  ** VERIFICAR se o Mico est� liberando o objeto.
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
      /* [doubt] se eu receber uma exce��o ap�s a constru��o do orbInitializer, quem 
      ** vai liberar a mem�ria do orbInitializer ? O destrutor n�o pode fazer 
      ** isso... P.S.: Acho que n�o posso liberar o orbInitializer, pelo menos 
      ** no Mico.
      */
      if (!orbInitializer) {
        orbInitializer = new interceptors::ORBInitializer();
        PortableInterceptor::register_orb_initializer(orbInitializer);
      }
      /* [obs] Mico 2.3.13 s� permite a cria��o de apenas *um* ORB.
      ** CORBA garante que cada chamada a CORBA::ORB_init(argc, argv, "") retorna o mesmo ORB.
      */
      CORBA::ORB* orb = CORBA::ORB_init(argc, argv);
      ConnectionManager* manager = new ConnectionManager(orb, orbInitializer);
      l.level_log(debug_level, "Registrando ConnectionManager");
      orb->register_initial_reference(CONNECTION_MANAGER_ID, manager);
      /* [obs]
      ** � necess�rio ativar o POA para evitar uma deadlock distribu�do, que pode por exemplo ser 
      ** exercitado com uma chamada offer_registry::registerService(), em que  o  servidor  chama 
      ** um getComponentId() ao servant que � respons�vel pelo servi�o que est� sendo registrado.
      ** [doubt] 
      ** A responsabilidade desta ativa��o deve ser do SDK ou do SCS?
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
