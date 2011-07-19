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

const char* entityName;
const char* privateKeyFilename;
const char* ACSCertificateFilename;
const char* facetName;

void commandLineParse(int argc, char* argv[]) {
  for (short i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-EntityName")) {
      i++;
      entityName = argv[i];
    } else if (!strcmp(argv[i], "-PrivateKeyFilename")) {
      i++;
      privateKeyFilename = argv[i];
    } else if (!strcmp(argv[i], "-ACSCertificateFilename")) {
      i++;
      ACSCertificateFilename = argv[i];
    } else if (!strcmp(argv[i], "-FacetName")) {
      i++;
      facetName = argv[i];
    } 
  }
}

int main(int argc, char* argv[]) {
  openbus::Openbus* bus;
  registry_service::IRegistryService* registryService;

  bus = openbus::Openbus::getInstance();

  bus->init(argc, argv);

  commandLineParse(argc, argv);

  cout << "Conectando no barramento..." << endl;

/* Conex�o com o barramento. */
  try {
    registryService = bus->connect(
      entityName, 
      privateKeyFilename,
      ACSCertificateFilename);
  } catch (CORBA::SystemException& e) {
    cout << "** N�o foi poss�vel se conectar ao barramento. **" << endl \
         << "* Falha na comunica��o. *" << endl;
    exit(-1);
  } catch (openbus::LOGIN_FAILURE& e) {
    cout << "** N�o foi poss�vel se conectar ao barramento. **" << endl \
         << "* Falha no processo de autentica��o. *" << endl;
    exit(-1);
  }

  cout << "Conex�o com o barramento estabelecida com sucesso!" << endl;

/* Define a lista de facetas que caracteriza o servi�o implementado.
*  O trabalho de cria��o da lista � facilitado pelo uso da classe 
*  FacetListHelper.
*/
  openbus::util::FacetListHelper* facetListHelper = new openbus::util::FacetListHelper();
  facetListHelper->add(facetName);

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
