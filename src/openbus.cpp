#include "openbus.h"
#include <memory>

namespace openbus {
  ORB* singleORB;

  /* [obs]
  ** Eu n�o consegui usar um auto_ptr para segurar a refer�ncia ao orbInitializer porque 
  ** tive problemas no t�rmino do programa com rela��o a destrui��o do objeto. Parece que 
  ** o Mico deve ficar com o ownership por conta de uma vetor de ORBInitializer_var que 
  ** ele possui em mico/pi_impl.h
  ** VERIFICAR se o Mico est� liberando o objeto.
  */
  /** Inicializador do ORB. */
  interceptors::ORBInitializer* orbInitializer;
  
  ORB::ORB(int argc, char** argv) {
    /* [doubt] se eu receber uma exce��o ap�s a constru��o do orbInitializer, quem 
    ** vai liberar a mem�ria do orbInitializer ? O destrutor n�o pode fazer 
    ** isso... P.S.: Acho que n�o posso liberar o orbInitializer, pelo menos 
    ** no Mico.
    */
    if (!orbInitializer) {
      orbInitializer = new interceptors::ORBInitializer();
      PortableInterceptor::register_orb_initializer(orbInitializer);
    }
    /* [doubt] Mico 2.3.13 s� permite a cria��o de apenas *um* ORB.
    ** CORBA garante que cada chamada a CORBA::ORB_init(argc, argv, "") retorna o mesmo ORB.
    */
    _orb = CORBA::ORB_init(argc, argv);
    /* [obs]
    ** � necess�rio ativar o POA para evitar uma deadlock distribu�do, que pode por exemplo ser 
    ** exercitado com uma chamada offer_registry::registerService(), em que  o  servidor  chama 
    ** um getComponentId() ao servant que � respons�vel pelo servi�o que est� sendo registrado.
    ** [doubt] 
    ** A responsabilidade desta ativa��o deve ser do SDK ou do SCS?
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
