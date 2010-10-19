/*
* OpenBus Demo - delegate/singlethread/mico
* client.cpp
*/

#include <fstream>
#include <iostream>

#include "stubs/delegate.h"
#include <openbus.h>

using namespace std;
using namespace tecgraf::openbus::core::v1_05;

access_control_service::Credential* myCredential;
openbus::Openbus* bus;
demoidl::demoDelegate::IHello_var hello;

class DelegateThread : public MICOMT::Thread {
  private:
    char* myName;
  public:
    DelegateThread(char* name) {
      myName = name;
    }
    
    void _run(void*) {
      for (int x=0; x<10; x++) {
        access_control_service::Credential* newCredential = new access_control_service::Credential();
        newCredential->identifier = myCredential->identifier;
        newCredential->owner = myCredential->owner;
        newCredential->delegate = myName;
      
        bus->setThreadCredential(newCredential);
      
        cout << "Fazendo chamada remota sayHello() com credencial: " << endl
             << "[identifier] " << newCredential->identifier << endl 
             << "[owner] " << newCredential->owner << endl
             << "[delegate] " << newCredential->delegate << endl << endl;

        hello->sayHello(myName);
      }
    }
};

int main(int argc, char* argv[]) {
  registry_service::IRegistryService* registryService;

  bus = openbus::Openbus::getInstance();

  bus->init(argc, argv);

  cout << "Conectando no barramento..." << endl;

/* Conexão com o barramento. */
  try {
    registryService = bus->connect(
      "DelegateService", 
      "DelegateService.key", 
      "AccessControlService.crt");
  } catch (CORBA::SystemException& e) {
    cout << "** Não foi possível se conectar ao barramento. **" << endl \
         << "* Falha na comunicação. *" << endl;
    exit(-1);
  } catch (openbus::LOGIN_FAILURE& e) {
    cout << "** Não foi possível se conectar ao barramento. **" << endl \
         << "* Par usuário/senha inválido. *" << endl;
    exit(-1);
  }

  cout << "Conexão com o barramento estabelecida com sucesso!" << endl;

/* Define a lista de facetas que caracteriza o serviço implementado.
*  O trabalho de criação da lista é facilitado pelo uso da classe 
*  FacetListHelper.
*/
  openbus::util::FacetListHelper* facetListHelper = new openbus::util::FacetListHelper();
  facetListHelper->add("IHello");

/* Busca no barramento o serviço desejado.
*  Uma lista de *ofertas de serviço* é retornada para o usuário.
*  OBS.: Neste demo somente há uma oferta de serviço.
*/
  registry_service::ServiceOfferList_var serviceOfferList = 
    registryService->find(facetListHelper->getFacetList());
  delete facetListHelper;

  if (serviceOfferList->length() > 0) {
    registry_service::ServiceOffer serviceOffer = serviceOfferList[(CORBA::ULong) 0];
  
    scs::core::IComponent_var component = serviceOffer.member;
    CORBA::Object_var obj = component->getFacet("IDL:demoidl/demoDelegate/IHello:1.0");
    hello = demoidl::demoDelegate::IHello::_narrow(obj);

    myCredential = bus->getCredential();
    
    DelegateThread* delegateThread_A = new DelegateThread("A");
    DelegateThread* delegateThread_B = new DelegateThread("B");
    delegateThread_A->start();
    delegateThread_B->start();
    delegateThread_A->wait();
    delegateThread_B->wait();
    // hello->sayHello("myName");
    
  } else {
    cout << "Nenhuma oferta encontrada." << endl;
  }
  
  cout << "Desconectando-se do barramento..." << endl;

  bus->disconnect();
  delete bus;

  return 0;
}
