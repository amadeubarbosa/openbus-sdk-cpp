#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "02"

using namespace openbus;
using namespace auxiliar;

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    Openbus* bus = Openbus::getInstance();
    if (bus->disconnect()) {
      fail(TESTCASE, "Nao deveria haver conexao.");
    }
  } catch (CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
  }
  finish(TESTCASE);
}
