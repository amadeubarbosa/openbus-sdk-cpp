/*
* OpenBus Demo - delegate/singlethread/orbix
* client.cpp
*/

#include <fstream>
#include <iostream>

#include "stubs/delegate.hh"
#include <openbus.h>

using namespace std;

int main(int argc, char* argv[]) {
  openbus::Openbus* bus;
  registry_service::IRegistryService* registryService;

  bus = openbus::Openbus::getInstance();

  bus->init(argc, argv);

  cout << "Conectando no barramento..." << endl;

/* Conex�o com o barramento. */
  try {
    registryService = bus->connect(
      "DelegateService", 
      "DelegateService.key", 
      "AccessControlService.crt");
  } catch (CORBA::SystemException& e) {
    cout << "** Nao foi possivel se conectar ao barramento. **" << endl \
         << "* Falha na comunicacao. *" << endl;
    exit(-1);
  } catch (openbus::LOGIN_FAILURE& e) {
    cout << "** Nao foi possivel se conectar ao barramento. **" << endl \
         << "* Par usuario/senha inv�lido. *" << endl;
    exit(-1);
  }

  cout << "Conex�o com o barramento estabelecida com sucesso!" << endl;

/* Define a lista de facetas que caracteriza o servi�o implementado.
*  O trabalho de cria��o da lista � facilitado pelo uso da classe 
*  FacetListHelper.
*/
  openbus::util::FacetListHelper* facetListHelper = new openbus::util::FacetListHelper();
  facetListHelper->add("IHello");

/* Busca no barramento o servi�o desejado.
*  Uma lista de *ofertas de servi�o* � retornada para o usu�rio.
*  OBS.: Neste demo somente h� uma oferta de servi�o.
*/
  registry_service::ServiceOfferList_var serviceOfferList =
    registryService->find(facetListHelper->getFacetList());
  delete facetListHelper;

  if (serviceOfferList->length() > 0) {
    registry_service::ServiceOffer serviceOffer = serviceOfferList[(CORBA::ULong) 0];

    scs::core::IComponent_var component = serviceOffer.member;
    CORBA::Object_var obj = component->getFacet("IDL:demoidl/demoDelegate/IHello:1.0");
    demoidl::demoDelegate::IHello_var hello = demoidl::demoDelegate::IHello::_narrow(obj);

    access_control_service::Credential* myCredential = bus->getCredential();
    
    access_control_service::Credential_var newCredential = new access_control_service::Credential();
    newCredential->identifier = myCredential->identifier;
    newCredential->owner = myCredential->owner;
    newCredential->delegate = "fulano";
    
    bus->setThreadCredential(newCredential);
    
    cout << "Fazendo chamada remota sayHello() com credencial: " << endl
         << "[identifier] " << newCredential->identifier << endl 
         << "[owner] " << newCredential->owner << endl
         << "[delegate] " << newCredential->delegate << endl << endl;
    
    hello->sayHello("Ol�!");
    
    bus->setThreadCredential(myCredential);
    
    cout << "Fazendo chamada remota sayHello() com credencial: " << endl
         << "[identifier] " << myCredential->identifier << endl 
         << "[owner] " << myCredential->owner << endl
         << "[delegate] " << myCredential->delegate << endl << endl;

    hello->sayHello("Voltei!");

  } else {
    cout << "Nenhuma oferta encontrada." << endl;
  }

  cout << "Desconectando-se do barramento..." << endl;

  bus->disconnect();
  delete bus;

  return 0;
}