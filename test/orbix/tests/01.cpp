#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "01"

using namespace openbus;
using namespace auxiliar;

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
    bus->connect(getUsername(), getPassword());
    bus->disconnect();
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (LOGIN_FAILURE& e) {
    fail(TESTCASE, "openbus::LOGIN_FAILURE");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  finish(TESTCASE);
}
