#include <openbus.h>
#include <scs/ComponentContext.h>
#include <iostream>
#include "../util/auxiliar.h"
#include "../stubs/RGSTest.h"

#define TESTCASE "22"

using namespace openbus;
using namespace auxiliar;
using namespace tecgraf::openbus::core::v1_05;
using namespace tecgraf::openbus::core::v1_05::registry_service;

registry_service::IRegistryService* rgs;
access_control_service::Credential* credential;
char* registryIdentifier;
char* registryIdentifier2;
openbus::util::PropertyListHelper* propertyListHelper;
openbus::util::PropertyListHelper* propertyListHelper2;

class RGSTest : virtual public POA_IRGSTest {
    void foo() 
      throw(CORBA::SystemException) 
    {
      std::cout << "Calling foo..." << std::endl;
    };
};

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    Openbus* bus = Openbus::getInstance();
    const char* _args[] = {
      "exec", 
      "-OpenbusHost", 
      getServerHost(),
      "-OpenbusPort", 
      getServerPort(),
      "-OpenbusDebug",
      getOpenbusDebug(),
      "-OpenbusTimeRenewing",
      "2"};
    bus->init(9, (char**) _args);
    rgs = bus->connect(
     "TesteBarramento", 
     "TesteBarramento.key", 
     "AccessControlService.crt"); 
    if (!rgs) {
      fail(TESTCASE, "Nao foi possivel obter o servico de registro.");
    }
    
    if (rgs->unregister((char*) "ID")) {
      fail(TESTCASE, "Remocao de uma oferta que nao existe.");
    }
    
    bus->disconnect();
  } catch (LOGIN_FAILURE& e) {
    fail(TESTCASE, "LOGIN_FAILURE");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  finish(TESTCASE);
}
