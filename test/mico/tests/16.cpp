#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "16"

using namespace openbus;
using namespace auxiliar;

Openbus* bus;
tecgraf::openbus::core::v1_05::access_control_service::IAccessControlService* iAccessControlService;

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    bus = Openbus::getInstance();
    const char* argv[] = {
      "exec", 
      "-OpenbusHost", 
      getServerHost(), 
      "-OpenbusPort", 
      getServerPort(),
      "-OpenbusDebug",
      getOpenbusDebug(),
      "-OpenbusTimeRenewing",
      "2"};
    bus->init(9, (char**) argv);
    bus->connect(getUsername(), getPassword());
    bus->disconnect();
    bus->finish(0);
    if (!CORBA::is_nil(bus->getORB())) {
      fail(TESTCASE, "ORB não finalizado.");
    }
  } catch (SECURITY_EXCEPTION& e) {
    fail(TESTCASE, "SECURITY_EXCEPTION");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Excecao desconhecida");
  }
  finish(TESTCASE);
}
