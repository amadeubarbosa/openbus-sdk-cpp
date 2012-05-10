#include <openbus.h>
#include <iostream>
#include "../util/auxiliar.h"

#define TESTCASE "02"

using namespace auxiliar;

bool alreadyLogged = false;
bool accessDenied = false;

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    openbus::ConnectionManager* manager = openbus::getConnectionManager(orb);
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    manager->setDefaultConnection(conn.get());
    conn->loginByPassword("admin", "admin");
    conn->logout();
    conn->loginByPassword("admin", "admin");
    try {
      conn->loginByPassword("admin", "admin");
    } catch (const openbus::Connection::AlreadyLogged& e) {
      alreadyLogged = true;
    }
    if (!alreadyLogged) fail(TESTCASE, "Ja estou logado.");
    try {
      conn->loginByPassword("admin", "wrong");
    } catch (const openbus::Connection::AlreadyLogged& e) {
      accessDenied = true;
    }
    if (!accessDenied) fail(TESTCASE, "Nao deveria conseguir acesso.");
    conn->close();
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
