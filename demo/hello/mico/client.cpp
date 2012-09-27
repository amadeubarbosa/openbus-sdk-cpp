/*
** OpenBus Demo - Mico
** client.cpp
*/

#include <fstream>
#include <iostream>

#include "stubs/hello.h"
#include <openbus.h>

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
  tecgraf::openbus::core::v1_05::registry_service::IRegistryService* registryService;

  bus = openbus::Openbus::getInstance();

  bus->init(argc, argv);
  
  commandLineParse(argc, argv);

  std::cout << "Conectando no barramento..." << std::endl;

/* Conex�o com o barramento. */
  try {
    registryService = bus->connect(
      busUser,
      busPassword);
  } catch (CORBA::SystemException& e) {
    std::cout << "** Nao foi possivel se conectar ao barramento. **" << std::endl \
         << "* Falha na comunicacao. *" << std::endl;
    exit(-1);
  } catch (openbus::LOGIN_FAILURE& e) {
    std::cout << "** Nao foi possivel se conectar ao barramento. **" << std::endl \
         << "* Par usuario/senha inv�lido. *" << std::endl;
    exit(-1);
  }

  std::cout << "Conex�o com o barramento estabelecida com sucesso!" << std::endl;

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
  tecgraf::openbus::core::v1_05::registry_service::ServiceOfferList_var serviceOfferList =
    registryService->find(facetListHelper->getFacetList());
  delete facetListHelper;

  if (serviceOfferList->length() > 0) {
    tecgraf::openbus::core::v1_05::registry_service::ServiceOffer serviceOffer = serviceOfferList[(CORBA::ULong) 0];
  
    scs::core::IComponent_var component = serviceOffer.member;
    CORBA::Object_var obj = component->getFacet("IDL:demoidl/hello/IHello:1.0");
    demoidl::hello::IHello_var hello = demoidl::hello::IHello::_narrow(obj);

    std::cout << "Fazendo chamada remota sayHello()..." << std::endl;

    hello->sayHello();
  } else {
    std::cout << "Nenhuma oferta encontrada." << std::endl;
  }
  
  std::cout << "Desconectando-se do barramento..." << std::endl;

  bus->disconnect();
  delete bus;

  return 0;
}

