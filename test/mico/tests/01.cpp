#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "01"

using namespace openbus;
using namespace auxiliar;

bool isInvalid = false;
bool receiveNoPermission = false;

void InvalidLoginCallback(openbus::Connection* conn, char* login) {
  std::cout << "login [" << login << "] terminated shutting the server down." << std::endl;
  isInvalid = true;
  // conn->close();
}

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    openbus::Connection* conn (openbus::connect("localhost", 2089));
    conn->onInvalidLoginCallback(&InvalidLoginCallback);
    conn->loginByPassword("admin", "admin");
    conn->login_registry()->invalidateLogin(conn->login()->id);
    openbus::idl_offerregistry::ServicePropertySeq props;
    props.length(0);
    try {
      openbus::idl_offerregistry::ServiceOfferDescSeq_var offers =
        conn->offers()->findServices(props);
    } catch (CORBA::NO_PERMISSION& e) {
      receiveNoPermission = true;
    }
    if (!isInvalid) 
      fail(TESTCASE, "InvalidLoginCallback não foi chamado.");
    if (!receiveNoPermission) 
      fail(TESTCASE, "Exceção CORBA::NO_PERMISSION deveria ser lançada.");
  } catch (const CORBA::Exception& e) {
    fail(TESTCASE, "CORBA::Exception");
    return -1;
  } catch (const openbus::Connection::Exception& e) {
    fail(TESTCASE, "Connection::Exception");
    // std::cout << "[error (Connection::Exception)] " << e.name() << std::endl;
    return -1;
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
    return -1;
  }
  finish(TESTCASE);
}
