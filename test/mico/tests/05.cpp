#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "05"

using namespace auxiliar;

bool isInvalidORB = false;

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    try{
      openbus::connect("localhost", 2089, (CORBA::ORB*) 1);
    } catch(openbus::InvalidORB& e) {
      isInvalidORB = true;
    }
    if (!isInvalidORB)    
      fail(TESTCASE, "Aceitou um OBR que nao foi fornecido pelo SDK.");
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
