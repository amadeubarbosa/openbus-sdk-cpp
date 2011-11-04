#include <openbus.h>
#include <iostream>
#if HAVE_UNISTD_H
  #include <unistd.h>
#endif
#include "../util/auxiliar.h"

#define TESTCASE "15"

using namespace openbus;
using namespace auxiliar;

Openbus* bus;
access_control_service::IAccessControlService* iAccessControlService;
bool leaseExpiredCallbackAfter;

class MyCallbackAfter : public Openbus::LeaseExpiredCallback {
  public:
    void expired() {
      std::cout << "Executando MyCallbackAfter()..." << std::endl;
      leaseExpiredCallbackAfter = true;
      bus->disconnect();
      #ifndef MULTITHREAD
        bus->stop();
      #endif
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
    leaseExpiredCallbackAfter = false;
    MyCallbackAfter myCallback;
    bus->connect(getUsername(), getPassword());
    bus->setLeaseExpiredCallback(&myCallback);
    bus->getAccessControlService()->logout(*bus->getCredential());
    #ifdef MULTITHREAD
      std::cout << "(sleep)" << std::endl;
      sleep(4);
    #else
      std::cout << "Openbus::run()..." << std::endl;
      bus->run();
    #endif
    std::cout << "Termino do Openbus::run()..." << std::endl;
    if (!leaseExpiredCallbackAfter) {
      fail(TESTCASE, "Metodo MyCallbackAfter::expired() nao foi chamado.");
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
