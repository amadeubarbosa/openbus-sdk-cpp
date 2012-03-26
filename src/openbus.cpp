#include "openbus.h"
#include <memory>

namespace openbus {
  ORB* singleORB;

  /* [obs]
  ** Eu não consegui usar um auto_ptr para segurar a referência ao orbInitializer porque 
  ** tive problemas no término do programa com relação a destruição do objeto. Parece que 
  ** o Mico deve ficar com o ownership por conta de uma vetor de ORBInitializer_var que 
  ** ele possui em mico/pi_impl.h
  ** VERIFICAR se o Mico está liberando o objeto.
  */
  /** Inicializador do ORB. */
  interceptors::ORBInitializer* orbInitializer;
  
  ORB::ORB(int argc, char** argv) {
    /* [doubt] se eu receber uma exceção após a construção do orbInitializer, quem 
    ** vai liberar a memória do orbInitializer ? O destrutor não pode fazer 
    ** isso... P.S.: Acho que não posso liberar o orbInitializer, pelo menos 
    ** no Mico.
    */
    if (!orbInitializer) {
      orbInitializer = new interceptors::ORBInitializer();
      PortableInterceptor::register_orb_initializer(orbInitializer);
    }
    /* [doubt] Mico 2.3.13 só permite a criação de apenas *um* ORB.
    ** CORBA garante que cada chamada a CORBA::ORB_init(argc, argv, "") retorna o mesmo ORB.
    */
    _orb = CORBA::ORB_init(argc, argv);
    /* [obs]
    ** É necessário ativar o POA para evitar uma deadlock distribuído, que pode por exemplo ser 
    ** exercitado com uma chamada offer_registry::registerService(), em que  o  servidor  chama 
    ** um getComponentId() ao servant que é responsável pelo serviço que está sendo registrado.
    ** [doubt] 
    ** A responsabilidade desta ativação deve ser do SDK ou do SCS?
    */
    CORBA::Object_var o = _orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
    PortableServer::POAManager_var poa_manager = poa->the_POAManager();
    poa_manager->activate();
    
    _multiplexer = new multiplexed::ConnectionMultiplexer;
  }
  
  ORB::~ORB() { }
  
  ORB* createORB(int argc, char** argv) throw(CORBA::Exception) {
    return new ORB(argc, argv);
  }
  
  Connection* connect(const std::string host, const unsigned int port, ORB* orb) 
    throw(CORBA::Exception) 
  {
    if (!orb)
      //[doubt] posso inicializar com 0, 0 ?
      singleORB = createORB(0, 0);
    else
      singleORB = orb;
    return new Connection(host, port, singleORB, orbInitializer);
  }
}
