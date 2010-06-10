/*
* acs/ACSTestSuite.cpp
*/

#ifndef ACS_TESTSUITE_H
#define ACS_TESTSUITE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cxxtest/TestSuite.h>
#include <openbus.h>
#include <fstream>

using namespace openbus;
using namespace tecgraf::openbus::core::v1_05;

bool leaseExpiredCallbackAfter;
bool leaseExpiredCallbackBefore;
Openbus* bus;
std::string OPENBUS_USERNAME;
std::string OPENBUS_PASSWORD;

class MyCallbackBefore : public Openbus::LeaseExpiredCallback {
  public:
    void expired() {
      TS_TRACE("Executando MyCallbackBefore()...");
      leaseExpiredCallbackBefore = true;
      bus->disconnect();
      delete bus;
    }
};

class MyCallbackAfter : public Openbus::LeaseExpiredCallback {
  public:
    void expired() {
      TS_TRACE("Executando MyCallbackAfter()...");
      leaseExpiredCallbackAfter = true;
      bus->disconnect();
      delete bus;
    }
};

class ACSTestSuite: public CxxTest::TestSuite {
  private:
    access_control_service::IAccessControlService* iAccessControlService;
    registry_service::IRegistryService* rgs;
    access_control_service::Credential* credential;
    access_control_service::Credential* credential2;
    access_control_service::Lease lease;
    access_control_service::Lease lease2;
    std::string OPENBUS_SERVER_HOST;
    unsigned short OPENBUS_SERVER_PORT;

  public:
    ACSTestSuite() {
      try {
        std::string OPENBUS_HOME = getenv("OPENBUS_HOME");
        OPENBUS_HOME += "/test/orbix/config.txt";
        std::string temp;
        std::ifstream inFile;
        inFile.open(OPENBUS_HOME.c_str());
        if (!inFile) {
          temp = "Não foi possível carregar o arquivo " + OPENBUS_HOME + ".";
          TS_FAIL(temp);
        }
        while (inFile >> temp) {
          if (temp.compare("OPENBUS_SERVER_HOST") == 0) {
            inFile >> temp; // le o '='
            inFile >> OPENBUS_SERVER_HOST; // le o valor
          }
          if (temp.compare("OPENBUS_SERVER_PORT") == 0) {
            inFile >> temp;
            inFile >> OPENBUS_SERVER_PORT;
          }
          if (temp.compare("OPENBUS_USERNAME") == 0) {
            inFile >> temp;
            inFile >> OPENBUS_USERNAME;
          }
          if (temp.compare("OPENBUS_PASSWORD") == 0) {
            inFile >> temp;
            inFile >> OPENBUS_PASSWORD;
          }
        }
        inFile.close();
        bus = Openbus::getInstance();
        bus->init(
          0, 
          NULL,
          const_cast<char*>(OPENBUS_SERVER_HOST.c_str()), 
          OPENBUS_SERVER_PORT);
        credential2 = new access_control_service::Credential;
      }
      catch (const char* errmsg) {
        TS_FAIL(errmsg);
      }
    }

    ~ACSTestSuite() {
    }

    void setUP() {
    }

    void tearDown() {
    }

