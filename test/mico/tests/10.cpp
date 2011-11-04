#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "10"

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
      getServerHost(),
      "-OpenbusPort", 
      getServerPort(),
      "-OpenbusDebug",
      getOpenbusDebug(),
      "-OpenbusTimeRenewing",
      "2"};
    bus->init(9, (char**) _args);
    bus->connect(
      "WRONG", 
      "TesteBarramento.key", 
      "AccessControlService.crt");
  } catch (SECURITY_EXCEPTION& e) {
    /* pass */
    finish(TESTCASE);
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  finish(TESTCASE);
}
