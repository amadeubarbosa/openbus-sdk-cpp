#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "11"

using namespace openbus;
using namespace auxiliar;

access_control_service::IAccessControlService* iAccessControlService;
stringstream entityName;
stringstream privateKeyFilename;

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
  } catch (SECURITY_EXCEPTION& e) {
    fail(TESTCASE, "SECURITY_EXCEPTION");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  finish(TESTCASE);
}
