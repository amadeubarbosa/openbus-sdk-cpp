#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "12"

using namespace openbus;
using namespace auxiliar;

tecgraf::openbus::core::v1_05::access_control_service::IAccessControlService* iAccessControlService;
std::stringstream entityName;
std::stringstream privateKeyFilename;

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
    entityName << "TesteBarramento" << getenv("TEC_UNAME");
    privateKeyFilename << "TesteBarramento" << getenv("TEC_UNAME") << ".key";
    bus->connect(
      entityName.str().c_str(), 
      privateKeyFilename.str().c_str(), 
      "AccessControlService.crt"); 
    iAccessControlService = bus->getAccessControlService();
    if (!iAccessControlService->isValid(*bus->getCredential())) {
      fail(TESTCASE, "Credencial deveria ser valida.");
    }
    tecgraf::openbus::core::v1_05::access_control_service::Credential* credential2 =
      new tecgraf::openbus::core::v1_05::access_control_service::Credential;
    credential2->identifier = "123";
    credential2->owner = "TesteBarramento";
    credential2->delegate = "";
    if (iAccessControlService->isValid(*credential2)) {
      fail(TESTCASE, "Credencial NAO deveria ser valida.");      
    }
    bus->disconnect();
  } catch (SECURITY_EXCEPTION& e) {
    fail(TESTCASE, "SECURITY_EXCEPTION");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  finish(TESTCASE);
}