    void testInitWithArgcArgv() {
      try {
        delete bus;
        bus = Openbus::getInstance();
        const char* argv[] = {
          "exec", 
          "-OpenbusHost", 
          "localhost", 
          "-OpenbusPort", 
          "2089",
          "-OpenbusDebug",
          "ALL"}; 
        bus->init(7, (char**) argv);
        bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str());
        bus->disconnect();
      } catch(CORBA::SystemException& e) {
        cout << e << endl;
        TS_FAIL("** Não foi possível se conectar ao barramento. **");
      }
    }

    void testeDisconnected() {
      TS_ASSERT(!bus->disconnect());
    }

    void testConnectByPasswordInvalidLogin() {
      TS_ASSERT_THROWS(bus->connect(OPENBUS_USERNAME.c_str(), "wrong"), 
        openbus::LOGIN_FAILURE);
    }

    void testConnectByPassword() {
      try {
        rgs = bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str());
        TS_ASSERT(rgs);
        credential = bus->getCredential();
        TS_ASSERT(credential);
      }
      catch (CORBA::COMM_FAILURE& e) {
        TS_FAIL("** Não foi possível se conectar ao barramento. **");
      }
      catch (openbus::LOGIN_FAILURE& e) {
        TS_FAIL(
          "** Não foi possível se conectar ao barramento. \
          Par usuario/senha inválido. **");
      }
      catch (const char* errmsg) {
        TS_FAIL(errmsg);
      }
    }

    void testConnectByPasswordTwice() {
      TS_ASSERT_THROWS(bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str()), 
        openbus::LOGIN_FAILURE);
    }

    void testIsConnected() {
      TS_ASSERT(bus->isConnected());
      bus->disconnect();
      TS_ASSERT(!bus->isConnected());
      try {
        rgs = bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str());
        TS_ASSERT(rgs);
        credential = bus->getCredential();
        TS_ASSERT(credential);
      }
      catch (CORBA::COMM_FAILURE& e) {
        TS_FAIL("** Não foi possível se conectar ao barramento. **");
      }
      catch (openbus::LOGIN_FAILURE& e) {
        TS_FAIL(
          "** Não foi possível se conectar ao barramento. \
          Par usuario/senha inválido. **");
      }
      catch (const char* errmsg) {
        TS_FAIL(errmsg);
      }
    }

    void testGetORB() {
      TS_ASSERT(bus->getORB());
    }

    void testRootPOA() {
      TS_ASSERT(bus->getRootPOA());
    }

    void testGetComponentBuilder() {
      TS_ASSERT(bus->getComponentBuilder());
    }

    void testGetAccessControlService() {
      try {
        iAccessControlService = bus->getAccessControlService();
        TS_ASSERT(iAccessControlService);
      }
      catch (const char* errmsg) {
        TS_FAIL(errmsg) ;
      }
    }

    void testGetRegistryService() {
      try {
        rgs = 0;
        rgs = bus->getRegistryService();
        TS_ASSERT(rgs);
      }
      catch (const char* errmsg) {
        TS_FAIL(errmsg);
      }
    }

    void testIAccessControlService() {
      access_control_service::Credential_var c;
      access_control_service::Lease l;
      iAccessControlService->loginByPassword(OPENBUS_USERNAME.c_str(), 
        OPENBUS_PASSWORD.c_str(),
        c, l);
      TS_ASSERT(iAccessControlService->logout(c));
      credential2->owner = OPENBUS_USERNAME.c_str();
      credential2->identifier = "dadadsa";
      credential2->delegate = "";
      TS_ASSERT(!iAccessControlService->logout(c));
    }

    void testConnectByCertificateNullKey() {
      bus->disconnect();
      TS_ASSERT_THROWS(bus->connect(
        "TesteBarramento", 
        0, 
        "TesteBarramento.crt"), 
        openbus::SECURITY_EXCEPTION);
    }

    void testConnectByCertificateNullACSCertificate() {
      bus->disconnect();
      TS_ASSERT_THROWS(bus->connect(
        "TesteBarramento", 
        "TesteBarramento.key", 
        0), 
        openbus::SECURITY_EXCEPTION);
    }

    void testConnectByCertificateInvalidEntityName() {
      bus->disconnect();
      TS_ASSERT_THROWS(bus->connect(
        "WRONG", 
        "TesteBarramento.key", 
        "AccessControlService.crt"), 
        openbus::SECURITY_EXCEPTION);
    }

    void testLoginByCertificate() {
      bus->disconnect();
      try {
        rgs = bus->connect(
         "TesteBarramento", 
         "TesteBarramento.key", 
         "AccessControlService.crt"); 
        TS_ASSERT(rgs);
      } catch (CORBA::SystemException& e) {
        TS_FAIL("Falha na comunicação.");
      } catch (openbus::LOGIN_FAILURE& e) {
        TS_FAIL("Par usuário/senha inválido.");
      } catch (openbus::SECURITY_EXCEPTION& e) {
        TS_FAIL("e.what()");
      } 
    }

    void testIsValid() {
      try {
        iAccessControlService = bus->getAccessControlService();
        TS_ASSERT(iAccessControlService->isValid(*bus->getCredential()));
        credential2->identifier = "123";
        credential2->owner = OPENBUS_USERNAME.c_str();
        credential2->delegate = "";
        TS_ASSERT(!iAccessControlService->isValid(*credential2));
      }
      catch (const char* errmsg) {
        TS_FAIL(errmsg);
      }
    }

    void testSetThreadCredential() {
      access_control_service::Credential* trueCredential = 
        bus->getCredential();
      access_control_service::Credential wrongCredential;
      wrongCredential.identifier = "00000000";
      wrongCredential.owner = "none";
      wrongCredential.delegate = "";
      bus->setThreadCredential(&wrongCredential);
      iAccessControlService = bus->getAccessControlService();
      try {
        bool status = iAccessControlService->isValid(wrongCredential);
        if (status) {
          TS_FAIL("Uma credencial inválida conseguiu ser validada! ");
        }
      } catch(CORBA::NO_PERMISSION& e) {
      }
      bus->setThreadCredential(trueCredential);
    }

    void testFinish() {
#if 0
      bus->disconnect();
      bus->finish(true);
      try {
        if (!CORBA::is_nil(bus->getORB())) {
          TS_FAIL("ORB não finalizado.");
        }
      } catch(CORBA::SystemException& e) {
      }

      delete bus;
#endif
    }

    void testAddLeaseExpiredCbBeforeConnect() {
      bus->disconnect();
      bus = Openbus::getInstance();
      const char* argv[] = {
        "exec", 
        "-OpenbusHost", 
        "localhost", 
        "-OpenbusPort", 
        "2089",
        "-OpenbusTimeRenewing",
        "5"};
      bus->init(7, (char**) argv);
      leaseExpiredCallbackBefore = false;
      MyCallbackBefore myCallback;
      bus->setLeaseExpiredCallback(&myCallback);
      bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str());
      bus->getAccessControlService()->logout(*bus->getCredential());
      TS_TRACE("bus->run()...");
      bus->run();
      TS_TRACE("Término do sleep...");
      if (!leaseExpiredCallbackBefore) {
        TS_FAIL("Método MyCallbackBefore::expired() não foi chamado.");
      }
    }

    void testAddLeaseExpiredCbAfterConnect() {
      bus->disconnect();
      bus = Openbus::getInstance();
      const char* argv[] = {
        "exec", 
        "-OpenbusHost", 
        "localhost", 
        "-OpenbusPort", 
        "2089",
        "-OpenbusTimeRenewing",
        "8"};
      bus->init(7, (char**) argv);
      leaseExpiredCallbackAfter = false;
      MyCallbackAfter myCallback;
      bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str());
      bus->setLeaseExpiredCallback(&myCallback);
      bus->getAccessControlService()->logout(*bus->getCredential());
      TS_TRACE("bus->run()...");
      bus->run();
      TS_TRACE("Término do sleep...");
      if (!leaseExpiredCallbackAfter) {
        TS_FAIL("Método MyCallbackAfter::expired() não foi chamado.");
      }
    }
};

#endif
