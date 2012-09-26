/*
** FaultToleranceManager.cpp
*/

#include "FaultToleranceManager.h"
#include "openbus.h"

#include <lua.hpp>

namespace openbus {
  FaultToleranceManager* FaultToleranceManager::faultToleranceManager = 0;

  FaultToleranceManager::FaultToleranceManager() {
    Openbus::logger->log(logger::INFO, 
      "FaultToleranceManager::FaultToleranceManager() BEGIN");
    Openbus::logger->indent();
    trials = 1;
    currTrial = 1;
    Openbus::logger->dedent(logger::INFO,
      "FaultToleranceManager::FaultToleranceManager() END");
  }

  FaultToleranceManager::~FaultToleranceManager() {
  }

  void FaultToleranceManager::loadConfig(char* filename) {
    Openbus::logger->log(logger::INFO, 
      "FaultToleranceManager::loadConfig() BEGIN");
    Openbus::logger->indent();
    if (luaL_loadfile(Openbus::luaState, filename)) {
      const char* errmsg = lua_tostring(Openbus::luaState, -1);
      lua_pop(Openbus::luaState, 1);
      Openbus::logger->log(logger::ERROR, errmsg);
    } else {
      if (lua_pcall(Openbus::luaState, 0, 0, 0)) {
        const char* errmsg = lua_tostring(Openbus::luaState, -1);
        lua_pop(Openbus::luaState, 1);
        Openbus::logger->log(logger::ERROR, errmsg);
      } else {
        Openbus::logger->log(logger::INFO, 
          "Arquivo de configuracao carregado com sucesso.");
        lua_getglobal(Openbus::luaState, "ACSHosts");
        if (lua_istable(Openbus::luaState, -1)) {
          size_t numACSHosts = lua_objlen(Openbus::luaState, -1);
          stringstream out;
          out << "Numero de replicas: " << numACSHosts;
          Openbus::logger->log(logger::INFO, out.str());
          lua_pushnil(Openbus::luaState);
          while (lua_next(Openbus::luaState, -2)) {
            stringstream out;
            size_t strLen;
            char* name;
            unsigned short port;
            Host* host;
            out << "Replica[" << lua_tonumber(Openbus::luaState, -2) << "]: ";
          /* Obtenho a maquina. */
            lua_pushnumber(Openbus::luaState, 1);
            lua_gettable(Openbus::luaState, -2);
            strLen = lua_objlen(Openbus::luaState, -1); 
            const char* luaString = 
              lua_tolstring(Openbus::luaState, -1, &strLen);
            name = new char[strLen + 1];
            memcpy(name, luaString , strLen);
            name[strLen] = '\0';
            out << name;
            lua_pop(Openbus::luaState, 1);
          /* Obtenho a porta. */
            lua_pushnumber(Openbus::luaState, 2);
            lua_gettable(Openbus::luaState, -2);
            port = (unsigned short) lua_tointeger(Openbus::luaState, -1);
            out << "::" << port;
            lua_pop(Openbus::luaState, 1);
            Openbus::logger->log(logger::INFO, out.str());
          /* Armazenando par maquina/porta na lista de replicas.*/
            host = new Host;
            host->name = name;
            host->port = port;
            acsHosts.push_back(host);
          /* Removo a tabela que discrima a maquina e a porta. */
            lua_pop(Openbus::luaState, 1);
          }
          itACSHosts = acsHosts.begin();
        } else {
          Openbus::logger->log(logger::ERROR, "Aus�ncia da tabela ACSHosts.");
        }    
      }
    }
    Openbus::logger->dedent(logger::INFO, 
      "FaultToleranceManager::loadConfig() END");
  }

  FaultToleranceManager* FaultToleranceManager::getInstance() {
    if (!faultToleranceManager) {
      faultToleranceManager = new FaultToleranceManager();
    }
    return faultToleranceManager;
  }

  Host* FaultToleranceManager::updateACSHostInUse() {
    Openbus::logger->log(logger::INFO, 
      "FaultToleranceManager::updateACSHostInUse() BEGIN");
    Openbus::logger->indent();
    Openbus* bus = Openbus::getInstance();
    stringstream out;
    out << "Replica ACS em uso: [" << bus->hostBus << "::" << bus->portBus << "]";
    Openbus::logger->log(logger::INFO, out.str());
    out.str(" ");
    if (acsHosts.size() > 0) {  
    /*
    * Se todo o conjunto foi percorrido sem se obter uma replica valida, 
    * verifica-se o numero maximo de tentativas disponiveis. Quando este
    * numero � alcan�ado, a falha n�o ser�  recuperada,  caso contr�rio,
    * uma nova tentativa � realizada em se percorrer todo o conjunto  em
    * busca de uma replica valida. 
    */
      itACSHosts++;
      if (itACSHosts == acsHosts.end()) {
        Openbus::logger->log(logger::INFO, "Conjunto de replicas percorrido.");
        if (currTrial > trials) {
          out << "Numero de tentativas esgotado." << endl;
          Openbus::logger->log(logger::WARNING, out.str());
          out.str(" ");
          return 0;
        } else {
          out << "N�mero de tentativas realizadas: " << currTrial << endl;
          currTrial++;
          Openbus::logger->log(logger::INFO, out.str());
          out.str(" ");
          itACSHosts = acsHosts.begin();
        }
      } 
      acsHostInUse.name = (*itACSHosts)->name; 
      acsHostInUse.port = (*itACSHosts)->port; 
      out << "Nova replica ACS: [" << acsHostInUse.name  << "::" << acsHostInUse.port << "]";
      Openbus::logger->log(logger::INFO, out.str());
    } else {
      Openbus::logger->log(logger::WARNING, "Lista de replicas vazia.");
      return 0;
    }
    Openbus::logger->dedent(logger::INFO, 
      "FaultToleranceManager::updateACSHostInUse() END");
    return &acsHostInUse;
  }
}
