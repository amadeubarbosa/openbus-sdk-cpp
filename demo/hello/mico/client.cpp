/*
** OpenBus Demo - Mico
** client.cpp
*/

#include <fstream>
#include <iostream>

#include "stubs/hello.h"
#include <openbus.h>

using namespace std;
using namespace tecgraf::openbus::core::v1_05;

const char* busUser;
const char* busPassword;
const char* facetName;

void commandLineParse(int argc, char* argv[]) {
  for (short i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-BusUser")) {
      i++;
      busUser = argv[i];
    } else if (!strcmp(argv[i], "-BusPassword")) {
      i++;
      busPassword = argv[i];
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
      busUser,
      busPassword);
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

/* Define a lista de facetas que caracteriza o servi�o implementa.
*  O trabalho de cria��o da lista � facilitado pelo uso da classe 
*  FacetListHelper.
*/
  openbus::util::FacetListHelper* facetListHelper =
    new openbus::util::FacetListHelper();
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
    CORBA::Object_var obj = component->getFacet("IDL:demoidl/hello/IHello:1.0");
    demoidl::hello::IHello_var hello = demoidl::hello::IHello::_narrow(obj);

    cout << "Fazendo chamada remota sayHello()..." << endl;

    hello->sayHello();
  } else {
    cout << "Nenhuma oferta encontrada." << endl;
  }
  
  cout << "Desconectando-se do barramento..." << endl;

  bus->disconnect();
  delete bus;

  return 0;
}

