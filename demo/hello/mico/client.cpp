/*
** OpenBus Demo - Mico
** client.cpp
*/

#include <fstream>
#include <iostream>

#include "stubs/hello.h"
#include <openbus/openbus.h>

using namespace std;
using namespace tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE;

int main(int argc, char* argv[])
{
  registry_service::IRegistryService* registryService;
  openbus::Openbus bus(argc, argv);

  cout << "Conectando no barramento..." << endl;

/* Conex�o com o barramento. */
  try
  {
    registryService = bus.connect("tester", "tester");
  }
  catch (CORBA::SystemException& e)
  {
    cout << "** Nao foi possivel se conectar ao barramento. **" << endl \
         << "* Falha na comunicacao. *" << endl;
    exit(-1);
  }
  catch (openbus::login_error& e)
  {
    cout << "** Nao foi possivel se conectar ao barramento. **" << endl \
         << "* Par usuario/senha inv�lido. *" << endl;
    exit(-1);
  }

  cout << "Conex�o com o barramento estabelecida com sucesso!" << endl;

  /* Define a lista de facetas que caracteriza o servi�o implementa.
   *  O trabalho de cria��o da lista � facilitado pelo uso da classe 
   *  FacetListHelper.
   */
  openbus::util::FacetListHelper facetListHelper;
  facetListHelper.add("IHello");

/* Busca no barramento o servi�o desejado.
*  Uma lista de *ofertas de servi�o* � retornada para o usu�rio.
*  OBS.: Neste demo somente h� uma oferta de servi�o.
*/
  if(!registryService)
  {
    std::cout << "Couldn't get registry service" << std::endl;
    return -1;
  }
  else
    std::cout << "Could! get registry service" << std::endl;

  registry_service::ServiceOfferList_var serviceOfferList =
    registryService->find(facetListHelper.getFacetList());

  std::cout << "got serviceofferlist" << std::endl;

  if (serviceOfferList->length() > 0)
  {
    std::cout << "offer list is bigger than 0" << std::endl;
    CORBA::ULong zero = 0u;
    registry_service::ServiceOffer serviceOffer = serviceOfferList[zero];

    std::cout << "first offer" << std::endl;
  
    scs::core::IComponent_var component = serviceOffer.member;

    std::cout << bus.orb_state->orb->object_to_string(component) << std::endl;

    CORBA::Object_var obj = component->getFacet("IDL:demoidl/hello/IHello:1.0");

    std::cout << "got object" << std::endl;

    demoidl::hello::IHello_var hello = demoidl::hello::IHello::_narrow(obj);

    cout << "Fazendo chamada remota sayHello()..." << endl;

    hello->sayHello();
  } else {
    cout << "Nenhuma oferta encontrada." << endl;
  }
  
  cout << "Desconectando-se do barramento..." << endl;

  bus.disconnect();

  return 0;
}

