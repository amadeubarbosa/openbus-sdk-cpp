#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "04"

using namespace auxiliar;

bool isAlreadyConnected = false;

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    openbus::Connection* conn1 (openbus::connect("localhost", 2089));
    try {
      openbus::Connection* conn2 (openbus::connect("localhost", 2089));
    } catch(openbus::AlreadyConnected& e) {
      isAlreadyConnected = true;
    }
    if (!isAlreadyConnected) 
      fail(TESTCASE, "Aceitou mais de uma conexao no modo nao multiplexado.");
  } catch (const CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
    return -1;
  } catch (const openbus::Connection::Exception& e) {
    fail(TESTCASE, "Connection::Exception");
    return -1;
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
    return -1;
  }
  finish(TESTCASE);
}
