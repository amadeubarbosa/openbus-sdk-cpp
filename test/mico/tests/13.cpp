#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "13"

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
    tecgraf::openbus::core::v1_05::access_control_service::Credential* trueCredential = 
      bus->getCredential();
    tecgraf::openbus::core::v1_05::access_control_service::Credential wrongCredential;
    wrongCredential.identifier = "00000000";
    wrongCredential.owner = "none";
    wrongCredential.delegate = "";
    bus->setThreadCredential(&wrongCredential);
    iAccessControlService = bus->getAccessControlService();
    try {
      bool status = iAccessControlService->isValid(wrongCredential);
      if (status) {
        fail(TESTCASE, "Uma credencial invalida conseguiu ser validada.");
        finish(TESTCASE);
      }
    } catch(CORBA::NO_PERMISSION& e) {
      fail(TESTCASE, "CORBA::NO_PERMISSION");
      finish(TESTCASE);
    }
    bus->setThreadCredential(trueCredential);
    delete bus;
  } catch (SECURITY_EXCEPTION& e) {
    fail(TESTCASE, "SECURITY_EXCEPTION");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  finish(TESTCASE);
}
