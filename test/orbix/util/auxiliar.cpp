#include "auxiliar.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>

namespace auxiliar {
  std::string OPENBUS_USERNAME;
  std::string OPENBUS_PASSWORD;
  std::string OPENBUS_SERVER_HOST;
  std::string OPENBUS_SERVER_PORT;
  std::string OPENBUS_DEBUG;
  
  bool failFlag = false;
  
  void loadConfigFile() {
    std::string OPENBUS_HOME = getenv("OPENBUS_HOME");
    OPENBUS_HOME += "/test/orbix/config.txt";
    std::string temp;
    std::ifstream inFile;
    inFile.open(OPENBUS_HOME.c_str());
    if (!inFile) {
      std::cout << "Nao foi possivel carregar o arquivo de configuracao." 
        << std::endl;
      exit(-1);
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
      if (temp.compare("OPENBUS_DEBUG") == 0) {
        inFile >> temp;
        inFile >> OPENBUS_DEBUG;
      }
    }
    inFile.close();
  }

  const char* getUsername() {
    char* username = new char[OPENBUS_USERNAME.size() + 1];
    strcpy(username, OPENBUS_USERNAME.c_str());
    return username;
  }

  const char* getPassword() {
    char* password = new char[OPENBUS_PASSWORD.size() + 1];
    strcpy(password, OPENBUS_PASSWORD.c_str());
    return password;
  }

  const char* getServerHost() {
    char* serverHost = new char[OPENBUS_SERVER_HOST.size() + 1];
    strcpy(serverHost, OPENBUS_SERVER_HOST.c_str());
    return serverHost;
  }

  const char* getServerPort() {
    char* serverPort = new char[OPENBUS_SERVER_PORT.size() + 1];
    strcpy(serverPort, OPENBUS_SERVER_PORT.c_str());
    return serverPort;
  }
  
  const char* getOpenbusDebug() {
    if (!strcmp(OPENBUS_DEBUG.c_str(), "Yes")) {
      return "ALL";
    } else {
      return "OFF";
    }
  }
  
  void begin(const char* testcase) {
    std::cout << "..." << "Running test '" << testcase <<  "'" << std::endl;
  }

  void fail(const char* testcase, const char* msg) {
    failFlag = true;
    std::cout << "[ERROR]" << std::endl <<
      "......" << msg << std::endl;
  }
  
  void finish(const char* testcase) {
    if (!failFlag) {
      exit(0);    
    } else {
      exit(-1);    
    }
  }
}
