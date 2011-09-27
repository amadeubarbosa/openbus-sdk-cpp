#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "12"

using namespace openbus;
using namespace auxiliar;

access_control_service::IAccessControlService* iAccessControlService;

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    Openbus* bus = Openbus::getInstance();
    const char* _args[] = {
      "exec", 
      "-OpenbusHost", 
      getServerHost().c_str(),
      "-OpenbusPort", 
      getServerPort().c_str(),
      "-OpenbusDebug",
      getOpenbusDebug(),
      "-OpenbusTimeRenewing",
      "2"};
    bus->init(9, (char**) _args);
    bus->connect(
      "TesteBarramento", 
      "TesteBarramento.key", 
      "AccessControlService.crt"); 
    iAccessControlService = bus->getAccessControlService();
    if (!iAccessControlService->isValid(*bus->getCredential())) {
      fail(TESTCASE, "Credencial deveria ser valida.");
    }
    access_control_service::Credential* credential2 =
      new access_control_service::Credential;
    credential2->identifier = "123";
    credential2->owner = "TesteBarramento";
    credential2->delegate = "";
    if (iAccessControlService->isValid(*credential2)) {
      fail(TESTCASE, "Credencial NAO deveria ser valida.");      
    }
  } catch (SECURITY_EXCEPTION& e) {
    fail(TESTCASE, "SECURITY_EXCEPTION");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  finish(TESTCASE);
}
