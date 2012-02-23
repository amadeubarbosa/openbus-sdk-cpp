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
  * Inicializa o ORB para ser usado na conex�o com um barramento OpenBus.
  * Todos os acessos a servi�os e objetos em um barramento devem ser feitos
  * atrav�s do ORB que � retornado por esta fun��o.
  * 
  * @param[in] argc
  * @param[in] argv
  * 
  * @return ORB iniciado.
  */  
  ORB* createORB(int argc, char** argv) throw(CORBA::Exception);
  
  /**
  * Cria uma conex�o para um barramento a partir de um endere�o de rede IP e
  * uma porta.
  * 
  * @param [in] host Endere�o de rede IP onde o barramento est� executando.
  * @param [in] port Porta do processo do barramento no endere�o indicado.
  * @param [in] orb ORB a ser utilizado na cria��o da refer�ncia. Se o valor desse
  *             par�metro for 'null' um ORB � inicializado com configura��es
  *             default a ser utilizado na cria��o da refer�ncia.
  * 
  * @return Conex�o ao barramento referenciado.
  */
  Connection* connect(const std::string host, const unsigned int port, ORB* orb = 0)
    throw(CORBA::Exception);
}

#endif
