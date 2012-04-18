#ifndef AUXILIAR_H
#define AUXILIAR_H

#include <string>

namespace auxiliar {
  void loadConfigFile();
  const char* getUsername();
  const char* getPassword();
  const char* getServerHost();
  const char* getServerPort();
  const char* getOpenbusDebug();
  void begin(const char* testcase);
  void fail(const char* testcase, const char* msg);
  void finish(const char* testcase);
}

#endif
