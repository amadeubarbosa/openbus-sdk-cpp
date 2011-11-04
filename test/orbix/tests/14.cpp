#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "14"

using namespace openbus;
using namespace auxiliar;

Openbus* bus;
access_control_service::IAccessControlService* iAccessControlService;
bool leaseExpiredCallbackBefore;

class MyCallbackBefore : public Openbus::LeaseExpiredCallback {
  public:
    void expired() {
      std::cout << "Executando MyCallbackBefore()..." << std::endl;
      leaseExpiredCallbackBefore = true;
    }
};

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
    leaseExpiredCallbackBefore = false;
    MyCallbackBefore myCallback;
    bus->setLeaseExpiredCallback(&myCallback);
    bus->connect(getUsername(), getPassword());
    bus->getAccessControlService()->logout(*bus->getCredential());
    std::cout << "(sleep)" << std::endl;
    IT_CurrentThread::sleep(4000);
    std::cout << "Termino do Openbus::run()..." << std::endl;
    if (!leaseExpiredCallbackBefore) {
      fail(TESTCASE, "M�todo MyCallbackBefore::expired() nao foi chamado.");
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
