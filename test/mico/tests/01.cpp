#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "01"

using namespace openbus;
using namespace auxiliar;

bool isInvalid = false;

void onInvalidLogin(openbus::Connection* conn, char* login) {
  std::cout << "login [" << login << "] terminated shutting the server down." << std::endl;
  isInvalid = true;
//  conn->close();
}

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    openbus::Connection* conn (openbus::connect("localhost", 2089));
    conn->onInvalidLogin(&onInvalidLogin);
    conn->loginByPassword("demo", "demo");
    // conn->login_registry()->invalidateLogin(conn->loginInfo()->id);
    sleep(60);
    openbus::idl_offerregistry::ServicePropertySeq props;
    props.length(0);
    openbus::idl_offerregistry::ServiceOfferDescSeq_var offers =
      conn->offer_registry()->findServices(props);
    if (!isInvalid) 
      fail(TESTCASE, "onInvalidLogin não foi chamado.");
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
