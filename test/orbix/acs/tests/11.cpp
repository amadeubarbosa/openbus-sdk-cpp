#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "11"

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
  } catch (SECURITY_EXCEPTION& e) {
    fail(TESTCASE, "SECURITY_EXCEPTION");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exce��o desconhecida");
  }
  finish(TESTCASE);
}