/*
* acs/ACSTestSuite.cpp
*/

#ifndef ACS_TESTSUITE_H
#define ACS_TESTSUITE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cxxtest/TestSuite.h>
#include <openbus/openbus.h>
#include <fstream>
#include <unistd.h>

namespace idl = tecgraf::openbus::core:: OPENBUS_IDL_VERSION_NAMESPACE;

bool leaseExpiredCallbackAfter;
bool leaseExpiredCallbackBefore;
std::string OPENBUS_USERNAME;
std::string OPENBUS_PASSWORD;

void MyCallbackBefore()
{
      TS_TRACE("Executando MyCallbackBefore()...");
      leaseExpiredCallbackBefore = true;
      // bus->disconnect();
      // #ifndef MULTITHREAD
      //   bus->stop();
      // #endif
};

void MyCallbackAfter()
{
  TS_TRACE("Executando MyCallbackAfter()...");
  leaseExpiredCallbackAfter = true;
  // bus->disconnect();
  // #ifndef MULTITHREAD
    //   bus->stop();
  // #endif
}

class ACSTestSuite: public CxxTest::TestSuite {
  private:
    idl::access_control_service::IAccessControlService* iAccessControlService;
    idl::registry_service::IRegistryService* rgs;
    // * credential;
    // idl::access_control_service::Credential* credential2;
    idl::access_control_service::Lease lease;
    idl::access_control_service::Lease lease2;
    std::string OPENBUS_SERVER_HOST;
    std::string OPENBUS_SERVER_PORT;
    std::auto_ptr<openbus::Openbus> bus;

  public:
    ACSTestSuite() {
      TS_TRACE("ACSTestSuite::ACSTestSuite");
      try {
        std::string OPENBUS_HOME = getenv("OPENBUS_HOME");
        std::cout << "OPENBUS_HOME: " << OPENBUS_HOME << std::endl;
        OPENBUS_HOME += "/test/mico/config.txt";
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
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      try {
        const char* argv[] = {
          "exec", 
          "-OpenbusHost", 
          OPENBUS_SERVER_HOST.c_str(),
          "-OpenbusPort", 
          OPENBUS_SERVER_PORT.c_str(),
          "-OpenbusDebug",
          "ALL",
          "-OpenbusTimeRenewing",
          "2"};
        for(int i = 0; i < sizeof(argv)/sizeof(argv[0]);++i)
        {
          std::cout << "arg(" << i << "): " << argv[i] << std::endl;
        }
        bus.reset(new openbus::Openbus(9, (char**)argv));
        bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str());
        bus->disconnect();
      } catch(CORBA::Exception& e) {
        e._print(std::cout);
        throw;
      } catch (...) {
        TS_FAIL("Exceção não identificada.");
      }
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    } 

