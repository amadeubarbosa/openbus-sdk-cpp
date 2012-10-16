#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "04"

using namespace openbus;
using namespace auxiliar;

tecgraf::openbus::core::v1_05::registry_service::IRegistryService* rgs;
tecgraf::openbus::core::v1_05::access_control_service::Credential* credential;

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
    rgs = bus->connect(getUsername(), getPassword());
    if (!rgs) {
      fail(TESTCASE, "Nao foi possivel obter o servico de registro.");
    }
    credential = bus->getCredential();
    if (!credential) {
      fail(TESTCASE, "Nao foi possivel obter a credencial.");
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
