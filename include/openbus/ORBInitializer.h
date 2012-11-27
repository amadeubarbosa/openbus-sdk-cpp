// -*- coding: iso-8859-1 -*-
/**
* \mainpage API - SDK Openbus C++
* \file openbus/ORBInitializer.h
*/

#ifndef TECGRAF_OPENBUS_H_
#define TECGRAF_OPENBUS_H_

#include <CORBA.h>

/**
* \brief openbus
*/
namespace openbus {
  /**
   * \brief Inicializa um ORB utilizado exclusivamente para chamadas através de barramentos
   *        OpenBus.
   *
   * Inicializa um ORB utilizado exclusivamente para chamadas através de barramentos OpenBus, ou
   * seja, esse ORB não pode ser utilizado para fazer chamadas CORBA ordinárias sem o controle de
   * acesso do OpenBus que permite identificação da origem das chamadas. Esse controle de acesso é
   * feito através conexões que são obtidas e manipuladas através de um OpenBusContext. Cada ORB
   * possui um OpenBusContext associado, que pode ser obitido através do comando:
   * CORBA::ORB::resolve_initial_reference("OpenBusContext")
   *
   * O ORB é inicializado da mesma forma feita pela operação 'CORBA::ORB_init' definida pelo padrão
   * CORBA. Em particular, algumas implementações de CORBA não permitem inicialização de múltiplos
   * ORBs num mesmo processo, como por exemplo o ORB Mico 2.3.13.
   * 
   * Chamadas realizadas e recebidas através deste ORB são interceptadas pela biblioteca de acesso
   * do OpenBus e podem lançar exceções de sistema de CORBA definidas pelo OpenBus. A seguir são
   * apresentadas essas exceções: 
   *   - CORBA::NO_PERMISSION{NoLogin}: Nenhuma conexão "Requester" com login válido está associada 
   *   ao contexto atual, ou seja, a conexão "Requester" corrente está desautenticada.  
   *   - CORBA::NO_PERMISSION{InvalidChain}: A cadeia de chamadas associada ao contexto atual não é
   *   compatível com o login da conexão "Requester" desse mesmo contexto. Isso ocorre pois não é
   *   possível fazer chamadas dentro de uma cadeia recebida por uma conexão com um login diferente.
   *   - CORBA::NO_PERMISSION{UnknownBus}: O ORB remoto que recebeu a chamada indicou que não possui
   *   uma conexão com login válido no barramento através do qual a chamada foi realizada, portanto 
   *   não é capaz de validar a chamada para que esta seja processada.  
   *   - CORBA::NO_PERMISSION{UnverifiedLogin}: O ORB remoto que recebeu a chamada indicou que não é
   *   capaz de validar a chamada para que esta seja processada. Isso indica que o lado remoto tem
   *   problemas de acesso aos serviços núcleo do barramento.  
   *   - CORBA::NO_PERMISSION{InvalidRemote}: O ORB remoto que recebeu a chamada não está se 
   *   comportando de acordo com o protocolo OpenBus 2.0, o que indica que está mal implementado e
   *   tipicamente representa um bug no servidor sendo chamado ou um erro de implantação do 
   *   barramento.
   * 
   * @return O ORB inicializado, similar à operação 'CORBA::ORB_init' definida pelo padrão CORBA.
   *
   * Atenção: O Mico 2.3.13 não permite a inicialização de mais de um ORB. Após uma primeira
   *          chamada bem sucedida de ORBInitializer(), todas as chamadas consecutivas retornarão o
   *          mesmo ORB que foi criado na primeira chamada.
   * 
   * @param[in] argc Número de parâmetros usados na inicialização do ORB.
   * @param[in] argv Parâmetros usados na inicialização do ORB.
   * 
   * @throw CORBA::Exception
   *
   * @return O ORB inicializado.
   */
  CORBA::ORB *ORBInitializer(int& argc, char **argv);
}

#endif
