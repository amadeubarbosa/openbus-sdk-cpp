#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "04"

using namespace auxiliar;

bool isAlreadyConnected = false;
bool isInvalidORB = false;

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    openbus::Connection* c = openbus::connect("localhost", 2089);
    try {
      c = openbus::connect("localhost", 2089);
    } catch(openbus::AlreadyConnected& e) {
      isAlreadyConnected = true;
    }
    if (!isAlreadyConnected) 
      fail(TESTCASE, "Aceitou mais de uma conexao no modo nao multiplexado.");
    try {
      isAlreadyConnected = false;
      c->close();
      openbus::connect("localhost", 2089);
    } catch(openbus::AlreadyConnected& e) {
      isAlreadyConnected = true;
    }
    if (isAlreadyConnected) 
      fail(TESTCASE, "Nao permitiu uma nova conexao apos o encerramento da existente.");      
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
