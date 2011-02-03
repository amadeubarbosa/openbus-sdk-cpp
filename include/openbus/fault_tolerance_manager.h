/*
** FaultToleranceManager.h
*/

#ifndef OPENBUS_SDK_FAULT_TOLERANCE_MANAGER_H_
#define OPENBUS_SDK_FAULT_TOLERANCE_MANAGER_H_

#include <list>

namespace openbus {

struct orb_state;

struct Host
{
  char* name;
  unsigned short port;
};
      
class fault_tolerance_manager
{
private:
  openbus::orb_state* orb_state;

  /**
   * Maquina que contem a replica que esta sendo usada.
   */
  Host acsHostInUse;

  /**
   * Lista das replicas disponiveis do ACS.
   */
  std::list<Host*> acsHosts;
  std::list<Host*>::iterator itACSHosts;
      
  /**
   * Numero máximo de tentativas de se obter uma replica valida 
   * do  conjunto  de  replicas.  Conta-se como uma tentativa o 
   * percorrimento de todo o conjunto sem que se consiga  obter 
   * uma replica valida.
   */
  short trials;
      
  /**
   * Numero  de  tentativas realizadas de se obter uma replica
   * valida do conjunto de replicas.
   */
  short currTrial;
      
public:
  fault_tolerance_manager(openbus::orb_state& orb_state);
  ~fault_tolerance_manager();

  /**
   * Carrega o arquivo de configuracao das replicas.
   *
   * @param[in] filename Nome do arquivo de configuracao.
   */
  void loadConfig(char* filename);

  /**
   * Substitui a referência do ACS atual por uma outra replica 
   * do ACS que esteja na lista de replicas.
   */
  Host* updateACSHostInUse();
};

}

#endif

