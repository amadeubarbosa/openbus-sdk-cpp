#ifndef TECGRAF_OPENBUS_H_
#define TECGRAF_OPENBUS_H_

#include "connection.h"
#include <CORBA.h>
#include <string>

namespace openbus {
  /**
  * Smart pointer que representa um ORB.
  */
  class ORB {
    public:
      ~ORB();
      CORBA::ORB* orb() const
        { return _orb; }
      friend ORB* createORB(int argc, char** argv) throw(CORBA::Exception);
    private:
      ORB(int argc, char** argv);
      CORBA::ORB_var _orb;
  };
  
  /**
  * Inicializa o ORB para ser usado na conexão com um barramento OpenBus.
  * Todos os acessos a serviços e objetos em um barramento devem ser feitos
  * através do ORB que é retornado por esta função.
  * 
  * @param[in] argc
  * @param[in] argv
  * 
  * @return ORB iniciado.
  */  
  ORB* createORB(int argc, char** argv) throw(CORBA::Exception);
  
  /**
  * Cria uma conexão para um barramento a partir de um endereço de rede IP e
  * uma porta.
  * 
  * @param [in] host Endereço de rede IP onde o barramento está executando.
  * @param [in] port Porta do processo do barramento no endereço indicado.
  * @param [in] orb ORB a ser utilizado na criação da referência. Se o valor desse
  *             parâmetro for 'null' um ORB é inicializado com configurações
  *             default a ser utilizado na criação da referência.
  * 
  * @return Conexão ao barramento referenciado.
  */
  Connection* connect(const std::string host, const unsigned int port, ORB* orb = 0)
    throw(CORBA::Exception);
}

#endif
