#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "09"

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
    bus->connect(getUsername().c_str(), getPassword().c_str());
    iAccessControlService = bus->getAccessControlService();
    access_control_service::Credential_var c;
    access_control_service::Lease l;
    iAccessControlService->loginByPassword(getUsername().c_str(), 
      getPassword().c_str(),
      c, l);
    if (!iAccessControlService->logout(c)) {
      fail(TESTCASE, "Falha no logout.");      
    }
    if (iAccessControlService->logout(c)) {
      fail(TESTCASE, "Logout repetido.");            
    }
  } catch (LOGIN_FAILURE& e) {
    fail(TESTCASE, "LOGIN_FAILURE");
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  finish(TESTCASE);
}
