#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "05"

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
      getServerHost().c_str(),
      "-OpenbusPort", 
      getServerPort().c_str(),
      "-OpenbusDebug",
      getOpenbusDebug(),
      "-OpenbusTimeRenewing",
      "2"};
    bus->init(9, (char**) _args);
    bus->connect(getUsername().c_str(), getPassword().c_str());
    try {
      bus->connect(getUsername().c_str(), getPassword().c_str());
    } catch (LOGIN_FAILURE& e) {
      /* pass */
      finish(TESTCASE);
    }
    bus->disconnect();
  } catch (LOGIN_FAILURE& e) {
    fail(TESTCASE, "LOGIN_FAILURE");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  fail(TESTCASE, "Ja estava logado.");
  finish(TESTCASE);
}
