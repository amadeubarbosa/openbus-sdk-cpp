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
 * \brief Representa um ORB atrav�s de um inv�lucro.
 *
 * Representa um ORB atrav�s de um inv�lucro que compartilha a 
 * propriedade(ownership) sobre a inst�ncia do ORB. O destrutor 
 * do inv�lucro chama orb::destroy(). A inst�ncia do ORB pode 
 * ser obtida atrav�s do m�todo orb().
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
   * \brief Compartilha a inst�ncia do ORB.
   *
   * Retorna um smart pointer de CORBA referente a inst�ncia do ORB que � 
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
 *        atrav�s de barramentos OpenBus.
 *
 * Inicializa um ORB utilizado exclusivamente para chamadas atrav�s de
 * barramentos OpenBus, ou seja, esse ORB n�o pode ser utilizado para
 * fazer chamadas CORBA ordin�rias sem o controle de acesso do OpenBus
 * que permite identifica��o da origem das chamadas. Esse controle de
 * acesso � feito atrav�s conex�es que s�o obtidas e manipuladas
 * atrav�s de um OpenBusContext. Cada ORB possui um OpenBusContext
 * associado, que pode ser obitido atrav�s do comando:
 * CORBA::ORB::resolve_initial_reference("OpenBusContext")
 *
 * O ORB � inicializado da mesma forma feita pela opera��o
 * 'CORBA::ORB_init' definida pelo padr�o CORBA. 
 * 
 * Chamadas realizadas e recebidas atrav�s deste ORB s�o interceptadas
 * pela biblioteca de acesso do OpenBus e podem lan�ar exce��es de
 * sistema de CORBA definidas pelo OpenBus. A seguir s�o apresentadas
 * essas exce��es: 
 *  - CORBA::NO_PERMISSION{NoLogin}: Nenhuma conex�o
 *    "Requester" com login v�lido est� associada ao contexto atual, ou
 *     seja, a conex�o "Requester" corrente est� desautenticada.  
 *  - CORBA::NO_PERMISSION{InvalidChain}: A cadeia de chamadas associada
 *     ao contexto atual n�o � compat�vel com o login da conex�o
 *     "Requester" desse mesmo contexto. Isso ocorre pois n�o � poss�vel
 *     fazer chamadas dentro de uma cadeia recebida por uma conex�o com um
 *     login diferente.  - CORBA::NO_PERMISSION{UnknownBus}: O ORB remoto
 *     que recebeu a chamada indicou que n�o possui uma conex�o com login
 *     v�lido no barramento atrav�s do qual a chamada foi realizada,
 *     portanto n�o � capaz de validar a chamada para que esta seja
 *     processada.  
 *  - CORBA::NO_PERMISSION{UnverifiedLogin}: O ORB remoto
 *    que recebeu a chamada indicou que n�o � capaz de validar a chamada
 *    para que esta seja processada. Isso indica que o lado remoto tem
 *    problemas de acesso aos servi�os n�cleo do barramento.  
 *  - CORBA::NO_PERMISSION{InvalidRemote}: O ORB remoto que recebeu a
 *    chamada n�o est� se comportando de acordo com o protocolo OpenBus
 *    2.0, o que indica que est� mal implementado e tipicamente
 *    representa um bug no servidor sendo chamado ou um erro de
 *    implanta��o do barramento.
 * 
 * @param[in] argc N�mero de par�metros usados na inicializa��o do ORB.
 * @param[in] argv Par�metros usados na inicializa��o do ORB.
 * 
 * @throw CORBA::Exception
 *
 * @return Uma referencia do tipo boost::shared_ptr para um 
 * inv�lucro que cont�m o ORB inicializado.
 */
  OPENBUS_SDK_DECL boost::shared_ptr<orb_ctx>
  ORBInitializer(int &argc, char **argv);
}

#endif
