#include <memory>

#include <openbus.h>
#ifdef OPENBUS_SDK_MULTITHREAD
#include <util/mutex.h>
#endif

#include <log.h>

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
    log_scope function_scope(log.general_logger(), info_level, "openbus::initORB");
    if (!singleORB) {
      function_scope.level_log(debug_level, "Nenhum ORB já criado, criando um ORB");
      /* [doubt] se eu receber uma exceção após a construção do orbInitializer, quem 
      ** vai liberar a memória do orbInitializer ? O destrutor não pode fazer 
      ** isso... P.S.: Acho que não posso liberar o orbInitializer, pelo menos 
      ** no Mico.
      */
      if (!orbInitializer) {
        function_scope.level_log(debug_level, "Nenhum ORB já criado, criando um ORB");
        orbInitializer = new interceptors::ORBInitializer();
        PortableInterceptor::register_orb_initializer(orbInitializer);
      }
      /* [obs] Mico 2.3.13 só permite a criação de apenas *um* ORB.
      ** CORBA garante que cada chamada a CORBA::ORB_init(argc, argv, "") retorna o mesmo ORB.
      */
      CORBA::ORB* orb = CORBA::ORB_init(argc, argv);
      function_scope.level_log(debug_level, "Criando ConnectionManager para ORB");
      ConnectionManager* manager = new ConnectionManager;
      manager->orb(orb);
      manager->_orbInitializer = orbInitializer;
      function_scope.level_log(debug_level, "Registrando ConnectionManager no ORB através de register_initial_reference");
      orb->register_initial_reference(CONNECTION_MANAGER_ID, manager);
      /* [obs]
      ** É necessário ativar o POA para evitar uma deadlock distribuído, que pode por exemplo ser 
      ** exercitado com uma chamada offer_registry::registerService(), em que  o  servidor  chama 
      ** um getComponentId() ao servant que é responsável pelo serviço que está sendo registrado.
      ** [doubt] 
      ** A responsabilidade desta ativação deve ser do SDK ou do SCS?
      */
      CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
      PortableServer::POAManager_var poa_manager = poa->the_POAManager();
      function_scope.level_log(debug_level, "Ativando o RootPOA");
      poa_manager->activate();
      orbInitializer->getClientInterceptor()->setConnectionManager(manager);
      orbInitializer->getServerInterceptor()->setConnectionManager(manager);
      singleORB = orb;
    }
    function_scope.log("Retornando ORB único já criado");
    return singleORB;
  }

  ConnectionManager* getConnectionManager(CORBA::ORB* orb) {
    log_scope function_scope(log.general_logger(), info_level
                             , "openbus::getConnectionManager");
    CORBA::Object* o;
    try {
      function_scope.level_log(debug_level, "Fazendo resolve_initial_references");
      o = orb->resolve_initial_references(CONNECTION_MANAGER_ID);
      if(CORBA::is_nil(o))
        function_scope.level_log(error_level, "ConnectionManager retornado de "
                                 "resolve_initial_references é nulo");
    } catch(CORBA::Exception& e) {
      function_scope.level_log(error_level, "Uma exceção foi lançada ao executar resolve_initial_references para retornar ConnectionManager associado ao ORB");
      return 0;
    }
    return  dynamic_cast<ConnectionManager*> (o);
  }
}
