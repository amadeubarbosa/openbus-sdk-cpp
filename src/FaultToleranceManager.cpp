/*
** FaultToleranceManager.cpp
*/

#include "openbus/fault_tolerance_manager.h"
#include "openbus/openbus.h"

#include <lua.hpp>

#include <sstream>

namespace openbus {
  
fault_tolerance_manager::fault_tolerance_manager(openbus::orb_state& orb_state)
  : orb_state(&orb_state)
{
  //Openbus::logger->log(INFO, 
  //"fault_tolerance_manager::fault_tolerance_manager() BEGIN");
  //Openbus::logger->indent();
  trials = 1;
  currTrial = 1;
  //Openbus::logger->dedent(INFO,
  //"fault_tolerance_manager::fault_tolerance_manager() END");
}

fault_tolerance_manager::~fault_tolerance_manager()
{
}

void fault_tolerance_manager::loadConfig(char* filename)
{
  //Openbus::logger->log(INFO, 
  //"fault_tolerance_manager::loadConfig() BEGIN");
  //Openbus::logger->indent();
  if (luaL_loadfile(orb_state->luaState, filename))
  {
    const char* errmsg = lua_tostring(orb_state->luaState, -1);
    lua_pop(orb_state->luaState, 1);
    //Openbus::logger->log(ERROR, errmsg);
  }
  else
  {
    if (lua_pcall(orb_state->luaState, 0, 0, 0))
    {
      const char* errmsg = lua_tostring(orb_state->luaState, -1);
      lua_pop(orb_state->luaState, 1);
      //Openbus::logger->log(ERROR, errmsg);
    }
    else
    {
      //Openbus::logger->log(INFO, 
      //"Arquivo de configuracao carregado com sucesso.");
      lua_getglobal(orb_state->luaState, "ACSHosts");
      if (lua_istable(orb_state->luaState, -1))
      {
        size_t numACSHosts = lua_objlen(orb_state->luaState, -1);
        std::stringstream out;
        out << "Numero de replicas: " << numACSHosts;
        //Openbus::logger->log(INFO, out.str());
        lua_pushnil(orb_state->luaState);
        while (lua_next(orb_state->luaState, -2))
        {
          std::stringstream out;
          size_t strLen;
          char* name;
          unsigned short port;
          Host* host;
          out << "Replica[" << lua_tonumber(orb_state->luaState, -2) << "]: ";
          /* Obtenho a maquina. */
          lua_pushnumber(orb_state->luaState, 1);
          lua_gettable(orb_state->luaState, -2);
          strLen = lua_objlen(orb_state->luaState, -1); 
          const char* luaString = 
            lua_tolstring(orb_state->luaState, -1, &strLen);
          name = new char[strLen + 1];
          memcpy(name, luaString , strLen);
          name[strLen] = '\0';
          out << name;
          lua_pop(orb_state->luaState, 1);
          /* Obtenho a porta. */
          lua_pushnumber(orb_state->luaState, 2);
          lua_gettable(orb_state->luaState, -2);
          port = (unsigned short) lua_tointeger(orb_state->luaState, -1);
          out << "::" << port;
          lua_pop(orb_state->luaState, 1);
          //Openbus::logger->log(INFO, out.str());
          /* Armazenando par maquina/porta na lista de replicas.*/
          host = new Host;
          host->name = name;
          host->port = port;
          acsHosts.push_back(host);
          /* Removo a tabela que discrima a maquina e a porta. */
          lua_pop(orb_state->luaState, 1);
        }
        itACSHosts = acsHosts.begin();
      }
      else
      {
        //Openbus::logger->log(ERROR, "Ausência da tabela ACSHosts.");
      }    
    }
  }
  //Openbus::logger->dedent(INFO, 
  //"fault_tolerance_manager::loadConfig() END");
}

Host* fault_tolerance_manager::updateACSHostInUse()
{
  //Openbus::logger->log(INFO, 
  //"fault_tolerance_manager::updateACSHostInUse() BEGIN");
  //Openbus::logger->indent();
  std::stringstream out;
  out << "Replica ACS em uso: [" << orb_state->hostBus << "::" << orb_state->portBus << "]";
  //Openbus::logger->log(INFO, out.str());
  out.str(" ");
  if (acsHosts.size() > 0)
  {  
    /*
    * Se todo o conjunto foi percorrido sem se obter uma replica valida, 
    * verifica-se o numero maximo de tentativas disponiveis. Quando este
    * numero é alcançado, a falha não será  recuperada,  caso contrário,
    * uma nova tentativa é realizada em se percorrer todo o conjunto  em
    * busca de uma replica valida. 
    */
    itACSHosts++;
    if (itACSHosts == acsHosts.end())
    {
      //Openbus::logger->log(INFO, "Conjunto de replicas percorrido.");
      if (currTrial > trials)
      {
        out << "Numero de tentativas esgotado." << std::endl;
        //Openbus::logger->log(WARNING, out.str());
        out.str(" ");
        return 0;
      }
      else
      {
        out << "Número de tentativas realizadas: " << currTrial << std::endl;
        currTrial++;
        //Openbus::logger->log(INFO, out.str());
        out.str(" ");
        itACSHosts = acsHosts.begin();
      }
    } 
    acsHostInUse.name = (*itACSHosts)->name; 
    acsHostInUse.port = (*itACSHosts)->port; 
    out << "Nova replica ACS: [" << acsHostInUse.name  << "::" << acsHostInUse.port << "]";
    //Openbus::logger->log(INFO, out.str());
  }
  else
  {
    //Openbus::logger->log(WARNING, "Lista de replicas vazia.");
    return 0;
  }
  //Openbus::logger->dedent(INFO, 
  //"fault_tolerance_manager::updateACSHostInUse() END");
  return &acsHostInUse;
}

}
