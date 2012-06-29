#include <openbus.h>
#include <iostream>
#include "../openbus/util/auxiliar.h"

#define TESTCASE "01"

using namespace openbus;
using namespace auxiliar;

bool isInvalid = false;
bool receiveNoPermission = false;

bool InvalidLoginCallback(const openbus::Connection* conn, const openbus::idl_ac::LoginInfo* login) 
{
  std::cout << "login [" << login->id << "] terminated shutting the server down." << std::endl;
  isInvalid = true;
  // conn->close();
  return false;
}

int main(int argc, char* argv[]) {
  loadConfigFile();
  begin(TESTCASE);
  try {
    CORBA::ORB* orb = openbus::initORB(argc, argv);
    openbus::ConnectionManager* manager = openbus::getConnectionManager(orb);
    std::auto_ptr <openbus::Connection> conn (manager->createConnection("localhost", 2089));
    manager->setDefaultConnection(conn.get());
    conn->onInvalidLoginCallback(&InvalidLoginCallback);
    conn->loginByPassword("admin", "admin");
    conn->login_registry()->invalidateLogin(conn->login()->id);
    openbus::idl_or::ServicePropertySeq props;
    props.length(0);
    try {
      openbus::idl_or::ServiceOfferDescSeq_var offers = conn->offers()->findServices(props);
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
    return -1;
  } catch (...) {
    fail(TESTCASE, "Exceção desconhecida");
    return -1;
  }
  finish(TESTCASE);
}
