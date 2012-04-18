#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "03"

using namespace auxiliar;

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    openbus::Connection* conn (openbus::connect("localhost", 2089));
    conn->loginByPassword("admin", "admin");
    openbus::idl_or::ServicePropertySeq props;
    props.length(0);
    openbus::idl_or::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
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
