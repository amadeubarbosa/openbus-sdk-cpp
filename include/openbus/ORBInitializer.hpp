// -*- coding: iso-8859-1-unix -*-
/**
* \mainpage API - SDK Openbus C++
* \file openbus/ORBInitializer.h
*/

#ifndef TECGRAF_SDK_OPENBUS_ORB_INITIALIZER_H_
#define TECGRAF_SDK_OPENBUS_ORB_INITIALIZER_H_

#include "openbus/decl.hpp"

#include <tao/ORB.h>
#include <boost/shared_ptr.hpp>

/**
* \brief openbus
*/
namespace openbus 
{

/**
 * \brief Representa um ORB através de um invólucro.
 *
 * Representa um ORB através de um invólucro que compartilha a 
 * propriedade(ownership) sobre a instância do ORB. O destrutor 
 * do invólucro chama orb::destroy(). A instância do ORB pode 
 * ser obtida através do método orb().
 *
 */
class OPENBUS_SDK_DECL orb_ctx
{
public:
  /**
   * \brief Construtor que recebe um CORBA::ORB_var.
   */
  orb_ctx(CORBA::ORB_var orb);

  /**
   * \brief Destrutor que chama orb::destroy().
   */
  ~orb_ctx();

  /**
   * \brief Compartilha a instância do ORB.
   *
   * Retorna um smart pointer de CORBA referente a instância do ORB que é 
   * armazenada internamente.
   * 
   * @return CORBA::ORB_var
   */
  CORBA::ORB_var orb() const
  {
    return orb_;
  }
private:
  orb_ctx &operator=(const orb_ctx &);
  CORBA::ORB_var orb_;
};

/**
 * \brief Inicializa um ORB utilizado exclusivamente para chamadas
 *        através de barramentos OpenBus.
 *
 * Inicializa um ORB utilizado exclusivamente para chamadas através de
 * barramentos OpenBus, ou seja, esse ORB não pode ser utilizado para
 * fazer chamadas CORBA ordinárias sem o controle de acesso do OpenBus
 * que permite identificação da origem das chamadas. Esse controle de
 * acesso é feito através conexões que são obtidas e manipuladas
 * através de um OpenBusContext. Cada ORB possui um OpenBusContext
 * associado, que pode ser obitido através do comando:
 * CORBA::ORB::resolve_initial_reference("OpenBusContext")
 *
 * O ORB é inicializado da mesma forma feita pela operação
 * 'CORBA::ORB_init' definida pelo padrão CORBA. 
 * 
 * Chamadas realizadas e recebidas através deste ORB são interceptadas
 * pela biblioteca de acesso do OpenBus e podem lançar exceções de
 * sistema de CORBA definidas pelo OpenBus. A seguir são apresentadas
 * essas exceções: 
 *  - CORBA::NO_PERMISSION{NoLogin}: Nenhuma conexão
 *    "Requester" com login válido está associada ao contexto atual, ou
 *     seja, a conexão "Requester" corrente está desautenticada.  
 *  - CORBA::NO_PERMISSION{InvalidChain}: A cadeia de chamadas associada
 *     ao contexto atual não é compatível com o login da conexão
 *     "Requester" desse mesmo contexto. Isso ocorre pois não é possível
 *     fazer chamadas dentro de uma cadeia recebida por uma conexão com um
 *     login diferente.  - CORBA::NO_PERMISSION{UnknownBus}: O ORB remoto
 *     que recebeu a chamada indicou que não possui uma conexão com login
 *     válido no barramento através do qual a chamada foi realizada,
 *     portanto não é capaz de validar a chamada para que esta seja
 *     processada.  
 *  - CORBA::NO_PERMISSION{UnverifiedLogin}: O ORB remoto
 *    que recebeu a chamada indicou que não é capaz de validar a chamada
 *    para que esta seja processada. Isso indica que o lado remoto tem
 *    problemas de acesso aos serviços núcleo do barramento.  
 *  - CORBA::NO_PERMISSION{InvalidRemote}: O ORB remoto que recebeu a
 *    chamada não está se comportando de acordo com o protocolo OpenBus
 *    2.0, o que indica que está mal implementado e tipicamente
 *    representa um bug no servidor sendo chamado ou um erro de
 *    implantação do barramento.
 * 
 * @param[in] argc Número de parâmetros usados na inicialização do ORB.
 * @param[in] argv Parâmetros usados na inicialização do ORB.
 * 
 * @throw CORBA::Exception
 *
 * @return Uma referencia do tipo boost::shared_ptr para um 
 * invólucro que contêm o ORB inicializado.
 */
  OPENBUS_SDK_DECL boost::shared_ptr<orb_ctx>
  ORBInitializer(int &argc, char **argv);
}

#endif
