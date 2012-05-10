#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "03"

using namespace auxiliar;

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    openbus::ConnectionManager* manager = openbus::getConnectionManager(orb);
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    manager->setDefaultConnection(conn.get());
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
