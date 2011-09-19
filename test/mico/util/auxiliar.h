#ifndef AUXILIAR_H
#define AUXILIAR_H

#include <string>

namespace auxiliar {
  void loadConfigFile();
  std::string getUsername();
  std::string getPassword();
  std::string getServerHost();
  std::string getServerPort();
  const char* getOpenbusDebug();
  void begin(const char* testcase);
  void fail(const char* testcase, const char* msg);
  void finish(const char* testcase);
}

#endif
