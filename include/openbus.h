/**
* \mainpage API - SDK Openbus C++
* \file openbus.h
*/

#ifndef TECGRAF_OPENBUS_H_
#define TECGRAF_OPENBUS_H_

#include <CORBA.h>

/**
* \brief openbus
*/
namespace openbus {
  /**
	* Inicializa um ORB utilizado exclusivamente para uso dentro de barramentos OpenBus. É possível 
	* obter um ConnectionManager através do resolve_initial_reference("OpenBusConnectionManager").
	*
  * Uma instância de ORBInitializer é criada e um par de interceptadores cliente/servidor são 
  * registrados. O RootPOA é ativado.
  *
  * Atenção: O Mico 2.3.13 não permite a inicialização de mais de um ORB. Após uma primeira chamada 
  * bem sucedida de initORB(), todas as chamadas consecutivas retornarão o mesmo ORB que foi criado
  * na primeira chamada.
  * 
  * @param[in] argc Número de parâmetros usados na inicialização do ORB.
  * @param[in] argv Parâmetros usados na inicialização do ORB.
  * 
  * @throw CORBA::Exception
  *
  * @return O ORB inicializado.
	*/
  CORBA::ORB* initORB(int argc, char** argv) throw(CORBA::Exception);
}

#endif
