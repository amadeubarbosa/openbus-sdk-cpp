#include <memory>

#include <openbus.h>
#include <util/mutex.h>

namespace openbus {
  /* [obs]
  ** Eu n�o consegui usar um auto_ptr para segurar a refer�ncia ao orbInitializer porque 
  ** tive problemas no t�rmino do programa com rela��o a destrui��o do objeto. Parece que 
  ** o Mico deve ficar com o ownership por conta de uma vetor de ORBInitializer_var que 
  ** ele possui em mico/pi_impl.h
  ** VERIFICAR se o Mico est� liberando o objeto.
  */
  interceptors::ORBInitializer* orbInitializer;
  std::set<CORBA::ORB*> ORBSet;
  char* nullArgv[] = {(char*) " "};
  char* multithreadArgv[] = {(char*) " "};
  CORBA::ORB* singleORB;
  Connection* singleConnection;
#ifdef OPENBUS_SDK_MULTITHREAD
  MICOMT::Mutex _mutex;
#else
  void* _mutex;
#endif

  CORBA::ORB* createORB(int argc, char** argv) throw(CORBA::Exception) {
    Mutex m(&_mutex);
    if (!singleORB) {
      /* [doubt] se eu receber uma exce��o ap�s a constru��o do orbInitializer, quem 
      ** vai liberar a mem�ria do orbInitializer ? O destrutor n�o pode fazer 
      ** isso... P.S.: Acho que n�o posso liberar o orbInitializer no Mico.
      */
      if (!orbInitializer) {
        orbInitializer = new interceptors::ORBInitializer();
        PortableInterceptor::register_orb_initializer(orbInitializer);
      }
      /* [doubt] Mico 2.3.13 s� permite a cria��o de apenas *um* ORB.
      ** CORBA garante que cada chamada a CORBA::ORB_init(argc, argv, "") retorna o mesmo ORB.
      */
      CORBA::ORB* orb = CORBA::ORB_init(argc, argv);
      /* [obs]
      ** � necess�rio ativar o POA para evitar uma deadlock distribu�do, que pode por exemplo ser 
      ** exercitado com uma chamada offer_registry::registerService(), em que  o  servidor  chama 
      ** um getComponentId() ao servant que � respons�vel pelo servi�o que est� sendo registrado.
      ** [doubt] 
      ** A responsabilidade desta ativa��o deve ser do SDK ou do SCS?
      */
      CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
      PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
      PortableServer::POAManager_var poa_manager = poa->the_POAManager();
      poa_manager->activate();
      ORBSet.insert(orb);
      singleORB = orb;
    }
    return singleORB;
  }
  
  Connection* connect(const std::string host, const unsigned int port, CORBA::ORB* orb) 
    throw(CORBA::Exception, AlreadyConnected, InvalidORB) 
  {
    Mutex m(&_mutex);
    if (!singleORB) {
      if (!orb) {
      #ifdef OPENBUS_SDK_MULTITHREAD
        m.unlock();
        singleORB = createORB(2, multithreadArgv);
        m.lock();
      #else
        singleORB = createORB(1, nullArgv);
      #endif
      } else {
        if (ORBSet.find(orb) == ORBSet.end()) throw InvalidORB();
        if (multiplexed::getConnectionMultiplexer(orb)) throw InvalidORB();
        singleORB = orb;
      }
    }
    if (singleConnection && !singleConnection->_isClosed)
      throw AlreadyConnected();
    else
      singleConnection = new Connection(host, port, singleORB, orbInitializer);
    return singleConnection;
  }
  
  namespace multiplexed {
    CORBA::ORB* createORB(int argc, char** argv) throw(CORBA::Exception) {
      Mutex m(&_mutex);
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
        /* [doubt] Mico 2.3.13 s� permite a cria��o de apenas *um* ORB.
        ** CORBA garante que cada chamada a CORBA::ORB_init(argc, argv, "") retorna o mesmo ORB.
        */
        CORBA::ORB* orb = CORBA::ORB_init(argc, argv);
        multiplexed::ConnectionMultiplexer* multiplexer = new multiplexed::ConnectionMultiplexer;
        orb->register_initial_reference(CONNECTION_MULTIPLEXER_ID, multiplexer);
        /* [obs]
        ** � necess�rio ativar o POA para evitar uma deadlock distribu�do, que pode por exemplo ser 
        ** exercitado com uma chamada offer_registry::registerService(), em que  o  servidor  chama 
        ** um getComponentId() ao servant que � respons�vel pelo servi�o que est� sendo registrado.
        ** [doubt] 
        ** A responsabilidade desta ativa��o deve ser do SDK ou do SCS?
        */
        CORBA::Object_var o = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(o);
        PortableServer::POAManager_var poa_manager = poa->the_POAManager();
        poa_manager->activate();
        orbInitializer->getClientInterceptor()->setConnectionMultiplexer(multiplexer);
        orbInitializer->getServerInterceptor()->setConnectionMultiplexer(multiplexer);
        ORBSet.insert(orb);
        singleORB = orb;
      }
      return singleORB;
    }

    Connection* connect(const std::string host, const unsigned int port, CORBA::ORB* orb) 
      throw(CORBA::Exception, InvalidORB)
    {
      Mutex m(&_mutex);
      if (!singleORB) {
        if (!orb) {
        #ifdef OPENBUS_SDK_MULTITHREAD
          m.unlock();
          singleORB = openbus::multiplexed::createORB(2, multithreadArgv);
          m.lock();
        #else
          singleORB = openbus::multiplexed::createORB(1, nullArgv);
        #endif
        } else {
          if (ORBSet.find(orb) == ORBSet.end()) throw InvalidORB();
          singleORB = orb;
        }
      }
      return new Connection(host, port, singleORB, orbInitializer);
    }
    
    multiplexed::ConnectionMultiplexer* getConnectionMultiplexer(CORBA::ORB* orb) {
      CORBA::Object* o;
      try {
        o = orb->resolve_initial_references(CONNECTION_MULTIPLEXER_ID);
      } catch(CORBA::Exception& e) {
        return 0;
      }
      return  dynamic_cast<multiplexed::ConnectionMultiplexer*> (o);
    }
  }
}