    void testeDisconnected() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      TS_ASSERT_THROWS_ANYTHING(bus->disconnect());
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testConnectByPasswordInvalidLogin() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      TS_ASSERT_THROWS(bus->connect(OPENBUS_USERNAME.c_str(), "wrong"), 
        openbus::login_error);
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testConnectByPassword() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      try {
        rgs = bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str());
        TS_ASSERT(rgs);
        boost::optional<idl::access_control_service::Credential>
          credential = bus->getCredential();
        TS_ASSERT(credential);
      }
      catch (CORBA::COMM_FAILURE& e) {
        TS_FAIL("** Não foi possível se conectar ao barramento. **");
      }
      catch (openbus::login_error& e) {
        TS_FAIL(
          "** Não foi possível se conectar ao barramento. \
          Par usuario/senha inválido. **");
      }
      catch (const char* errmsg) {
        TS_FAIL(errmsg);
      }
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testConnectByPasswordTwice() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      TS_ASSERT_THROWS(bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str()), 
        openbus::login_error);
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testIsConnected() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      TS_ASSERT(bus->isConnected());
      bus->disconnect();
      TS_ASSERT(!bus->isConnected());
      try {
        rgs = bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str());
        TS_ASSERT(rgs);
        boost::optional<idl::access_control_service::Credential>
          credential = bus->getCredential();
        TS_ASSERT(credential);
      }
      catch (CORBA::COMM_FAILURE& e) {
        TS_FAIL("** Não foi possível se conectar ao barramento. **");
      }
      catch (openbus::login_error& e) {
        TS_FAIL(
          "** Não foi possível se conectar ao barramento. \
          Par usuario/senha inválido. **");
      }
      catch (const char* errmsg) {
        TS_FAIL(errmsg);
      }
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testGetORB() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      // TS_ASSERT(bus->getORB());
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testRootPOA() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      TS_ASSERT(bus->getRootPOA());
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testGetComponentBuilder() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      //TS_ASSERT(bus->getComponentBuilder());
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testGetAccessControlService() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      // try {
      //   iAccessControlService = bus->getAccessControlService();
      //   TS_ASSERT(iAccessControlService);
      // }
      // catch (const char* errmsg) {
      //   TS_FAIL(errmsg) ;
      // }
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testGetRegistryService() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      try {
        rgs = 0;
        rgs = bus->getRegistryService();
        TS_ASSERT(rgs);
      }
      catch (const char* errmsg) {
        TS_FAIL(errmsg);
      }
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testIAccessControlService() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      // idl::access_control_service::Credential_var c;
      // idl::access_control_service::Lease l;
      // iAccessControlService->loginByPassword(OPENBUS_USERNAME.c_str(), 
      //   OPENBUS_PASSWORD.c_str(),
      //   c, l);
      // TS_ASSERT(iAccessControlService->logout(c));
      // credential2->owner = OPENBUS_USERNAME.c_str();
      // credential2->identifier = "dadadsa";
      // credential2->delegate = "";
      // TS_ASSERT(!iAccessControlService->logout(c));
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    // void testConnectByCertificateNullKey() {
    //   std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    //   bus->disconnect();
    //   TS_ASSERT_THROWS(bus->connect(
    //     "TesteBarramento", 
    //     0, 
    //     "TesteBarramento.crt"), 
    //     openbus::security_error);
    //   std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    // }

    // void testConnectByCertificateNullACSCertificate() {
    //   std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    //   bus->disconnect();
    //   TS_ASSERT_THROWS(bus->connect(
    //     "TesteBarramento", 
    //     "TesteBarramento.key", 
    //     0), 
    //     openbus::security_error);
    //   std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    // }

    // void testConnectByCertificateInvalidEntityName() {
    //   std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    //   bus->disconnect();
    //   TS_ASSERT_THROWS(bus->connect(
    //     "WRONG", 
    //     "TesteBarramento.key", 
    //     "AccessControlService.crt"), 
    //     openbus::security_error);
    //   std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    // }

    void testLoginByCertificate() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      bus->disconnect();
      try {
        rgs = bus->connect(
         "TesteBarramento", 
         "TesteBarramento.key", 
         "AccessControlService.crt"); 
        TS_ASSERT(rgs);
      } catch (CORBA::SystemException& e) {
        TS_FAIL("Falha na comunicação.");
      } catch (openbus::login_error& e) {
        TS_FAIL("Par usuário/senha inválido.");
      } catch (openbus::security_error& e) {
        TS_FAIL(e.what());
      } 
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testIsValid() {
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
      // try {
      //   iAccessControlService = bus->getAccessControlService();
      //   TS_ASSERT(iAccessControlService->isValid(*bus->getCredential()));
      //   credential2->identifier = "123";
      //   credential2->owner = OPENBUS_USERNAME.c_str();
      //   credential2->delegate = "";
      //   TS_ASSERT(!iAccessControlService->isValid(*credential2));
      // }
      // catch (const char* errmsg) {
      //   TS_FAIL(errmsg);
      // }
      std::cout << __FUNCTION__ << ":" << __LINE__ << std::endl;
    }

    void testSetThreadCredential() {
      // idl::access_control_service::Credential* trueCredential = 
      //   bus->getCredential();
      // idl::access_control_service::Credential wrongCredential;
      // wrongCredential.identifier = "00000000";
      // wrongCredential.owner = "none";
      // wrongCredential.delegate = "";
      // bus->setThreadCredential(&wrongCredential);
      // iAccessControlService = bus->getAccessControlService();
      // try {
      //   bool status = iAccessControlService->isValid(wrongCredential);
      //   if (status) {
      //     TS_FAIL("Uma credencial inválida conseguiu ser validada! ");
      //   }
      // } catch(CORBA::NO_PERMISSION& e) {
      // }
      // bus->setThreadCredential(trueCredential);
    }

    void testAddLeaseExpiredCbBeforeConnect() {
      bus->disconnect();
      const char* argv[] = {
        "exec", 
        "-OpenbusHost", 
        OPENBUS_SERVER_HOST.c_str(), 
        "-OpenbusPort", 
        OPENBUS_SERVER_PORT.c_str(),
        "-OpenbusTimeRenewing",
        "2"};
      bus.reset();
      bus.reset(new openbus::Openbus(7, (char**) argv));
      leaseExpiredCallbackBefore = false;
      bus->setLeaseExpiredCallback(&MyCallbackBefore);
      bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str());
      // bus->getAccessControlService()->logout(*bus->getCredential());
      // #ifdef MULTITHREAD
      //   TS_TRACE("sleep...");
      //   sleep(4);
      // #else
      //   TS_TRACE("Openbus::run()...");
      //   bus->run();
      // #endif
      // TS_TRACE("Término do Openbus::run()...");
      // if (!leaseExpiredCallbackBefore) {
      //   TS_FAIL("Método MyCallbackBefore::expired() não foi chamado.");
      // }
      // delete bus;
    }

    void testAddLeaseExpiredCbAfterConnect() {
      const char* argv[] = {
        "exec", 
        "-OpenbusHost", 
        OPENBUS_SERVER_HOST.c_str(), 
        "-OpenbusPort", 
        OPENBUS_SERVER_PORT.c_str(),
        "-OpenbusTimeRenewing",
        "2"};
      bus.reset();
      bus.reset(new openbus::Openbus(7, (char**) argv));
      leaseExpiredCallbackAfter = false;
      bus->connect(OPENBUS_USERNAME.c_str(), OPENBUS_PASSWORD.c_str());
      bus->setLeaseExpiredCallback(&MyCallbackAfter);
      bus->getAccessControlService()->logout(*bus->getCredential());
      #ifdef MULTITHREAD
        TS_TRACE("sleep...");
        sleep(4);
      #else
        TS_TRACE("Openbus::run()...");
        bus->run();
      #endif
      TS_TRACE("Término do Openbus::run()...");
      if (!leaseExpiredCallbackAfter) {
        TS_FAIL("Método MyCallbackAfter::expired() não foi chamado.");
      }
    }

    void testFinish() {
      // bus->disconnect();
      // bus->finish(0);
      // try {
      //   if (!CORBA::is_nil(bus->getORB())) {
      //     TS_FAIL("ORB não finalizado.");
      //   }
      // } catch(CORBA::SystemException& e) {
      // }

      // delete bus;
    }
};

#endif
