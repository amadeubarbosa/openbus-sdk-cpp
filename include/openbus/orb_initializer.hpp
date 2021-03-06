// -*- coding: iso-8859-1-unix -*-
/**
  \mainpage API do OpenBus SDK C++
  
  Este documento apresenta uma API C++ para o acesso ao barramento
  OpenBus. O conte�do aqui apresentado foi extra�do automaticamente
  dos fontes da biblioteca. 

  O ponto de entrada da interface � a classe \ref
  openbus::ORBInitializer, que retorna um inv�lucro para o
  ORB. Atrav�s do ORB, � poss�vel obter a inst�ncia para \ref
  openbus::OpenBusContext, que permite controlar o contexto das
  chamadas de um ORB para acessar informa��es que identificam essas
  chamadas em barramentos OpenBus.

  <a href="https://jira.tecgraf.puc-rio.br/confluence/pages/viewpage.action?pageId=67600726">Manual de uso do n�cleo do OpenBus 2.1</a>

  <a href="https://jira.tecgraf.puc-rio.br/confluence/display/OPENBUS021/CORE">Download do barramento OpenBus 2.1</a>
 
  \file openbus/orb_initializer.hpp
*/

#ifndef TECGRAF_SDK_OPENBUS_OPENBUS_ORB_INITIALIZER_HPP
#define TECGRAF_SDK_OPENBUS_OPENBUS_ORB_INITIALIZER_HPP

#include "openbus/detail/decl.hpp"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <tao/ORB.h>
#pragma clang diagnostic pop
#include <boost/shared_ptr.hpp>

/**
* \brief Namespace para a biblioteca de acesso.
*/
namespace openbus {

/**
 * \class orb_ctx
 * \brief 
 * Representa um ORB atrav�s de um inv�lucro que assume a 
 * propriedade(ownership) sobre o ORB. O destrutor do inv�lucro chama
 * CORBA::ORB::destroy(). A inst�ncia do ORB pode ser obtida atrav�s do
 * m�todo \ref orb.
 *
 */
class OPENBUS_SDK_DECL orb_ctx
{
public:
  /**
   * \brief Construtor que recebe um 'smart pointer' de CORBA 
   * para a inst�ncia do ORB(CORBA::ORB_var).
   */
  orb_ctx(CORBA::ORB_var orb);

  /**
   * \brief Destrutor que chama orb::destroy().
   */
  ~orb_ctx();

  /**
   * \brief Compartilha a inst�ncia do ORB.
   *
   * Retorna um 'smart pointer' para a inst�ncia do ORB que � 
   * referenciado internamente.
   * 
   * \return CORBA::ORB_var
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
 * Este ORB n�o pode ser utilizado para fazer chamadas CORBA
 * ordin�rias sem o controle de acesso do OpenBus que permite
 * identifica��o da origem das chamadas. Esse controle de acesso �
 * feito atrav�s de conex�es, que s�o obtidas e manipuladas atrav�s de
 * um \ref OpenBusContext. O ORB possui um \ref OpenBusContext
 * associado, que pode ser obitido atrav�s da chamada: 
 * \code
 * CORBA::ORB::resolve_initial_reference("OpenBusContext") 
 * \endcode
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
 *    ao contexto atual n�o � compat�vel com o login da conex�o
 *    "Requester" desse mesmo contexto. Isso ocorre pois n�o � poss�vel
 *    fazer chamadas dentro de uma cadeia recebida por uma conex�o com um
 *    login diferente.  
 *  - CORBA::NO_PERMISSION{UnknownBus}: O ORB remoto
 *    que recebeu a chamada indicou que n�o possui uma conex�o com login
 *    v�lido no barramento atrav�s do qual a chamada foi realizada,
 *    portanto n�o � capaz de validar a chamada para que esta seja
 *    processada.  
 *  - CORBA::NO_PERMISSION{UnverifiedLogin}: O ORB remoto
 *    que recebeu a chamada indicou que n�o � capaz de validar a chamada
 *    para que esta seja processada. Isso indica que o lado remoto tem
 *    problemas de acesso aos servi�os n�cleo do barramento.  
 *  - CORBA::NO_PERMISSION{InvalidRemote}: O ORB remoto que recebeu a
 *    chamada n�o est� se comportando de acordo com o protocolo OpenBus
 *    2.1, o que indica que est� mal implementado e tipicamente
 *    representa um bug no servidor sendo chamado ou um erro de
 *    implanta��o do barramento.
 * 
 * \param argc N�mero de par�metros usados na inicializa��o do ORB.
 * \param argv Par�metros usados na inicializa��o do ORB.
 * 
 * \throw CORBA::Exception
 *
 * \return Um 'smart pointer' do tipo \ref std::auto_ptr para um 
 * inv�lucro que cont�m o ORB inicializado.
 */
  OPENBUS_SDK_DECL std::auto_ptr<orb_ctx>
  ORBInitializer(int &argc, char **argv);
}

#endif
